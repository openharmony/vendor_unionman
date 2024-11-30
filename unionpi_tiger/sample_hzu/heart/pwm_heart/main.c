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
#include "pwm_heart.h"

#define INIT_DUTY 500000    //占空比初始,在这区间才能发声
#define MAX_DUTY 2500000    //占空比的最大最小值
#define INIT_PERIOD 2600000L // 最小周期，单位为微秒
#define MAX_PERIOD 13000000L // 最大周期，单位为微秒

int main(int argc, char **argv) {
    int pwmChannel = PWM1;  // 设置蜂鸣器的引脚
    int cur_duty = INIT_DUTY;   //当前的占空比
    int cur_period = INIT_PERIOD; //当前周期
    init_pmw(pwmChannel);   // 初始化引脚
    set_pwm_period(pwmChannel, cur_period);   // 设置周期值
    set_pwm_dutyCycle(pwmChannel, cur_duty);


    HILOG_INFO(LOG_CORE, "Init Success!\n");


    while(1){
        
            //输出周期和占空比
    printf("The current period %d, duty_cycle:%d.\n", get_pwm_period(pwmChannel), get_pwm_dutyCycle(pwmChannel));   

        // 计算最终的占空比，使声音更尖锐,设置下一个周期的占空比
    cur_duty = cur_duty + (cur_duty* 0.01); // 假设增加1%的占空比
    
    //防止占空比超最大值
    if(cur_duty>MAX_DUTY){
       cur_duty=MAX_DUTY;
    }

    //设置占空比
    set_pwm_dutyCycle(pwmChannel, cur_duty);

    // 计算新的周期值，使声音更尖锐
    cur_period = cur_period + (cur_period * 0.05); // 假设增加5%的周期

    // 防止周期值低于最小值
        if(cur_period > MAX_PERIOD){
            cur_period = MAX_PERIOD;
        }

    // 设置新的周期值
    set_pwm_period(pwmChannel, cur_period);    
 

       // 使蜂鸣器发声
    set_pwm_enable(pwmChannel, PWM_IS_ENABLED);

    // 等待一段时间，让蜂鸣器发声
    sleep(5);

    // 关闭蜂鸣器
    set_pwm_enable(pwmChannel, PWM_NOT_ENABLED);

    }


    return 0;
}
