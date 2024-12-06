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

#define HAVE_MAGNETISM 0     //触发了霍尔传感器，OUT引脚输出低电平
#define WITHOUT_MAGNETISM 1  //没有触发，OUT引脚输出高电平

int get_if_have_item(void);

int main(int argc, char **argv)
{
    int gpioNum = UM_GPIO_08;                   //指定GPIO08引脚
    int bExport = UM_GPIO_EXPORTED;             //GPIO是否已导出
    int direction = UM_GPIO_DIRECTION_IN;       //GPIO引脚方向，设置为输入
    int value = UM_GPIO_LOW_LEVE;               //存储GPIO08引脚的电平，初始化为低电平
    int getValue = -1;                          //getValue用来接收任何值    
    int count = 0;                              //统计有磁性的物体数量
    int if_have_item = 0;
    // 判断gpio口是否已经导出，如未导出则执行对应函数
    UM_GPIO_IsExport(gpioNum, &getValue);
    if (getValue == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpioNum, bExport);
    }

    // 设置gpio口为输入或输出模式
    // 这里设置为输入
    UM_GPIO_SetDirection(gpioNum, direction);
    
    //循环检测GPIO08的电平
    while(1)
    {
        if_have_item = get_if_have_item();
        //如果有物体通过光折断器
        if(if_have_item == 1)
        {
            UM_GPIO_GetValue(gpioNum, &getValue);
            value = getValue;

            //如果GPIO08的电平为0
            if(value == HAVE_MAGNETISM)
            {
                count++;
                printf("该物体有磁性！\n");
                printf("当前磁性物体数量为：[%d]\n", count);
            }

            //如果GPIO08的电平为1
            if(value == WITHOUT_MAGNETISM)
            {
                printf("该物体没有磁性！\n");
                printf("当前磁性物体数量为：[%d]\n", count);
            }
        }
    }
    
    
    return 0;
}

