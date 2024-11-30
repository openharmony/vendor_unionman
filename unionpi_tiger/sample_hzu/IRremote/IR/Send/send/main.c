#include "Gree.h"

#define INIT_DUTY 500000
#define MAX_DUTY 2500000

void user_help(void) {
    printf("Please input like this: pwm_test <int> <int>\n");
    printf("the first number is to write the PWM_PIN to the PWM range[1,2] \n");
    printf("the second number is to write the duty_cycle to the PWM range[-2500000,2500000] \n");
}

int main(int argc, char **argv) {
    int isEnable = PWM_IS_ENABLED;  // PWM启用标志
    uint16_t nbits = kGreeBits;
    
    // 初始化空调状态结构体
    struct INFO info = {0};
    info.Mode = 1;
    info.Power = 1;
    
    uint64_t data = transform(&info);

    // 初始化PWM
    init_pmw(pwmChannel);

    // 设置PWM周期（根据舵机的程序推算，pwm使用的时钟频率为130MHZ，设置红外载波频率为38kHz)
    set_pwm_period(pwmChannel, 3421L);
    set_pwm_dutyCycle(pwmChannel, 50);

    // 发送格力红外信号
    sendGree(data, nbits, 1);

    // 启用PWM信号输出
    set_pwm_enable(pwmChannel, isEnable);

    return 0;
}
