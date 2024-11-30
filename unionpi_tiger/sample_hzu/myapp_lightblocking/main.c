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

int main(int argc, char **argv)
{
    int gpioNum1 = UM_GPIO_07;
    int bExport = UM_GPIO_EXPORTED;
    int direction = UM_GPIO_DIRECTION_IN;
    int value = UM_GPIO_HIGH_LEVE;
    int getValue = -1;

    // 判断是否有输入参数，如有，则赋值指定gpio口
    if (argv[1] != NULL) {
        getValue = atoi(argv[1]);
        if (getValue >= UM_GPIO_01 && getValue <= UM_GPIO_16) {
            gpioNum = getValue;
        } else {
            printf("please input the gpioNum between 380 and 395.\n");
            return UM_GPIO_ERR;
        }
    }

    // 判断gpio口是否已经导出，如未导出则执行对应函数
    UM_GPIO_IsExport(gpioNum, &getValue);
    if (getValue == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpioNum, bExport);
    }

    // 设置gpio口为输入或输出模式
    UM_GPIO_SetDirection(gpioNum, direction);

    // 获取对应gpio口的模式并打印
    UM_GPIO_GetDirection(gpioNum, &getValue);
    printf("gpioNum:[%d], direction:[%d]\n", gpioNum, getValue);

    // 获取对应gpio口的电平值并打印
    UM_GPIO_GetValue(gpioNum, &getValue);
    printf("gpioNum:[%d], value:[%d]\n", gpioNum, getValue);

    return 0;
}
