/*
本例程是学生大作业设置的家用智能防火警报系统南向开发，使用传感器火焰传感器，磁簧传感器，蜂鸣器，三种
使用gpio两个引脚接受两个传感器的数字量信息，并判断是否出现火灾或者燃气泄漏，同时进行蜂鸣器报警
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "um_gpio.h"

int main()
{    
    //12口接收来自火焰传感器的DO数字量口       有火为1 ，无火为0
    int gpiofire = UM_GPIO_12;//395
    //16口输出的蜂鸣器口                      1为不触发模式。         
    int gpiofmq = UM_GPIO_16;//387
    //1口接收来自磁簧传感器的DO数字量口        有感应是0，无感应是1
    int gpiocihuan = UM_GPIO_01;

    int bExport = UM_GPIO_EXPORTED;//0
    int direction = UM_GPIO_DIRECTION_IN;//0
    int ledout=UM_GPIO_DIRECTION_OUT;     //1
    int getfire = -1;
    int getcihuan = -1;
    int fmq=-1;
    int getfmq=-1;  


    // 判断gpio口是否已经导出，如未导出则执行对应函数
    UM_GPIO_IsExport(gpiofire, &getfire);
    if (getfire == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpiofire, bExport);
    }
    UM_GPIO_IsExport(gpiocihuan,&getcihuan);
    if (getcihuan == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpiocihuan, bExport);
    }
    UM_GPIO_IsExport(gpiofmq,&getfmq);
    if (getfmq == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpiofmq, bExport);
    }


    //设置15号口为输入模式
    UM_GPIO_SetDirection(gpiocihuan, direction);
    //设置12号口为输入模式
    UM_GPIO_SetDirection(gpiofire, direction);
    //设置16号口为输出模式
    UM_GPIO_SetDirection(gpiofmq, ledout);

    //先将蜂鸣器设为高电平不触发模式。
    UM_GPIO_SetValue(gpiofmq, UM_GPIO_HIGH_LEVE); 
    


    while(1){
    fmq=1;
    //获取两个感应器gpio的输出数据 有火为1 
    UM_GPIO_GetValue(gpiofire, &getfire);
    UM_GPIO_GetValue(gpiocihuan, &getcihuan);
    
    //异或 11 00都为正常，10、01的情况下 蜂鸣器报警
    if(getfire^getcihuan){
    // 有火1有磁0  正常蜂鸣器为1
    // 有火1无磁1  着火蜂鸣器为0
    // 无火0有磁0  燃气泄漏蜂鸣器为0
    // 无火0无磁1  正常蜂鸣器为1

    //将蜂鸣器设为高电平不触发模式。 
    UM_GPIO_SetValue(gpiofmq, UM_GPIO_HIGH_LEVE);
    //用于打印检测
    fmq=1;

    }
    else{
    //将蜂鸣器设为低电平为触发方式
    UM_GPIO_SetValue(gpiofmq, UM_GPIO_LOW_LEVE);

    fmq=0;
  
    }

    UM_GPIO_GetValue(gpiofmq, &getfmq);

    //打印各个gpio口状态
    if(fmq==0){
        printf("Flame detection: [%d], Cihuan detection: [%d], fmq status:[%d]\n", getfire, getcihuan,getfmq);
        sleep(1);
    }  
    }



 
    return 0;
}
