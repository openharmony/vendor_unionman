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

#include <stdio.h>
#include "../include/um_adc.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "hilog/log.h"
#include "../include/um_pwm.h"

#define INIT_PERIOD 3500000
#define MAX_PERIOD 3500000
#define MIN_PERIOD 500000
#define step 10000//周期步长设置，连续pwm的周期间隔
#define percent 50//占空比设置为50%

void change_period(int pwmChannel, double vol, int *current_period)//根据电压vol值，改变当前pwm的周期，保持占空比不变，即改变音调大小
{
    int direction = -1;//方向设置，指周期增长还是下降
    if(vol>=1.3&&vol<=2.9)//合法区间
        {
            printf("enable is:%d\n",is_pwm_enabled(pwmChannel));
            *current_period = get_pwm_period(pwmChannel);
            printf("$$$$The current period %d, duty_cycle:%d.\n", get_pwm_period(pwmChannel), get_pwm_dutyCycle(pwmChannel));
            if(vol >=1.3 && vol <1.5)//停止油门
            {
                direction = 1;
                while(1)
                {
                    printf("The current period %d, duty_cycle:%d.\n", get_pwm_period(pwmChannel), get_pwm_dutyCycle(pwmChannel));
                    set_pwm_period(pwmChannel, *current_period);
                    set_pwm_dutyCycle(pwmChannel, *current_period * percent / 100);
                    usleep(1000);
                    *current_period += step * direction;
                    if(*current_period >= MAX_PERIOD)
                    {
                        *current_period = MAX_PERIOD;//停止
                        set_pwm_period(pwmChannel, *current_period);
                        set_pwm_dutyCycle(pwmChannel, *current_period * percent / 100);
                        set_pwm_enable(pwmChannel, PWM_NOT_ENABLED);//关闭通道
                        break;
                    }
                }
            }
            else
            {
                int ret = is_pwm_enabled(pwmChannel);
                if(ret == 0) set_pwm_enable(pwmChannel, PWM_IS_ENABLED);//使能有效，开始工作
                int initt = (MAX_PERIOD - MIN_PERIOD) / 15;//1.5-2.9共15个挡，每个档位的差值200000
                int x = vol * 10;
                int aim = MAX_PERIOD - initt * (x - 15 + 1);//目标值
                printf("aim:%d\n",aim);
                if(*current_period > aim) direction = -1;
                else direction = 1;

                if(direction == -1)//升调
                {
                        while(1)
                        {
                            printf("The current period %d, duty_cycle:%d.\n", get_pwm_period(pwmChannel), get_pwm_dutyCycle(pwmChannel));
                            set_pwm_period(pwmChannel, *current_period);
                            set_pwm_dutyCycle(pwmChannel, *current_period * percent / 100);
                            usleep(1000);
                            *current_period += step * direction;
                            if(*current_period <= aim )
                            {
                                *current_period = aim;
                                set_pwm_period(pwmChannel, *current_period);
                                set_pwm_dutyCycle(pwmChannel, *current_period * percent / 100);
                                break;
                            }
                        }
                }
                else
                {
                    while(1)
                    {
                        printf("The current period %d, duty_cycle:%d.\n", get_pwm_period(pwmChannel), get_pwm_dutyCycle(pwmChannel));
                        set_pwm_period(pwmChannel, *current_period);
                        set_pwm_dutyCycle(pwmChannel, *current_period * percent / 100);
                        usleep(1000);
                        *current_period += step * direction;
                        if(*current_period >= aim )
                        {
                            *current_period = aim;
                            set_pwm_period(pwmChannel, *current_period);
                            set_pwm_dutyCycle(pwmChannel, *current_period * percent / 100);
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            printf("Excepted voltage!\n");
        }
}

int main(int argc, char **argv)
{
    int pwmChannel = PWM1; //选用通道一
    int polarity = PWM_POLARITY_NORMAL;//设置极性
    double vol = 0;//霍尔元件输出电压
    int value = 0;//adc采样数据
    int current_period = INIT_PERIOD;
    init_pwm(pwmChannel);//初始化pmw通道一，后面要用到
    set_pwm_polarity(pwmChannel, polarity);//设置正常极性
    set_pwm_period(pwmChannel, current_period); //设置周期初始值，以纳秒为单位
    set_pwm_dutyCycle(pwmChannel, 0);//初始化为不发声状态
    set_pwm_enable(pwmChannel, PWM_IS_ENABLED);//使能有效，开始工作
    while(1)
    {
    if (get_adc_data(ADC_1, &value) >= 0){
        //获取霍尔原件的模拟电压
        vol = (double)value * 0.00080586;
        printf("Liner Hall Magnetic Sensor\n");
        printf("adc:[%d]\n", value);
        printf("Voltage is:[%.2f]V\n\n",vol);
        //根据电压调整蜂鸣器
        change_period(pwmChannel, vol, &current_period);
    }
    usleep(20000);//设置20ms采样间隔
    }
    return 0;
}