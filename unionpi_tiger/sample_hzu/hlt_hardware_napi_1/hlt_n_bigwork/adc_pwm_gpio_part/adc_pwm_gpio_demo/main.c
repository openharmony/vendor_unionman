#include <stdio.h>
#include <securec.h>
#include <unistd.h>
#include "um_gpio.h"
#include "um_adc.h"
#include "um_pwm.h"

#define Z_GPIO_PIN UM_GPIO_02 // 假设Z按钮连接到GPIO2
#define GREEN_GPIO UM_GPIO_01 // 定义绿色灯的GPIO引脚

// 设置颜色值函数
void setRGB(int red, int blue, int green) {
    // 设置红色的PWM
    if (set_pwm_dutyCycle(PWM2, red) != 0) {
        printf("设置红色PWM失败！\n");
    }
    // 设置蓝色的PWM
    if (set_pwm_dutyCycle(PWM1, blue) != 0) {
        printf("设置蓝色PWM失败！\n");
    }
    // 设置绿色灯的GPIO状态
    if (UM_GPIO_SetValue(GREEN_GPIO, green) != 0) {
        printf("设置绿色GPIO状态失败！\n");
    }
}

// 获取摇杆X、Y、Z轴的值
void get_joystick_values(int *x, int *y, int *z) {
    // 获取X轴的值
    if (get_adc_data(ADC_1, x) != ADC_OK) {
        printf("获取X轴数据失败！\n");
    }

    // 获取Y轴的值
    if (get_adc_data(ADC_2, y) != ADC_OK) {
        printf("获取Y轴数据失败！\n");
    }

    // 获取Z轴的值（通过GPIO读取）
    if (UM_GPIO_GetValue(Z_GPIO_PIN, z) != 0) {
        printf("获取Z轴数据失败！\n");
    }
}

int main() {
    int x, y, z;

    // 初始化GPIO
    if (UM_GPIO_Export(Z_GPIO_PIN, 1) != 0) {
        printf("GPIO导出失败！\n");
        return -1;
    }
    if (UM_GPIO_SetDirection(Z_GPIO_PIN, UM_GPIO_DIRECTION_IN) != 0) {
        printf("设置GPIO方向失败！\n");
        return -1;
    }

    // 初始化PWM
    if (init_pmw(PWM1) != 0 || init_pmw(PWM2) != 0) {
        printf("PWM初始化失败！\n");
        return -1;
    }
    // 设置PWM周期
    if (set_pwm_period(PWM1, 1000) != 0 || set_pwm_period(PWM2, 1000) != 0) {
        printf("设置PWM周期失败！\n");
        return -1;
    }
    // 设置PWM为正常极性
    if (set_pwm_polarity(PWM1, PWM_POLARITY_NORMAL) != 0 || 
        set_pwm_polarity(PWM2, PWM_POLARITY_NORMAL) != 0) {
        printf("设置PWM极性失败！\n");
        return -1;
    }
    // 使能PWM
    if (set_pwm_enable(PWM1, PWM_IS_ENABLED) != 0 || 
        set_pwm_enable(PWM2, PWM_IS_ENABLED) != 0) {
        printf("使能PWM失败！\n");
        return -1;
    }

    // 初始化绿色灯GPIO
    if (UM_GPIO_Export(GREEN_GPIO, 1) != 0) {
        printf("GPIO导出失败！\n");
        return -1;
    }
    if (UM_GPIO_SetDirection(GREEN_GPIO, UM_GPIO_DIRECTION_OUT) != 0) {
        printf("设置GPIO方向失败！\n");
        return -1;
    }

    printf("开始控制三色灯！\n");

    while (1) {
        // 获取摇杆的X、Y、Z轴值
        get_joystick_values(&x, &y, &z);

        // 判断Z键状态，如果按下（z == 0），绿色灯亮；如果未按下（z == 1），绿色灯熄灭
        int greenState = (z == 0) ? UM_GPIO_HIGH_LEVE : UM_GPIO_LOW_LEVE;

        // 如果Z键未按下，保持混色效果，调整红蓝灯的亮度
        if (z == 1) {
            int red = (x * 255) / 1023; // 假设X轴最大值为1023
            int blue = (y * 255) / 1023; // 假设Y轴最大值为1023
            // 将红蓝的亮度设置为混色，绿色灯熄灭
            setRGB(red, blue, UM_GPIO_LOW_LEVE);
        } else {
            // 如果Z键按下，点亮绿色灯，关闭红蓝灯
            setRGB(0, 0, greenState); // 关闭红蓝，点亮绿灯
        }

        // 输出当前的RGB状态
        printf("X轴的值：%d, Y轴的值：%d, Z轴的按钮状态：%s\n", 
               x, y, z == 0 ? "按下" : "松开");

        // 延时100ms
        usleep(100000); // 延时100ms
    }

    // 停止时关闭所有灯
    setRGB(0, 0, UM_GPIO_LOW_LEVE);

    // 清理资源
    set_pwm_enable(PWM1, PWM_NOT_ENABLED);
    set_pwm_enable(PWM2, PWM_NOT_ENABLED);
    UM_GPIO_Export(GREEN_GPIO, 0);
    UM_GPIO_Export(Z_GPIO_PIN, 0);

    printf("三色灯控制结束！\n");
    return 0;
}
