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

#include "um_pwm.h"

int main(int argc, char **argv)
{
    int pwmChannel = PWM1; // PWM2
    int isEnable = PWM_IS_ENABLED;
    long lValue = 0;
    int s32Value = 0;

    if (argv[1] != NULL) {
        pwmChannel = atoi(argv[1]);
    }

    set_pwm_enable(pwmChannel, isEnable);
    get_pwm_dutyCycle(pwmChannel, &lValue);
    printf("dutyCycle:[%ld]\n", lValue);
    get_pwm_period(pwmChannel, &lValue);
    printf("period:[%ld]\n", lValue);

    is_pwm_enabled(pwmChannel, &s32Value);
    printf("enabled:[%d]\n", s32Value);
    get_pwm_polarity(pwmChannel, &s32Value);
    printf("polarity:[%d]\n", s32Value);

    return 0;
}
