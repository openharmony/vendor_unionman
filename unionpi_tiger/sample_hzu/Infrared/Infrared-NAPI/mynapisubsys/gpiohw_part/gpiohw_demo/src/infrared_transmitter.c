#include "../include/um_gpio.h"

void Infrared_Transmit_Setup(int gpioNum)
{
    // 导出GPIO,UM_GPIO_EXPORTED为1 固定输出
    UM_GPIO_Export(gpioNum, UM_GPIO_EXPORTED);

    // 设置GPIO为 输出模式UM_GPIO_DIRECTION_OUT为1 固定输出
    UM_GPIO_SetDirection(gpioNum, UM_GPIO_DIRECTION_OUT);
}

void Infrared_Transmit_Control(int gpioNum, int value)
{
    // 控制GPIO电平，value = UM_GPIO_HIGH_LEVE 打开红外发射（高电平打开）
    // value = UM_GPIO_LOW_LEVE 关闭红外发射（低电平关闭）
    UM_GPIO_SetValue(gpioNum, value);
}