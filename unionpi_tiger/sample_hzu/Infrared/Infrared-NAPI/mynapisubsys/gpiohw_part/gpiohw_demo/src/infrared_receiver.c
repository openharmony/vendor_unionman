#include "../include/um_gpio.h"

void Infrared_Receive_Setup(int gpioNum)
{
    // 导出GPIO
    UM_GPIO_Export(gpioNum, UM_GPIO_EXPORTED);

    // 设置GPIO为输入模式 UM_GPIO_DIRECTION_IN为0 固定输入
    UM_GPIO_SetDirection(gpioNum, UM_GPIO_DIRECTION_IN);
}

int Infrared_Receive_Read(int gpioNum)
{
    int value = -1;
    // 读取GPIO电平值
    UM_GPIO_GetValue(gpioNum, &value);
    return value; // 返回值 0 表示低电平，1 表示高电平
}

