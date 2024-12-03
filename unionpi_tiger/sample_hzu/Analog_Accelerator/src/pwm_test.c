
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "hilog/log.h"
#include "um_pwm.h"

int main(int argc, char **argv)
{
    int pwmChannel = PWM1;
    int isEnable = PWM_IS_ENABLED;
    int polarity = PWM_POLARITY_NORMAL;//设置极性
    int pwm_duty = atoi(argv[1]);//占空比50 = 50%
    int period=3000000;
    int minperiod=500000;
    int maxperiod = 3000000;
    int step = 10000;
    int direction = -1;
    init_pwm(pwmChannel);
    set_pwm_polarity(pwmChannel, polarity);
    set_pwm_period(pwmChannel, period);
    set_pwm_dutyCycle(pwmChannel, period * pwm_duty / 100);
    set_pwm_enable(pwmChannel, isEnable);

    while(1)
    {
        set_pwm_period(pwmChannel, period);
        set_pwm_dutyCycle(pwmChannel, period * pwm_duty / 100);
        printf("The current period %d, duty_cycle:%d.\n", get_pwm_period(pwmChannel), get_pwm_dutyCycle(pwmChannel));
        usleep(10000);

        period += step * direction;

        if(period >= maxperiod)
        {
            period = maxperiod;
            direction = -1;
        }
        else if(period <= minperiod)
        {
            period = minperiod;
            direction = 1;
        }
    }

    return 0;
}
