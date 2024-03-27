#include "my_hello_napi.h"

namespace my_hello_napi {
bool get_Hello_Generator(std::string& out)
{
    out = " HELLO NAPI lzy! \r\n";
    return true;
}
}
