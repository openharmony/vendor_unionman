#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "um_gpio.h"

#define HAVE_ITEM 1                         //光折断器被遮挡输出高电平
#define WITHOUT_ITEN 0                      //光折断器连通输出低电平

int if_have_item = 0;                       //是否有物品，1表示有，0表示无

int main(int argc, char **argv)
{
    int gpioNum = UM_GPIO_07;                   //指定GPIO07引脚
    int bExport = UM_GPIO_EXPORTED;             //GPIO是否已导出
    int direction = UM_GPIO_DIRECTION_IN;       //GPIO引脚方向
    int value = UM_GPIO_LOW_LEVE;               //存储当前GPIO07引脚的电平，初始化为低电平
    int lastvalue = UM_GPIO_LOW_LEVE;           //存储上一次GPIO07引脚的电平，初始化为低电平
    int getValue = -1;                          //getValue用来接收任何值
    
    // 判断gpio口是否已经导出，如未导出则执行对应函数
    UM_GPIO_IsExport(gpioNum, &getValue);
    if (getValue == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpioNum, bExport);
    }

    // 因为GPIO口用于接受光折断器输出的电平， 所以这里设置方向为输入
    UM_GPIO_SetDirection(gpioNum, direction);
    
    //循环检测GPIO07的电平是否由0变为1，即光折断器中间是否有物体
    while(1)
    {
        //将GPIO07的电平读取出来，存到value中
        UM_GPIO_GetValue(gpioNum, &getValue);
        value = getValue;

        //如果GPIO07的电平由0变为1，即有物品通过光折断器
        if(value == HAVE_ITEM && lastvalue != value)
        {
            lastvalue = value;  //更新lastvalue为value
            if_have_item = 1;   //if_have_item置1，表示有物品
        }

        //如果GPIO07的电平由1变为0，即没有物品通过光折断器
        if(value == WITHOUT_ITEN && lastvalue != value)
        {
           lastvalue = value;   //更新lastvalue为value
        }
    }
    return 0;
}

