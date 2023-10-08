/* Copyright 2023 Unionman Technology Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "adc_if.h"
#include "hdf_log.h"
#include "osal_time.h"

/* 设备号0，通道号1 */
#define ADC_DEVICE_NUM  0
#define TEMP_CONST (1.8/(0.01*4096))

int32_t main(int argc, char *argv[])
{
    int32_t ret;
    uint32_t channelNum = 0;
    DevHandle adcHandle = NULL;
    uint32_t read_val = 0;
    double temperature = 0;
    // 默认打开通道0，可以输入参数使用其他通道
    if (argc == 2L) {
        channelNum=atoi(argv[1]);
    }
    /* 打开ADC设备 */
    adcHandle = AdcOpen(ADC_DEVICE_NUM);
    if (adcHandle == NULL) {
        HDF_LOGE("%s: Open ADC%u fail!", __func__, ADC_DEVICE_NUM);
        return -1;
    }
    /* 读取ADC数据 */
    ret = AdcRead(adcHandle, channelNum, &read_val);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: ADC read fail!:%d", __func__, ret);
        AdcClose(adcHandle);
        return -1;
    }
    /* 计算温度 */
    temperature = ((double)read_val) * TEMP_CONST;

    printf("Temperature is %.1f°C\n", temperature);

    /* 访问完毕关闭ADC设备 */
    AdcClose(adcHandle);
    return HDF_SUCCESS;
}
