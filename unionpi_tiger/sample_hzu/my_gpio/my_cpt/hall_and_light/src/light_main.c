#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "um_gpio.h"

#define HAVE_ITEM 1                         //光折断器被遮挡输出高电平
#define WITHOUT_ITEN 0                      //光折断器连通输出低电平

int if_have_item = 0;                       //是否有物品，1表示有，0表示无
int gpioNum = UM_GPIO_07;                   //指定GPIO07引脚
int bExport = UM_GPIO_EXPORTED;             //GPIO是否已导出
int direction = UM_GPIO_DIRECTION_IN;       //GPIO引脚方向
int value = UM_GPIO_LOW_LEVE;               //存储当前GPIO07引脚的电平，初始化为低电平
int lastvalue = UM_GPIO_LOW_LEVE;           //存储上一次GPIO07引脚的电平，初始化为低电平
int getValue = -1;                          //getValue用来接收任何值


int light_main(int argc, char **argv)
{
    // 判断gpio口是否已经导出，如未导出则执行对应函数
    UM_GPIO_IsExport(gpioNum, &getValue);
    if (getValue == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpioNum, bExport);
    }

    // 设置gpio口为输入或输出模式
    // 这里设置为输入
    UM_GPIO_SetDirection(gpioNum, direction);
    
    //检测GPIO07的电平是否由0变为1，即光折断器中间是否有物体
    
        UM_GPIO_GetValue(gpioNum, &getValue);
        value = getValue;

        //如果GPIO07的电平由0变为1
        if(value == HAVE_ITEM && lastvalue != value)
        {
            lastvalue = value;
            if_have_item = 1;   //有物品
        }

        //如果GPIO07的电平由1变为0
        if(value == WITHOUT_ITEN && lastvalue != value)
        {
           lastvalue = value;
           if_have_item = 0;
        }
    return 0;
}

