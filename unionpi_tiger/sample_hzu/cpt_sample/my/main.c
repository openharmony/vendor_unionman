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
#include <unistd.h>
#include <math.h>


#include "um_adc.h"
int delayTime = 2000000; // 延迟时间，单位微秒

// 假设这是从NTC热敏电阻读取的ADC值转换为温度的函数

double convert_adc_to_temperature(int adc_value) {
    if (adc_value == 0) {
        // 处理无效的ADC值，可以返回一个错误码或者一个默认的温度值
        printf("NO adc: %d\n", adc_value);
        return -1; // 或者其他表示错误的值
    }
    
    double Temp;
    Temp=log(((10240000.0/adc_value)-10000));
    Temp=1 / (0.001129148+(0.000234125+(0.0000000876741*Temp*Temp))*Temp);
    Temp=Temp-273.15;
    return Temp+50;
}


int main() {
    int light_value = 5;
    int temp_value = 0;
    int ligth_adc=0;
    double temperature = 0;

    while (1) {
        // 读取光敏传感器数据
        if (get_adc_data(ADC_1, &ligth_adc) == ADC_OK) {
           // light_value-=ligth_adc/1000;
            printf("Light sensor readings: %d\n", ligth_adc);//light_value
            light_value=5;
        } else {
            printf("Light Eoror\n");
        }

        // 读取KY-013温度传感器数据
        if (get_adc_data1(ADC_2, &temp_value) == ADC_OK) {
            temperature = convert_adc_to_temperature(temp_value);
            printf("Temperature ADC Value: %d, Temperature: %.1f C\n", temp_value, temperature);
        } else {
            printf("Temperature Eoror\n");
        }

        usleep(delayTime); // 延迟
    }

    return 0;
}
