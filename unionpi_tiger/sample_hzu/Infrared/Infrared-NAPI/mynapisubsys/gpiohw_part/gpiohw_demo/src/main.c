#include <stdio.h>
#include <unistd.h>
#include "../include/um_gpio.h"
#include "../include/infrared_receiver.h"
#include "../include/infrared_transmitter.h"

#define INFRARED_TRANSMIT_GPIO UM_GPIO_07
#define INFRARED_RECEIVE_GPIO UM_GPIO_08

int main()
{
    // 初始化红外发射模块
    Infrared_Transmit_Setup(INFRARED_TRANSMIT_GPIO);
    // 初始化红外接收模块
    Infrared_Receive_Setup(INFRARED_RECEIVE_GPIO);

    while(1){
        // 打开红外发射
        Infrared_Transmit_Control(INFRARED_TRANSMIT_GPIO, UM_GPIO_HIGH_LEVE);
        //读取红外发射模块状态

        // 读取红外接收模块状态
        int receiveValue = Infrared_Receive_Read(INFRARED_RECEIVE_GPIO);
        if (receiveValue == UM_GPIO_LOW_LEVE)//如果接收到为低电平，则表示红外信号被检测到
        {
            printf("Infrared signal detected!\n");
            sleep(1);//延时1秒
        }

    }

    // 关闭红外发射
    Infrared_Transmit_Control(INFRARED_TRANSMIT_GPIO, UM_GPIO_LOW_LEVE);
    printf("Infrared Transmitter OFF.\n");

    return 0;
}
