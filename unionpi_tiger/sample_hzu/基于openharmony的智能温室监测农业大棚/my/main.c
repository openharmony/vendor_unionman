#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include "um_adc.h"
#include "um_gpio.h"

#define TEMP_CONST -0.008
#define LIGTH_CONST -0.1
#define UM_GPIO_HIGH_LEVEL 1
#define UM_GPIO_LOW_LEVEL  0


int delayTime = 2000000; // 延迟时间，单位微秒

int main(int argc, char **argv)
{
    int value = 0;
    int value1 = 0;
    double temperature = 0;
    double ligth = 0;

    // 导出GPIO1和GPIO2
    UM_GPIO_Export(UM_GPIO_01, UM_GPIO_EXPORTED);
    UM_GPIO_Export(UM_GPIO_02, UM_GPIO_EXPORTED);

    // 设置GPIO1和GPIO2为输出方向
    UM_GPIO_SetDirection(UM_GPIO_01, UM_GPIO_DIRECTION_OUT);
    UM_GPIO_SetDirection(UM_GPIO_02, UM_GPIO_DIRECTION_OUT);

    while(1) { // 无限循环
        // 读取温度值
        if (get_adc_data(ADC_1, &value) >= 0) {
            temperature = ((double)value) * TEMP_CONST + 60.0; // 假设30°C时的ADC值为基准点
            printf("adc:[%d]\n", value);
            printf("temperature:[%.1f]C\n", temperature);
            
            // 根据温度设置GPIO1的状态
            UM_GPIO_SetValue(UM_GPIO_01, (temperature > 30.0) ? UM_GPIO_HIGH_LEVEL : UM_GPIO_LOW_LEVEL);
        } else {
            printf("Failed to get temperature ADC data\n");
        }

        // 读取光照值
        if (get_adc_data(ADC_2, &value1) >= 0) {
            ligth = ((double)value1) * LIGTH_CONST + 409.0; // 假设30°C时的ADC值为基准点
            printf("adc2:[%d]\n", value1);
            printf("ligth:[%.1f]\n", ligth);
            
            // 根据光照设置GPIO2的状态
            UM_GPIO_SetValue(UM_GPIO_02, (ligth < 10.0) ? UM_GPIO_HIGH_LEVEL : UM_GPIO_LOW_LEVEL);
        } else {
            printf("Failed to get ligth ADC data\n");
        }

        // 延迟一段时间
        usleep(delayTime);
    }

    // 注：在实际应用中，你可能需要在程序退出前取消导出GPIO
    // UM_GPIO_Export(UM_GPIO_01, UM_GPIO_NOT_EXPORT);
    // UM_GPIO_Export(UM_GPIO_02, UM_GPIO_NOT_EXPORT);

    return 0;
}
