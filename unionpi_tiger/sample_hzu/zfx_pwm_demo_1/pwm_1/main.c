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

#define pwm_g_pin 1
#define pwm_b_pin 2
#define color_F 10000

void user_help(void)
{
    printf("user_help\n");
}

int main(int argc, char **argv)
{
    int isEnable = PWM_IS_ENABLED;
    // int pwm_pin = atoi(argv[1]);
    // int pwm_duty = atoi(argv[2]);

    // if (argc > 3L || argc < 2L) {
    //     user_help();
    //     HILOG_DEBUG(LOG_CORE, "The number of parameters is incorrect!\n");
    //     return PWM_ERR;
    // }

    // if (pwm_pin < 1L || pwm_pin > 2L || pwm_duty > MAX_DUTY || pwm_duty < -MAX_DUTY) {
    //     user_help();
    //     HILOG_DEBUG(LOG_CORE, "The wrong parameter!\n");
    //     return PWM_ERR;
    // } else {
    //     pwmChannel = pwm_pin;
    // }

    init_pmw(pwm_g_pin);
    set_pwm_period(pwm_g_pin, 2600000L);
    set_pwm_dutyCycle(pwm_g_pin, 0);
    set_pwm_enable(pwm_g_pin, isEnable);

    init_pmw(pwm_b_pin);
    set_pwm_period(pwm_b_pin, 2600000L);
    set_pwm_dutyCycle(pwm_b_pin, 0);
    set_pwm_enable(pwm_b_pin, isEnable);

    while (1)
    {
        int color_i = 0, color_j = 0;
        for (color_i = 0; color_i <= 5; color_i++)
        {
            set_pwm_dutyCycle(pwm_g_pin, color_i * color_F * 50);
            for (color_j = 0; color_j <= 10; color_j++)
            {

                set_pwm_dutyCycle(pwm_b_pin, color_j * color_F * 25);
                usleep(500 * 1000);
            }
            printf("color_i = %d\n\n", color_i);
        }

        printf("first end\n");

        for (color_i = 0; color_i <= 5; color_i++)
        {
            set_pwm_dutyCycle(pwm_b_pin, color_i * color_F * 50);
            for (color_j = 0; color_j <= 10; color_j++)
            {

                set_pwm_dutyCycle(pwm_g_pin, color_j * color_F * 25);
                sleep(1);
            }
            printf("color_i = %d\n\n", color_i);
        }

        printf("second end\n");
    }
    return 0;
}
