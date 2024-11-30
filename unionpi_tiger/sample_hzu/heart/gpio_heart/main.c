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
#include <unistd.h>
#include "hilog/log.h"
#include "securec.h"
#include "gpio_heart.h"


int main(int argc, char **argv)
{
    int gpioNum = UM_GPIO_02;
    int bExport = UM_GPIO_EXPORTED;
    int direction = UM_GPIO_DIRECTION_OUT;
    int getValue = -1;
    int period=20;


    // 设置gpio口为输入或输出模式,初始化引脚
    UM_GPIO_SetDirection(gpioNum, direction);

    while (1) {

        UM_GPIO_SetValue(gpioNum, UM_GPIO_HIGH_LEVE); // 点亮 LED
        printf("The LED is on\n");
        // 获取对应gpio口的电平值并打印
        UM_GPIO_GetValue(gpioNum, &getValue);
        printf("gpioNum:[%d], value:[%d]\n", gpioNum, getValue);


        usleep(period * 5000); // 延时，period 单位为毫秒

        UM_GPIO_SetValue(gpioNum, UM_GPIO_LOW_LEVE); // 熄灭 LED
        printf("The LED is off\n");
        // 获取对应gpio口的电平值并打印
        UM_GPIO_GetValue(gpioNum, &getValue);
        printf("gpioNum:[%d], value:[%d]\n", gpioNum, getValue);

        usleep(period * 10000); // 延时，period 单位为毫秒
    }

    return 0;
}