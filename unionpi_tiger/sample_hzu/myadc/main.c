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
#include <stdio.h>
#include "um_adc.h"

#define ADC_MAX_VALUE 4095
#define ADC_REF_VOLTAGE 3.3
//#define TEMP_CONST 100.0

int main(int argc, char **argv)
{
    int adc_value = 0;
    double voltage = 0.0;
    double temperature = 0.0;

    if (get_adc_data(ADC_1, &adc_value) >= 0) {

        voltage = (double)adc_value * ADC_REF_VOLTAGE / ADC_MAX_VALUE;

        temperature = voltage * TEMP_CONST;
        printf("ADC Value: [%d]\n", adc_value);
        printf("Voltage: [%.3f]V\n", voltage);
        printf("Temperature: [%.1f]C\n", temperature);
        } else {
            printf("Failed to get ADC data.\n");
    }

    return 0;
}

