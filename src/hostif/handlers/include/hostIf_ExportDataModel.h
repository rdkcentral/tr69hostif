#ifndef HOSTIF_EXPORT_DATA_MODEL_H
#define HOSTIF_EXPORT_DATA_MODEL_H

#include <string>

#include "waldb.h"

static const char SUPPORTED_DATAMODEL_LIST_PATH[] = "/opt/datamodel.list";

typedef enum
{
    EXPORT_DATA_MODEL_SUCCESS = 0,
    EXPORT_DATA_MODEL_ERROR_INVALID_ARGUMENT = -1,
    EXPORT_DATA_MODEL_ERROR_XML_NOT_LOADED = -2,
    EXPORT_DATA_MODEL_ERROR_SNAPSHOT = -3,
    EXPORT_DATA_MODEL_ERROR_WRITE = -4,
    EXPORT_DATA_MODEL_ERROR_WORKER = -5
} ExportDataModelStatus;

int exportDataModelToText(const char *outputPath = SUPPORTED_DATAMODEL_LIST_PATH);
int scheduleDataModelExport();
int formatParametersAsPipeDelimited(DataModelExportParam *params, int count, std::string &output);

#endif