/*
 * Copyright 2024 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include "dm_stubs.h"

#include "waldb.h"
#include "wdmp-c.h"

using namespace std;

#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "datamodel_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 128

TEST(datamodelTest, ParameterExistPositive2) {

    /* Load the data model xml file*/
    DB_STATUS status = loadDataModel();
    if(status != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(status, DB_SUCCESS);

    DataModelParam dmParam = {0};
    WDMP_STATUS ret = WDMP_SUCCESS;

    const char* dbParamName = "Device.Time.NTPServer1";
    int match = getParamInfoFromDataModel(getDataModelHandle(), dbParamName, &dmParam);
    if(!match)
    {
        std::cout << "Invalid parameter name "<< dbParamName << " doesn't exist in data-model." << std::endl;
    }
    else
    {
        std::cout << "Parameter "<< dbParamName << " exist in data-model! Type = " << dmParam.dataType << std::endl;
    }
    EXPECT_NE(match, 0);
}

TEST(datamodelTest, ParameterExistNegative) {

    /* Load the data model xml file*/
    DB_STATUS status = loadDataModel();
    if(status != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(status, DB_SUCCESS);

    DataModelParam dmParam = {0};
    WDMP_STATUS ret = WDMP_SUCCESS;

    const char* dbParamName = "Device.Time.NTPServer6";
    int match = getParamInfoFromDataModel(getDataModelHandle(), dbParamName, &dmParam);
    if(!match)
    {
        std::cout << "Invalid parameter name "<< dbParamName << " doesn't exist in data-model." << std::endl;
    }
    else
    {
        std::cout << "Parameter "<< dbParamName << " exist in data-model! Type = " << dmParam.dataType << std::endl;
    }
    EXPECT_EQ(match, 0);
}

TEST(datamodelTest, ParameterExistPositive) {

    /* Load the data model xml file*/
    DB_STATUS status = loadDataModel();
    if(status != DB_SUCCESS)
    {
        std::cout << "Error in Data Model Initialization" << std::endl;
    }
    else
    {
        std::cout << "Successfully initialize Data Model." << std::endl;
    }
    EXPECT_EQ(status, DB_SUCCESS);

    DataModelParam dmParam = {0};
    WDMP_STATUS ret = WDMP_SUCCESS;

    const char* dbParamName = "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SHORTS.Enable";
    int match = getParamInfoFromDataModel(getDataModelHandle(), dbParamName, &dmParam);
    if(!match)
    {
        std::cout << "Invalid parameter name "<< dbParamName << " doesn't exist in data-model\n" << std::endl;
    }
    else
    {
        std::cout << "Parameter "<< dbParamName << " exist in data-model! Type = \n" << dmParam.dataType << std::endl;
    }
    EXPECT_NE(match, 0);
}

TEST(datamodelTest, ParameterListTest) {
    
    char **pParam_name_list;

    pParam_name_list =  (char**) calloc (2048, sizeof(char*));
    int num_of_params = 0;

    if(DB_SUCCESS != loadDataModel())
    {
        std::cout << "Failed to load data-model.xml file" << std::endl;
    }

    DB_STATUS status = get_complete_param_list (pParam_name_list, &num_of_params);
    if(DB_SUCCESS != status)
    {
        std::cout << "Failed to get complete parameter list."<< std::endl;
    }
    EXPECT_EQ(status, DB_SUCCESS);

    std::cout << "Count = " << num_of_params<< std::endl;
    EXPECT_NE(num_of_params, 0);
    
    int i = 0;
    std::cout << "======================================================================"<< std::endl;
    std::cout << "Iteration: => Starts "<< std::endl;
    std::cout << "======================================================================"<< std::endl;
    for(i=0; i< num_of_params; i++)
    {
        std::cout << "[" << i+1 << "] Parameter : [(" << pParam_name_list[i] << ")]" <<  std::endl;
        free(pParam_name_list[i]);
    }
    free(pParam_name_list);
    std::cout << "======================================================================"<< std::endl;
    std::cout << "Iteration: => Ends "<< std::endl;
    std::cout << "======================================================================"<< std::endl;
    DataModelParam dmParam = {0};
}


GTEST_API_ int main(int argc, char *argv[]){
    char testresults_fullfilepath[GTEST_REPORT_FILEPATH_SIZE];
    char buffer[GTEST_REPORT_FILEPATH_SIZE];

    memset( testresults_fullfilepath, 0, GTEST_REPORT_FILEPATH_SIZE );
    memset( buffer, 0, GTEST_REPORT_FILEPATH_SIZE );

    snprintf( testresults_fullfilepath, GTEST_REPORT_FILEPATH_SIZE, "json:%s%s" , GTEST_DEFAULT_RESULT_FILEPATH , GTEST_DEFAULT_RESULT_FILENAME);
    ::testing::GTEST_FLAG(output) = testresults_fullfilepath;
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
