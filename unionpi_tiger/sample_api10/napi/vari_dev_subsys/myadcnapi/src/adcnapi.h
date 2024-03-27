#ifndef IMPL_ADCNAPI_H
#define IMPL_ADCNAPI_H

#include <string>
#include <memory>
#include <vector>
#include <cmath>
#include <map>
#include <any>

using NUMBER_TYPE_1 = uint32_t;
using NUMBER_TYPE_2 = uint32_t;


namespace adcnapi {
bool get_adc_value(NUMBER_TYPE_1& channel, NUMBER_TYPE_2& out);
}
#endif // IMPL_ADCNAPI_H
