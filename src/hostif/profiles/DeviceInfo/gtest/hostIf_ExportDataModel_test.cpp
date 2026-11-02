#include "hostIf_ExportDataModel.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <chrono>
#include <thread>

#include <gtest/gtest.h>
#include "Device_DeviceInfo.h"

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

int countUnescapedPipes(const std::string &line)
{
    int count = 0;
    bool escaped = false;
    for (std::string::const_iterator character = line.begin(); character != line.end(); ++character)
    {
        if (escaped)
        {
            escaped = false;
        }
        else if (*character == '\\')
        {
            escaped = true;
        }
        else if (*character == '|')
        {
            ++count;
        }
    }
    return count;
}

bool waitForExport(const char *path)
{
    for (int attempt = 0; attempt < 200; ++attempt)
    {
        std::ifstream file(path);
        if (file.good() && file.peek() != std::ifstream::traits_type::eof())
            return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    return false;
}

int setExportTrigger(bool enabled)
{
    HOSTIF_MsgData_t message = {};
    message.reqType = HOSTIF_SET;
    message.requestor = HOSTIF_SRC_RFC;
    message.paramtype = hostIf_BooleanType;
    strncpy(message.paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.GetDataModelList",
            sizeof(message.paramName) - 1);
    put_boolean(message.paramValue, enabled);
    message.paramLen = strlen(message.paramValue);
    return hostIf_DeviceInfo::getInstance(0)->set_xRDKCentralComRFC(&message);
}
}

TEST(ExportDataModel, FormatterSortsAndEscapesFields)
{
    char zName[] = "Device.Z";
    char zType[] = "string";
    char zDefault[] = "";
    char zAccess[] = "readOnly";
    char zDescription[] = "pipe|line\nreturn\rslash\\";
    char aName[] = "Device.A";
    char aType[] = "boolean";
    char aDefault[] = "false";
    char aAccess[] = "readWrite";
    char aDescription[] = "";
    DataModelExportParam params[] = {
        {zName, zType, zDefault, zAccess, zDescription},
        {aName, aType, aDefault, aAccess, aDescription}
    };
    std::string output;

    ASSERT_EQ(EXPORT_DATA_MODEL_SUCCESS, formatParametersAsPipeDelimited(params, 2, output));
    EXPECT_EQ("name|dataType|defaultValue|access|description\n"
              "Device.A|boolean|false|readWrite|\n"
              "Device.Z|string||readOnly|pipe\\|line\\nreturn\\rslash\\\\\n",
              output);
    EXPECT_EQ(EXPORT_DATA_MODEL_ERROR_INVALID_ARGUMENT,
              formatParametersAsPipeDelimited(NULL, 0, output));
}

TEST(ExportDataModel, SnapshotIncludesRFCTriggerMetadata)
{
    ASSERT_EQ(DB_SUCCESS, loadDataModel());

    DataModelExportParam *params = NULL;
    int count = 0;
    ASSERT_EQ(DB_SUCCESS, getDataModelExportSnapshot(&params, &count));
    SnapshotGuard cleanup(params, count);
    ASSERT_GT(count, 0);

    const char *expectedName = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.GetDataModelList";
    const DataModelExportParam *trigger = NULL;
    const DataModelExportParam *parameterWithoutOptionalMetadata = NULL;
    bool foundTableTemplate = false;
    for (int index = 0; index < count; ++index)
    {
        if (params[index].name && !strcmp(params[index].name, expectedName))
        {
            trigger = &params[index];
        }
        if (params[index].name &&
            !strcmp(params[index].name, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ClearDB"))
            parameterWithoutOptionalMetadata = &params[index];
        if (params[index].name && strstr(params[index].name, "{i}"))
            foundTableTemplate = true;
    }

    ASSERT_NE(static_cast<const DataModelExportParam *>(NULL), trigger);
    EXPECT_STREQ("boolean", trigger->dataType);
    EXPECT_STREQ("false", trigger->defaultValue);
    EXPECT_STREQ("readWrite", trigger->access);
    EXPECT_STREQ("Generate an export of the loaded TR-181 data model.", trigger->description);
    ASSERT_NE(static_cast<const DataModelExportParam *>(NULL), parameterWithoutOptionalMetadata);
    EXPECT_STREQ("", parameterWithoutOptionalMetadata->defaultValue);
    EXPECT_STREQ("", parameterWithoutOptionalMetadata->description);
    EXPECT_TRUE(foundTableTemplate);
}

TEST(ExportDataModel, WritesSortedFiveFieldOutputWithWorldReadableMode)
{
    ASSERT_EQ(DB_SUCCESS, loadDataModel());
    const char *outputPath = "/tmp/supported_datamodel_list_unit_test";
    std::remove(outputPath);

    ASSERT_EQ(EXPORT_DATA_MODEL_SUCCESS, exportDataModelToText(outputPath));

    std::ifstream input(outputPath);
    ASSERT_TRUE(input.is_open());
    std::string content((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    input.close();

    const std::string header = "name|dataType|defaultValue|access|description\n";
    ASSERT_EQ(0, content.compare(0, header.size(), header));

    std::vector<std::string> names;
    std::string line;
    std::istringstream lines(content.substr(header.size()));
    while (std::getline(lines, line))
    {
        EXPECT_EQ(4, countUnescapedPipes(line));
        names.push_back(line.substr(0, line.find('|')));
    }
    ASSERT_FALSE(names.empty());
    EXPECT_TRUE(std::is_sorted(names.begin(), names.end()));

    struct stat fileInfo;
    ASSERT_EQ(0, stat(outputPath, &fileInfo));
    EXPECT_EQ(0644, fileInfo.st_mode & 0777);
    std::remove(outputPath);
}

TEST(ExportDataModel, FalseRFCSetDoesNotCreateExport)
{
    ASSERT_EQ(DB_SUCCESS, loadDataModel());
    std::remove(SUPPORTED_DATAMODEL_LIST_PATH);

    EXPECT_EQ(OK, setExportTrigger(false));
    std::ifstream output(SUPPORTED_DATAMODEL_LIST_PATH);
    EXPECT_FALSE(output.good());
}

TEST(ExportDataModel, TrueRFCSetSchedulesExport)
{
    ASSERT_EQ(DB_SUCCESS, loadDataModel());
    std::remove(SUPPORTED_DATAMODEL_LIST_PATH);

    ASSERT_EQ(OK, setExportTrigger(true));
    EXPECT_TRUE(waitForExport(SUPPORTED_DATAMODEL_LIST_PATH));
    std::remove(SUPPORTED_DATAMODEL_LIST_PATH);
}