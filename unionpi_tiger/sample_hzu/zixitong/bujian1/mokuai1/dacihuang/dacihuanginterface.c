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

#include "um_gpio.h"
#include "um_adc.h"

//输入gpio的编号，设置大磁簧数字信号接收引脚
//gpio1为输入数字信号引脚编号
//返回读取的电平值
int Read_dacihuang_shuzivalue(int gpio)
{
    int getValue1 = -1, temp = -1;

	if (gpio >= UM_GPIO_01 && gpio <= UM_GPIO_16) { } 
	else {
        printf("please input the gpioNum between 380 and 395.\n");
        return UM_GPIO_ERR;
    }

    UM_GPIO_IsExport(gpio, &getValue1);	//设置GPIO可用 
    if (getValue1 == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpio, UM_GPIO_EXPORTED);
    }
    
    UM_GPIO_SetDirection(gpio, UM_GPIO_DIRECTION_IN);	//设置方向
    
    UM_GPIO_GetDirection(gpio, &getValue1);	//得到GPIO方向 
    printf("gpioNum:[%d], direction:[%d]\n", gpio, getValue1);

    UM_GPIO_GetValue(gpio, &temp);	//得到GPIO方向 
    return temp;
}

//输入adc通道，设置大磁簧模拟信号接收引脚 
//adc为选择输入模拟信号引脚,1为ADC1，2为ADC2
//返回读取的模拟值
int Read_dacihuang_monivalue(char adc)
{
    int value = 0;
    double temperature = 0;

    if(adc == 1 || adc == 2) {
        if (get_adc_data(adc, &value) >= 0) {
            return value;
        }   
        else {
            printf("error:adc data error");
            return -1;
        }
    }
    else {
        printf("error:adc number error");
        return -1;
    }
}

