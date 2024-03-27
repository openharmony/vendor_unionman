#include "mypwm.h"
#include "um_pwm.h"
namespace mypwm {
bool get_Hello_Generator(std::string& out)
{
    out="my pwm lzy";
    return true;
}

bool updateAndGetsliderValue(NUMBER_TYPE_1& value, NUMBER_TYPE_2& out)
{
    int pwmPex =1;
    static const int MAX_DUTY = 2500000;
    int pwmAngle=value;
    int cur_duty = 0;
    double cur_angle = 0;
    static const int INIT_DUTY = 500000;
    int pwmChannel=1;

    set_pwm_period(pwmPex, MAX_DUTY);
    double pwmDuty = 1.0 * INIT_DUTY * pwmAngle / 45L + INIT_DUTY;
    set_pwm_dutyCycle(pwmPex, static_cast<int>(pwmDuty));

    cur_duty = get_pwm_dutyCycle(pwmChannel);
    set_pwm_enable(pwmPex, 1);
    cur_angle = 1.0 * (cur_duty - INIT_DUTY) / INIT_DUTY * 45L;

    out=static_cast<int>(value);
    return true;
}
}
