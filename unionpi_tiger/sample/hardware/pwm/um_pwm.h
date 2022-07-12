#ifndef __UM_PWM_H__
#define __UM_PWM_H__

// errno
#define PWM_ERR -1
#define PWM_WRONOG_CHANNEL -2
#define PWM_FILE_NOT_EXIST -3
#define PWM_PARAM_OUT_OF_RANGE -4
#define PWM_DUTYCYCLE_PARAM_OUT_OF_RANGE -5

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
