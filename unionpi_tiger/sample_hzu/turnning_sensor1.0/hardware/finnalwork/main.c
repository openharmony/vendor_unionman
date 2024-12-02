#include <string.h>
#include <stdlib.h>// 包含标准库函数的头文件，如atoi
#include <stdio.h>
#include <unistd.h>// 包含POSIX操作系统API的头文件


#include "um_gpio.h"

int main(int argc, char **argv)//旋转一小格编码器，数值加一，按下旋转按钮则清零
{
    int gpioNum1 = UM_GPIO_07;// 默认操作的GPIO编号为UM_GPIO_07
    int gpioNum2 = UM_GPIO_06;// 默认操作的GPIO编号为UM_GPIO_06
    int gpioNum3 = UM_GPIO_05;// 3应该连接SW即按键
    int bExport = UM_GPIO_EXPORTED; // 默认操作为导出GPIO
    int direction = UM_GPIO_DIRECTION_IN; // 默认设置GPIO方向为输入
    int getValue1 = -1;// 用于存储从GPIO读取的值
    int getValue2 = -1;
    int getValue3 = -1;// 3应该连接SW即按键
    int current1_Value = -1;//当前电平值
    int current2_Value = -1;
    int previous1_Value = -1;//上一秒电平值
    int previous2_Value = -1;
    int turn_click = 0;//转动次数

    // 判断是否有输入参数，如有，则赋值指定gpio口
    if (argv[1] != NULL) {
        getValue1 = atoi(argv[1]);// 将命令行参数转换为整数
        if (getValue1 >= UM_GPIO_01 && getValue1 <= UM_GPIO_16) {
            gpioNum1 = getValue1;// 如果参数在有效范围内，更新gpioNum
        } else {
            printf("please input the gpioNum1 between 380 and 395.\n");// 如果参数不在有效范围内，打印错误信息
            return UM_GPIO_ERR;
        }
    }


    // 判断gpio口是否已经导出，如未导出则执行对应函数
    UM_GPIO_IsExport(gpioNum1, &getValue1);// 检查GPIO是否已导出
    if (getValue1 == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpioNum1, bExport);// 如果GPIO未导出，则导出GPIO
    }
    UM_GPIO_IsExport(gpioNum2, &getValue2);
    if (getValue2 == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpioNum2, bExport);
    }
    UM_GPIO_IsExport(gpioNum3, &getValue3);
    if (getValue3 == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpioNum3, bExport);
    }

    // 设置gpio口为输入或输出模式
    UM_GPIO_SetDirection(gpioNum1, direction); // 设置GPIO方向
    UM_GPIO_SetDirection(gpioNum2, direction);
    UM_GPIO_SetDirection(gpioNum3, direction);

    // 获取对应gpio口的模式并打印
    UM_GPIO_GetDirection(gpioNum1, &getValue1);// 获取GPIO方向
    printf("gpioNum1:[%d], direction:[%d]\n", gpioNum1, getValue1);// 打印GPIO编号和方向
    UM_GPIO_GetDirection(gpioNum2, &getValue2);
    printf("gpioNum2:[%d], direction:[%d]\n", gpioNum2, getValue2);
    UM_GPIO_GetDirection(gpioNum3, &getValue3);// 3应该连接SW即按键
    printf("gpioNum3:[%d], direction:[%d]\n", gpioNum3, getValue3);



    while(1){

        // 获取对应gpio口的电平值并打印
        UM_GPIO_GetValue(gpioNum1, &getValue1); // 获取GPIO值
        current1_Value = getValue1;
        printf("gpioNum1:[%d], value1:[%d]\n", gpioNum1, getValue1);// 打印GPIO编号和值

        UM_GPIO_GetValue(gpioNum2, &getValue2); 
        current2_Value = getValue2;
        printf("gpioNum2:[%d], value2:[%d]\n", gpioNum2, getValue2);
        
        UM_GPIO_GetValue(gpioNum3, &getValue3); //3应该连接SW即按键
        printf("gpioNum3:[%d], value3:[%d]\n", gpioNum3, getValue3);

        if((previous1_Value != -1) && (previous2_Value != -1)){// 初始化上一秒的值（假设为 -1 以指示未设置）  
            if((current1_Value != previous1_Value) && (current2_Value != previous2_Value)){//当旋转一小格时增加旋转次数
                    turn_click++;
                    printf("当前转动了%d小格",turn_click);
            }
        }

        if(getValue3 == 0){//可选择是否退出程序
            turn_click = 0;
            printf("计数已清零",turn_click);
            // exit(0);
        }
        //更新上一秒的值
        previous1_Value = current1_Value;
        previous2_Value = current2_Value;

        usleep(500000);//延时1s

    }

    return 0;
}
//如果1和2相同且和刚才不一样，那么则加一

// ./build.sh --product-name unionpi_tiger --build-target finnalwork_test --ccache
//hdc shell mount -o remount,rw /
//hdc file send D:\turn_test_exit1 /bin/
