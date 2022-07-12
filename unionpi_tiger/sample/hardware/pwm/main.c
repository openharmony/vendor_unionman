
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

    if (NULL != argv[1]) {
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
