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

#ifndef __UM_ADC_H__
#define __UM_ADC_H__

// errno
#define ADC_PARAM_ERR (-1)
#define ADC_ERR (-2)

#define ADC_CHANNEL_1 "/sys/bus/iio/devices/iio:device0/in_voltage2_input"
#define ADC_CHANNEL_2 "/sys/bus/iio/devices/iio:device0/in_voltage3_input"
#define ADC_CHANNEL_HW "/sys/bus/iio/devices/iio:device0/in_voltage1_input"
#define ADC_CHANNEL_HP "/sys/bus/iio/devices/iio:device0/in_voltage0_input"

// channel
#define ADC_1 2
#define ADC_2 3
#define ADC_HW 1
#define ADC_HP 0

int get_adc_data(int adc_channel, int *value);

#endif // endif __UM_ADC_H__
