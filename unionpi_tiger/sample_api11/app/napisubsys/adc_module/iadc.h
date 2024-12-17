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

#ifndef __IADC_H__
#define __IADC_H__

#define ADC_1 1
#define ADC_2 2
#define ADC_CHANNEL_1 "/sys/bus/iio/devices/iio:device0/in_voltage2_raw"
#define ADC_CHANNEL_2 "/sys/bus/iio/devices/iio:device0/in_voltage3_raw"

#define ADC_VALUE_ERROR (-1)
#define ADC_CHANNEL_ERROR (-2)
#define ADC_PATH_ERROR (-3)

// 在c++环境下使用c语言函数接口
#ifdef __cplusplus // 如果宏_cplusplus已定义，说明当前代码在c++环境中编译。后面的代码块用extern "c"包裹
extern "C" {
#endif

int get_adc_data(int adc_channel, int* value);
int writeTemperaturetoFile(const char* filename, float temperature);

#ifdef __cplusplus
}
#endif

#endif // endif __IADC_H__
