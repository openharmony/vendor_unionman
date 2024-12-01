#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "ky006.h"
#include "hilog/log.h"
#include "um_pwm.h"
#define INIT_DUTY 500000
#define MAX_DUTY 2500000

int main(int argc, char **argv)
{
    int pwm_duty = atoi(argv[1]);
    if(pwm_duty < INIT_DUTY)
    {
        pwm_duty=INIT_DUTY;
    }
    if(pwm_duty > MAX_DUTY)
    {
        pwm_duty=MAX_DUTY;
    }
    ky006_Init(PWM1,pwm_duty);
    ky006_ring(PWM1);

    return 0;
}