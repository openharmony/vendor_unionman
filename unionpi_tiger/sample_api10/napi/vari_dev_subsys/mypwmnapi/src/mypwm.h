#ifndef IMPL_MYPWM_H
#define IMPL_MYPWM_H

#include <string>
#include <memory>
#include <vector>
#include <cmath>
#include <map>
#include <any>

using NUMBER_TYPE_1 = int;
using NUMBER_TYPE_2 = int;


namespace mypwm {
bool get_Hello_Generator(std::string& out);
bool updateAndGetsliderValue(NUMBER_TYPE_1& value, NUMBER_TYPE_2& out);
}
#endif // IMPL_MYPWM_H
