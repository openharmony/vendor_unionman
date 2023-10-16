/*
 * Copyright (c) 2023 Unionman Technology Co., Ltd.
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
 */

#undef LOG_TAG
#define LOG_TAG "adc_test"

#include <securec.h>
#include <stdio.h>
#include <stdlib.h>

#include "adc_if.h"
#include "hilog/log.h"
#include "um_adc.h"

int get_adc_data(int adc_channel, int* value)
{
    if (value == NULL) {
        HILOG_ERROR(LOG_CORE, "value pointer error\n");
        return ADC_VALUE_ERROR;
    }

    char adc_path[128];
    (void)memset_s(adc_path, sizeof(adc_path), 0, sizeof(adc_path));

    if (adc_channel == ADC_1) {
        (void)sprintf_s(adc_path, sizeof(adc_path), "%s", ADC_CHANNEL_1);
    } else if (adc_channel == ADC_2) {
        (void)sprintf_s(adc_path, sizeof(adc_path), "%s", ADC_CHANNEL_2);
    } else {
        HILOG_DEBUG(LOG_CORE, "no such a adc_channel\n");
        return ADC_PARAM_ERROR;
    }

    FILE* fp = NULL;
    char buffer[sizeof(int)];
    fp = fopen(adc_path, "r");
    if (!fp) {
        HILOG_ERROR(LOG_CORE, "adc_channel path error\n");
        printf("adc_channel path error\n");
        return ADC_PATH_ERROR;
    }
    (void)fread(buffer, sizeof(buffer), 1, fp);
    (void)fclose(fp);
    fp = NULL;

    *value = atoi(buffer);
    HILOG_INFO(LOG_CORE, "um_adc value = %{public}d\n", *value);

    return 0;
}

int UM_hdc_get_value(int channelNum, int* value)
{
    int32_t ret;
    DevHandle adcHandle = NULL;
    uint32_t read_val = 0;
    // 默认打开通道0，可以输入参数使用其他通道
    /* 打开ADC设备 */
    adcHandle = AdcOpen(0);
    if (adcHandle == NULL) {
        return -1;
    }
    /* 读取ADC数据 */
    ret = AdcRead(adcHandle, channelNum, &read_val);
    if (ret != HDF_SUCCESS) {
        AdcClose(adcHandle);
        return -1;
    }
    *value = read_val;
    /* 访问完毕关闭ADC设备 */
    AdcClose(adcHandle);
    return 0;
}