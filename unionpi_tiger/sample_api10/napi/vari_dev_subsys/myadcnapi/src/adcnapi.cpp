#include "adcnapi.h"
extern "C" {
    int get_adc_data(int channel, int *value);
}
namespace adcnapi {
bool get_adc_value(NUMBER_TYPE_1& channel, NUMBER_TYPE_2& out)
{
    get_adc_data(channel,&out);
    return true;
}
}
