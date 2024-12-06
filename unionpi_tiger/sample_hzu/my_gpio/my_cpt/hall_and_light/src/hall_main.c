#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "um_gpio.h"

#define HAVE_MAGNETISM 0                        //触发了霍尔传感器，OUT引脚输出低电平
#define WITHOUT_MAGNETISM 1                     //没有触发，OUT引脚输出高电平

extern int if_have_item;                        //引用light_main.c中的变量
extern int light_main();                        //引用light_main.c中的函数
int count = 0;                                  //统计有磁性的物体数量


int main(int argc, char **argv)
{
    int gpioNum = UM_GPIO_08;                   //指定GPIO08引脚
    int bExport = UM_GPIO_EXPORTED;             //GPIO是否已导出
    int direction = UM_GPIO_DIRECTION_IN;       //GPIO引脚方向，设置为输入
    int value = UM_GPIO_LOW_LEVE;               //存储GPIO08引脚的电平，初始化为低电平
    int getValue = -1;                          //getValue用来接收任何值   
 
    // 判断gpio口是否已经导出，如未导出则执行对应函数
    UM_GPIO_IsExport(gpioNum, &getValue);
    if (getValue == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpioNum, bExport);
    }

    // 设置GPIO08为输入，用于接收霍尔磁力传感器输出的电平
    UM_GPIO_SetDirection(gpioNum, direction);
    
    //循环检测是否有带磁性的物体通过光折断器和霍尔磁力传感器
    while(1)
    {
        //light_main()用于检测是否有物体经过光折断器
        light_main();

        //如果有物体通过光折断器
        if(if_have_item == 1)
        {
            //接收GPIO08的电平，存到value中
            UM_GPIO_GetValue(gpioNum, &getValue);
            value = getValue;

            //如果GPIO08的电平为0，则说明该物体有磁性
            if(value == HAVE_MAGNETISM)
            {
                //计数
                count++;
                printf("该物体有磁性！\n");
                printf("当前磁性物体数量为：[%d]\n", count);
                continue;
            }

            //如果GPIO08的电平为1，则说明该物体没有磁性
            if(value == WITHOUT_MAGNETISM)
            {
                printf("该物体没有磁性！\n");                
            }
        }
    }
    return 0;
}

