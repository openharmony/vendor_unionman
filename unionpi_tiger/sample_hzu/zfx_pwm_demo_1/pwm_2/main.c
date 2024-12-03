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

    init_pmw(pwm_g_pin);
    set_pwm_period(pwm_g_pin, 2600000L);
    set_pwm_dutyCycle(pwm_g_pin, 0);
    set_pwm_enable(pwm_g_pin, isEnable);

    init_pmw(pwm_b_pin);
    set_pwm_period(pwm_b_pin, 2600000L);
    set_pwm_dutyCycle(pwm_b_pin, 0);
    set_pwm_enable(pwm_b_pin, isEnable);
    printf("Please enter the hexadecimal code for the color:\n");

    while(1){
    char str[10] = {""};
    printf("Please enter the hexadecimal code for the color:\n");
    scanf("%s", str);
    
    char R[3]= {""};
    R[1]= str[1];
    R[0]= str[0];
    
    char G[3]= {""};
    G[1]= str[3];
    G[0]= str[2];
    
    char B[3]= {""};
    B[1]= str[5];
    B[0]= str[4];
    
    int R_d = strtol(R, NULL, 16);
    int G_d = strtol(G, NULL, 16);
    int B_d = strtol(B, NULL, 16);

    printf("R = %d    G = %d    B = %d\n\n", R_d, G_d, B_d);
    set_pwm_dutyCycle(pwm_g_pin, (MAX_DUTY / 255) *  G_d);
    set_pwm_dutyCycle(pwm_b_pin, (MAX_DUTY / 255) *  B_d);

    }
   
    return 0;
}
