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

void delay(int x)
{
    int i, j;
    for(i = 0; i < 1000; i++)
    {
        j = x;
        while(j--);
    }
}

int main(int argc, char **argv)
{
    int getValue = -1, temp = 0;

    UM_GPIO_IsExport(UM_GPIO_01, &getValue);	//设置GPIO可用 
    if (getValue == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(UM_GPIO_01, UM_GPIO_EXPORTED);
    }
    UM_GPIO_IsExport(UM_GPIO_02, &getValue);	//设置GPIO可用 
    if (getValue == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(UM_GPIO_02, UM_GPIO_EXPORTED);
    }
    
    
    UM_GPIO_SetDirection(UM_GPIO_01, UM_GPIO_DIRECTION_OUT);	//设置方向 
    UM_GPIO_SetDirection(UM_GPIO_02, UM_GPIO_DIRECTION_IN);	//设置方向 

    UM_GPIO_GetDirection(UM_GPIO_02, &getValue);	//得到GPIO方向 
    printf("gpioNum:[%d], direction:[%d]\n", UM_GPIO_02, getValue);

    while(1)
    {
    	UM_GPIO_GetDirection(UM_GPIO_02, &temp);
    	if(temp == UM_GPIO_LOW_LEVE)
        	UM_GPIO_SetValue(UM_GPIO_01, UM_GPIO_LOW_LEVE); //亮
        else if(temp == UM_GPIO_HIGH_LEVE)
        	UM_GPIO_SetValue(UM_GPIO_01, UM_GPIO_HIGH_LEVE);    //灭
        delay(1000);
    }

    return 0;
}







// #include <string.h>
// #include <stdlib.h>
// #include <stdio.h>
// #include <unistd.h>

// #include "um_gpio.h"

// int main(int argc, char **argv)
// {
//     int gpioNum = UM_GPIO_07;
//     int bExport = UM_GPIO_EXPORTED;
//     int direction = UM_GPIO_DIRECTION_OUT;
//     int value = UM_GPIO_HIGH_LEVE;
//     int getValue = -1;

//     // 判断是否有输入参数，如有，则赋值指定gpio口
//     if (argv[1] != NULL) {
//         getValue = atoi(argv[1]);
//         if (getValue >= UM_GPIO_01 && getValue <= UM_GPIO_16) {
//             gpioNum = getValue;
//         } else {
//             printf("please input the gpioNum between 380 and 395.\n");
//             return UM_GPIO_ERR;
//         }
//     }

//     // 判断gpio口是否已经导出，如未导出则执行对应函数，导出的意思是是否已经被系统识别了，被识别了才可以对其操作
//     UM_GPIO_IsExport(gpioNum, &getValue);
//     if (getValue == UM_GPIO_NOT_EXPORT) {
//         UM_GPIO_Export(gpioNum, bExport);
//     }

//     // 设置gpio口为输入或输出模式
//     UM_GPIO_SetDirection(gpioNum, direction);
//     // 设置gpio口为高低电平
//     UM_GPIO_SetValue(gpioNum, value);

//     // 获取对应gpio口的模式并打印
//     UM_GPIO_GetDirection(gpioNum, &getValue);
//     printf("gpioNum:[%d], direction:[%d]\n", gpioNum, getValue);

//     // 获取对应gpio口的电平值并打印
//     UM_GPIO_GetValue(gpioNum, &getValue);
//     printf("gpioNum:[%d], value:[%d]\n", gpioNum, getValue);

//     return 0;
// }