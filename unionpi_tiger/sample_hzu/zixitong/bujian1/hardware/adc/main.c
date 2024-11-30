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

int main(int argc, char **argv)
{
    int value = 0;
    double temperature = 0;

    if (get_adc_data(ADC_1, &value) >= 0) {
        temperature = ((double)value) * TEMP_CONST;
        printf("adc:[%d]\n", value);
        printf("temperature:[%.1f]C\n", temperature);
    }

    return 0;
}
