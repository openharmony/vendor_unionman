#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "../include/um_adc.h"
#include "hilog/log.h"
#include "../include/um_pwm.h"

#define INIT_DUTY 50
#define MAX_DUTY 2500000
#define BEEP_FREQUENCY1 1000 // 第一音符频率，单位 Hz
#define BEEP_FREQUENCY2 800  // 第二音符频率，单位 Hz
#define TEMPERATURE_THRESHOLD 2200 // 温度阈值（假设的 ADC 值）

int delayTime = 500000; // 延迟时间，单位微秒（控制音符间的间隔）

void user_help(void)
{
    printf("Please input like this: pwm_test <int> <int>\n");
    printf("the first number is to write the PWM_PIN to the PWM range[1,2] \n");
    printf("the second number is to write the duty_cycle to the PWM range[-2500000,2500000] \n");
}

// 获取有效的 duty cycle
int get_final_duty(int pwm_duty, int final_duty) {
    int real_duty = 0;
    // Ensure duty cycle is within the valid range
    if (final_duty < INIT_DUTY) {
        real_duty = INIT_DUTY;
        printf("Duty cycle too low, setting to minimum.\n");
    } else if (final_duty > MAX_DUTY) {
        real_duty = MAX_DUTY;
        printf("Duty cycle too high, setting to maximum.\n");
    } else {
        real_duty = final_duty;
    }
    return real_duty;
}

void play_two_tigers(int pwmChannel) {
    // 播放“两只老虎”旋律：按顺序播放两个音符
    printf("Playing Two Tigers melody...\n");
    
    // 播放第一个音符
    set_pwm_period(pwmChannel, BEEP_FREQUENCY1); // 设置音符频率
    set_pwm_dutyCycle(pwmChannel, MAX_DUTY);      // 启动蜂鸣器
    usleep(500000); // 持续500ms，控制音符时长

    // 播放第二个音符
    set_pwm_period(pwmChannel, BEEP_FREQUENCY2); // 设置第二个音符频率
    set_pwm_dutyCycle(pwmChannel, MAX_DUTY);      // 启动蜂鸣器
    usleep(500000); // 持续500ms，控制音符时长

    // 结束后关闭蜂鸣器
    set_pwm_dutyCycle(pwmChannel, 0);
}

int main(int argc, char **argv) {
    int temp_value = 0;
    int pwmChannel = PWM1;
    int isEnable = PWM_IS_ENABLED;
    int final_duty = 0;
    double cur_angle = 0;
    int cur_duty = 0;
    int pwm_pin = atoi(argv[1]);
    int pwm_duty = atoi(argv[2]);

    if (argc > 3L || argc < 2L) {
        user_help();
        HILOG_DEBUG(LOG_CORE, "The number of parameters is incorrect!\n");
        return PWM_ERR;
    }

    if (pwm_pin < 1L || pwm_pin > 2L || pwm_duty > MAX_DUTY || pwm_duty < -MAX_DUTY) {
        user_help();
        HILOG_DEBUG(LOG_CORE, "The wrong parameter!\n");
        return PWM_ERR;
    } else {
        pwmChannel = pwm_pin;
    }

    init_pmw(pwmChannel);
    set_pwm_period(pwmChannel, 2600000L); // 设置 PWM 周期
    cur_duty = get_pwm_dutyCycle(pwmChannel);
    if (cur_duty < INIT_DUTY) {
        set_pwm_dutyCycle(pwmChannel, INIT_DUTY);
        cur_duty = get_pwm_dutyCycle(pwmChannel);
    }

    HILOG_INFO(LOG_CORE, "Init Sucess!\n");
    printf("The current period %d, duty_cycle:%d.\n", get_pwm_period(pwmChannel), get_pwm_dutyCycle(pwmChannel));

    cur_angle = 1.0 * (cur_duty - INIT_DUTY) / INIT_DUTY * 45L;
    printf("The current angle %.2f, about to turn:%.2f.\n", cur_angle, 1.0 * pwm_duty / INIT_DUTY * 45L);

    final_duty = pwm_duty + cur_duty;
    final_duty = get_final_duty(pwm_duty, final_duty);
    set_pwm_dutyCycle(pwmChannel, final_duty);
    cur_duty = get_pwm_dutyCycle(pwmChannel);

    printf("The angle after rotation is %.2f.\n", 1.0 * (cur_duty - INIT_DUTY) / INIT_DUTY * 45L);
    set_pwm_enable(pwmChannel, isEnable);

    while (1) {
        // 读取KY-013温度传感器数据
        if (get_adc_data(ADC_2, &temp_value) == ADC_OK) {
            printf("Temperature ADC Value: %d\n", temp_value);
        } else {
            printf("Temperature Error\n");
        }

        // 判断温度是否超过阈值
        if (temp_value > TEMPERATURE_THRESHOLD) {
            // 温度超过阈值，启动蜂鸣器并播放旋律
            printf("Temperature exceeds threshold, activating buzzer and playing melody.\n");
            play_two_tigers(pwmChannel);
            //analogWrite(buzzer_pin, 1);  // 设置 PWM 输出为 0，关闭蜂鸣器
            //digitalWrite(buzzer_pin, HIGH);   // 关闭蜂鸣器
        } else {
            // 温度低于阈值，关闭蜂鸣器
            printf("Temperature below threshold, turning off buzzer.\n");
            //digitalWrite(buzzer_pin, LOW);   // 关闭蜂鸣器
            //analogWrite(buzzer_pin, 0);  // 设置 PWM 输出为 0，关闭蜂鸣器
            set_pwm_dutyCycle(pwmChannel, 0);  // 关闭蜂鸣器
        }

        usleep(delayTime);  // 延迟，控制检查间隔
    }

    return 0;
}
