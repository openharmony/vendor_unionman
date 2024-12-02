/*
 * Copyright (c) 2022 Unionman Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "hilog/log.h"
#include "um_pwm.h"

#define INIT_DUTY 500000
#define MAX_DUTY 2500000

void user_help(void)
{
    printf("Please input like this: pwm_test <int> <int>\n");
    printf("the first number is to write the PWM_PIN to the PWM range[1,2] \n");
    printf("the second number is to write the duty_cycle to the PWM range[-2500000,2500000] \n");
}

int get_final_duty(int pwm_duty, int final_duty)
{
    int real_duty = 0;
    // 判断最终转到的角度是否超过溢出值，输出实际转到的角度
    if (final_duty < INIT_DUTY) {
        real_duty = INIT_DUTY;
        printf("Beyond the angle of rotation");
    } else if (final_duty > MAX_DUTY) {
        real_duty = MAX_DUTY;
        printf("Beyond the angle of rotation");
    } else {
        real_duty = final_duty;
    }
    return real_duty;
}

int main(int argc, char **argv)
{
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

    init_pwm(pwmChannel);
    set_pwm_period(pwmChannel, 2600000L);
    // 0.5ms对应MG996R舵机0度
    cur_duty = get_pwm_dutyCycle(pwmChannel);
    if (cur_duty < INIT_DUTY) {
        set_pwm_dutyCycle(pwmChannel, INIT_DUTY);
        cur_duty = get_pwm_dutyCycle(pwmChannel);
    }

    HILOG_INFO(LOG_CORE, "Init Sucess!\n");
    printf("The current period %d, duty_cycle:%d.\n", get_pwm_period(pwmChannel), get_pwm_dutyCycle(pwmChannel));
    // 根据占空比计算当前角度
    cur_angle = 1.0 * (cur_duty - INIT_DUTY) / INIT_DUTY * 45L;
    printf("The current angle %.2f, about to turn:%.2f.\n", cur_angle, 1.0 * pwm_duty / INIT_DUTY * 45L);

    final_duty = pwm_duty + cur_duty;
    final_duty = get_final_duty(pwm_duty, final_duty);

    set_pwm_dutyCycle(pwmChannel, final_duty);
    cur_duty = get_pwm_dutyCycle(pwmChannel);
    // 计算旋转后的角度
    printf("The angle after rotation is %.2f.\n", 1.0 * (cur_duty - INIT_DUTY) / INIT_DUTY * 45L);
    set_pwm_enable(pwmChannel, isEnable);

    return 0;
}
