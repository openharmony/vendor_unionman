#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "securec.h"
#include "um_pwm.h"
#include "ky006.h"

void ky006_Init(int pwmChannel,int final_duty)
{
    init_pmw(pwmChannel);
    set_pwm_period(pwmChannel, 2600000L);
    set_pwm_dutyCycle(pwmChannel, final_duty);
}

void ky006_ring(int pwmChannel)
{
    set_pwm_enable(pwmChannel, PWM_IS_ENABLED);
    usleep(50L * 1000L);
    set_pwm_enable(pwmChannel, PWM_NOT_ENABLED);
}