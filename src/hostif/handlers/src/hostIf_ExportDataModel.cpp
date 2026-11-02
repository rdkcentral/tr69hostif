#include "hostIf_ExportDataModel.h"

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <mutex>
#include <string>
#include <thread>
#include <condition_variable>
#include <unistd.h>
#include <vector>
#include <sys/stat.h>

#include "libIBus.h"
#include "hostIf_main.h"
#include "waldb.h"

namespace
{
class SnapshotGuard
{
public:
    SnapshotGuard(DataModelExportParam *params, int count) : m_params(params), m_count(count) {}
    ~SnapshotGuard() { freeDataModelExportSnapshot(m_params, m_count); }

private:
    DataModelExportParam *m_params;
    int m_count;
};

std::string escapeField(const char *field)
{
    std::string escaped;
    for (const char *character = field ? field : ""; *character; ++character)
    {
        switch (*character)
        {
            case '\\': escaped += "\\\\"; break;
            case '|': escaped += "\\|"; break;
            case '\r': escaped += "\\r"; break;
            case '\n': escaped += "\\n"; break;
            default: escaped += *character; break;
        }
    }
    return escaped;
}

std::string formatSnapshot(DataModelExportParam *params, int count)
{
    std::sort(params, params + count, [](const DataModelExportParam &left, const DataModelExportParam &right) {
        return std::strcmp(left.name ? left.name : "", right.name ? right.name : "") < 0;
    });

    std::string output("name|dataType|defaultValue|access|description\n");
    for (int index = 0; index < count; ++index)
    {
        const char *fields[] = {params[index].name, params[index].dataType, params[index].defaultValue,
                                params[index].access, params[index].description};
        for (size_t field = 0; field < sizeof(fields) / sizeof(fields[0]); ++field)
        {
            if (field)
                output += '|';
            output += escapeField(fields[field]);
        }
        output += '\n';
    }
    return output;
}

bool writeAll(int descriptor, const std::string &content)
{
    const char *cursor = content.data();
    size_t remaining = content.size();
    while (remaining > 0)
    {
        ssize_t written = write(descriptor, cursor, remaining);
        if (written < 0 && errno == EINTR)
            continue;
        if (written <= 0)
        {
            if (written == 0)
                errno = EIO;
            return false;
        }

        size_t bytesWritten = static_cast<size_t>(written);
        if (bytesWritten > remaining)
        {
            errno = EIO;
            return false;
        }
        cursor += bytesWritten;
        remaining -= bytesWritten;
    }
    return true;
}

class ExportWorker
{
public:
    ExportWorker() : m_stopping(false), m_pending(false) {}

    ~ExportWorker()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_stopping = true;
            m_pending = false;
        }
        m_condition.notify_one();
        if (m_thread.joinable())
            m_thread.join();
    }

    int schedule()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_stopping)
            return EXPORT_DATA_MODEL_ERROR_WORKER;

        if (!m_thread.joinable())
        {
            try
            {
                m_thread = std::thread(&ExportWorker::run, this);
            }
            catch (...)
            {
                return EXPORT_DATA_MODEL_ERROR_WORKER;
            }
        }

        m_pending = true;
        m_condition.notify_one();
        return EXPORT_DATA_MODEL_SUCCESS;
    }

private:
    void run()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (true)
        {
            m_condition.wait(lock, [this]() { return m_stopping || m_pending; });
            if (m_stopping)
                return;

            m_pending = false;
            lock.unlock();
            int status = exportDataModelToText();
            if (status != EXPORT_DATA_MODEL_SUCCESS)
            {
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,
                        "[%s] Data model export failed with status %d\n", __FUNCTION__, status);
            }
            lock.lock();
        }
    }

    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::thread m_thread;
    bool m_stopping;
    bool m_pending;
};

ExportWorker g_exportWorker;
}

int formatParametersAsPipeDelimited(DataModelExportParam *params, int count, std::string &output)
{
    if (!params || count <= 0)
        return EXPORT_DATA_MODEL_ERROR_INVALID_ARGUMENT;

    try
    {
        output = formatSnapshot(params, count);
    }
    catch (...)
    {
        output.clear();
        return EXPORT_DATA_MODEL_ERROR_SNAPSHOT;
    }
    return EXPORT_DATA_MODEL_SUCCESS;
}

int exportDataModelToText(const char *outputPath)
{
    if (!outputPath || !*outputPath)
        outputPath = SUPPORTED_DATAMODEL_LIST_PATH;

    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] Starting export to %s\n", __FUNCTION__, outputPath);

    DataModelExportParam *params = NULL;
    int count = 0;
    DB_STATUS snapshotStatus = getDataModelExportSnapshot(&params, &count);
    if (snapshotStatus != DB_SUCCESS)
    {
        freeDataModelExportSnapshot(params, count);
        if (snapshotStatus == DB_ERR_NOT_EXIST)
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Data model is not loaded\n", __FUNCTION__);
            return EXPORT_DATA_MODEL_ERROR_XML_NOT_LOADED;
        }
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Failed to collect data model metadata\n", __FUNCTION__);
        return EXPORT_DATA_MODEL_ERROR_SNAPSHOT;
    }
    if (!params || count <= 0)
    {
        freeDataModelExportSnapshot(params, count);
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Data model snapshot is empty\n", __FUNCTION__);
        return EXPORT_DATA_MODEL_ERROR_SNAPSHOT;
    }
    SnapshotGuard snapshotGuard(params, count);

    try
    {
        std::string content;
        if (formatParametersAsPipeDelimited(params, count, content) != EXPORT_DATA_MODEL_SUCCESS)
            return EXPORT_DATA_MODEL_ERROR_SNAPSHOT;
        std::string temporaryPath(outputPath);
        temporaryPath += ".tmp.XXXXXX";
        std::vector<char> tempName(temporaryPath.begin(), temporaryPath.end());
        tempName.push_back('\0');

        int descriptor = mkstemp(tempName.data());
        if (descriptor < 0)
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] mkstemp failed for %s: %s\n",
                    __FUNCTION__, outputPath, strerror(errno));
            return EXPORT_DATA_MODEL_ERROR_WRITE;
        }

        bool writeSucceeded = fchmod(descriptor, 0644) == 0 && writeAll(descriptor, content) && fsync(descriptor) == 0;
        int savedError = errno;
        if (close(descriptor) != 0 && writeSucceeded)
        {
            writeSucceeded = false;
            savedError = errno;
        }

        if (!writeSucceeded)
        {
            if (unlink(tempName.data()) != 0 && errno != ENOENT)
            {
                int cleanupError = errno;
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Failed to remove temporary export file %s: %s\n",
                        __FUNCTION__, tempName.data(), strerror(cleanupError));
            }
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Writing %s failed: %s\n",
                    __FUNCTION__, outputPath, strerror(savedError));
            return EXPORT_DATA_MODEL_ERROR_WRITE;
        }

        if (rename(tempName.data(), outputPath) != 0)
        {
            savedError = errno;
            if (unlink(tempName.data()) != 0 && errno != ENOENT)
            {
                int cleanupError = errno;
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Failed to remove temporary export file %s: %s\n",
                        __FUNCTION__, tempName.data(), strerror(cleanupError));
            }
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Renaming export to %s failed: %s\n",
                    __FUNCTION__, outputPath, strerror(savedError));
            return EXPORT_DATA_MODEL_ERROR_WRITE;
        }
    }
    catch (...)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Failed to format data model export\n", __FUNCTION__);
        return EXPORT_DATA_MODEL_ERROR_SNAPSHOT;
    }

    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] Exported %d parameters to %s\n",
            __FUNCTION__, count, outputPath);
    return EXPORT_DATA_MODEL_SUCCESS;
}

int scheduleDataModelExport()
{
    return g_exportWorker.schedule();
}