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

#ifndef __UM_PWM_H__
#define __UM_PWM_H__

// errno
#define PWM_ERR (-1)
#define PWM_WRONOG_CHANNEL (-2)
#define PWM_FILE_NOT_EXIST (-3)
#define PWM_PARAM_OUT_OF_RANGE (-4)
#define PWM_DUTYCYCLE_PARAM_OUT_OF_RANGE (-5)

// pwm vaild range
#define PWM_MAX_PERIOD 340000000
#define PWM_MIN_PERIOD 400000

#define PWM_NOT_ENABLED 0
#define PWM_IS_ENABLED 1

// pwm polarity
#define PWM_POLARITY_NORMAL 0
#define PWM_POLARITY_INVERSED 1

#define PWM1 1
#define PWM2 2

#define PWM1_PEX "/sys/class/SteeringGearB"
#define PWM2_PEX "/sys/class/SteeringGear"

int set_pwm_enable(int pwmChannel, int isEnable);
int set_pwm_period(int pwmChannel, long period);
int get_pwm_dutyCycle(int pwmChannel, long *value);
int get_pwm_period(int pwmChannel, long *value);
int set_pwm_dutyCycle(int pwmChannel, long dutyCycle);
int is_pwm_enabled(int pwmChannel, int *value);
int set_pwm_polarity(int pwmChannel, int polarity);
int get_pwm_polarity(int pwmChannel, int *value);

#endif // endif __UM_PWM_H__
