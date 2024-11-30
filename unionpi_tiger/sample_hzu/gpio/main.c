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
#include "um_adc.h"
#define Centre_X 3800
#define Centre_Y 3800
#define Left_X 4095
#define Left_Y 4095
#define Right_X 3800
#define Right_Y 0
#define Up_X 0
#define Up_Y 3800
#define Down_X 0
#define Down_Y 685

// #include "um_gpio.h"
void service(int x,int y);
int func(int x,int y,int std_x,int std_y,int wc);
int main(int argc, char **argv)
{
    // int gpioNum = UM_GPIO_07;
    // int bExport = UM_GPIO_EXPORTED;
    // int direction = UM_GPIO_DIRECTION_OUT;
    // int value = UM_GPIO_HIGH_LEVE;
    // int getValue = -1;

    // // 判断是否有输入参数，如有，则赋值指定gpio口
    // if (argv[1] != NULL) {
    //     getValue = atoi(argv[1]);
    //     if (getValue >= UM_GPIO_01 && getValue <= UM_GPIO_16) {
    //         gpioNum = getValue;
    //     } else {
    //         printf("please input the gpioNum between 380 and 395.\n");
    //         return UM_GPIO_ERR;
    //     }
    // }

    // // 判断gpio口是否已经导出，如未导出则执行对应函数
    // UM_GPIO_IsExport(gpioNum, &getValue);
    // if (getValue == UM_GPIO_NOT_EXPORT) {
    //     UM_GPIO_Export(gpioNum, bExport);
    // }

    // // 设置gpio口为输入或输出模式
    // UM_GPIO_SetDirection(gpioNum, direction);
    // // 设置gpio口为高低电平
    // UM_GPIO_SetValue(gpioNum, value);

    // // 获取对应gpio口的模式并打印
    // UM_GPIO_GetDirection(gpioNum, &getValue);
    // printf("gpioNum:[%d], direction:[%d]\n", gpioNum, getValue);

    // // 获取对应gpio口的电平值并打印
    // UM_GPIO_GetValue(gpioNum, &getValue);
    // printf("gpioNum:[%d], value:[%d]\n", gpioNum, getValue);
    printf("hello world!");
    int value = 0;
    int valX,valY = 0;
    while(1){
    if (get_adc_data(ADC_1, &value) >= 0) {
        valX = (value);
        //printf("adc:[%d]", value);
        printf("X:%d", valX);
    }
     if (get_adc_data(ADC_2, &value) >= 0) {
        valY = (value);
        //printf("adc:[%d]\n", value);
        printf(",Y:%d\n", valY);
    }
    service(valX,valY);
    sleep(1);
    }

    return 0;
}
void service(int x,int y){
    const int Wc = 50;//误差
    if(func(x,y,Centre_X,Centre_Y,Wc))printf("NO EVENTS\n");
    else if(func(x,y,Up_X,Up_Y,Wc))printf("UP\n");
    else if(func(x,y,Down_X,Down_Y,Wc))printf("DOWN\n");
    else if(func(x,y,Left_X,Left_Y,Wc))printf("LEFT\n");
    else if(func(x,y,Right_X,Right_Y,Wc))printf("RIGHT\n");
}
int func(int x,int y,int std_x,int std_y,int wc){
    if((x <= std_x + wc&&x >= std_x - wc)&&(y <= std_y +wc&&y >= std_y - wc))return 1;
    else return 0;
}
