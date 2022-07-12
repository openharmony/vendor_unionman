/*
* Copyright (c) 2022 Unionman Technology Co., Ltd.
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "um_adc.h"

int get_adc_data(int adc_channel, int *value)
{
    int ret = 0;

    char adc_name[128];
    (void)memset_s(adc_name, sizeof(adc_name), 0, sizeof(adc_name));

    if (ADC_1 == adc_channel) {
        (void)sprintf_s(adc_name, sizeof(adc_name), "%s", ADC_CHANNEL_1);
    } else if (ADC_2 == adc_channel) {
        (void)sprintf_s(adc_name, sizeof(adc_name), "%s", ADC_CHANNEL_2);
    } else if (ADC_HW == adc_channel) {
        (void)sprintf_s(adc_name, sizeof(adc_name), "%s", ADC_CHANNEL_HW);
    } else if (ADC_HP == adc_channel) {
        (void)sprintf_s(adc_name, sizeof(adc_name), "%s", ADC_CHANNEL_HP);
    } else {
        return ADC_PARAM_ERR;
    }

    FILE *fp;
    char buffer[20];
    fp = fopen(adc_name, "r");
    (void)fread(buffer, sizeof(buffer), 1, fp);
    (void)fclose(fp);

    *value = atoi(buffer);

    return ret;
}
