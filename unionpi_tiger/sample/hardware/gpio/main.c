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
    int s32GpioNum = UM_GPIO_09;
    int bExport = UM_GPIO_EXPORTED;
    int direction = UM_GPIO_DIRECTION_OUT;
    int s32Value = UM_GPIO_HIGH_LEVE;
    int s32GetValue = -1;

    /* 本例子只是模拟一个gpio口（GPIO_09）
    来做设置out模式，且将其设置为高电平
    有需要的话可以自己设置程序来控制gpio */
    if (argv[1] != NULL) {
        s32GpioNum = atoi(argv[1]);
    }

    UM_GPIO_IsExport(s32GpioNum, &s32GetValue);
    if (s32GetValue == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(s32GpioNum, bExport);
    }

    UM_GPIO_SetDirection(s32GpioNum, direction);
    UM_GPIO_SetValue(s32GpioNum, s32Value);

    UM_GPIO_GetDirection(s32GpioNum, &s32GetValue);
    printf("s32GpioNum:[%d], direction:[%d]\n", s32GpioNum, s32GetValue);

    UM_GPIO_GetValue(s32GpioNum, &s32GetValue);
    printf("s32GpioNum:[%d], value:[%d]\n", s32GpioNum, s32GetValue);

    return 0;
}
