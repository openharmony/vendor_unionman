#include <cstring>
#include <string>
#include <memory>
#include <vector>
#include <node_api.h>
#include <any>
#include "tool_utility.h"
#include "adcnapi.h"

#define NUMBER_JS_2_C(napi_v, type, dest)        \
    if (typeid(type) == typeid(int32_t)) {       \
        dest = pxt->SwapJs2CInt32(napi_v);       \
    }                                            \
    else if (typeid(type) == typeid(uint32_t)) { \
        dest = pxt->SwapJs2CUint32(napi_v);      \
    }                                            \
    else if (typeid(type) == typeid(int64_t)) {  \
        dest = pxt->SwapJs2CInt64(napi_v);       \
    }                                            \
    else if (typeid(type) == typeid(double_t)) { \
        dest = pxt->SwapJs2CDouble(napi_v);      \
    }

#define NUMBER_JS_2_C_ENUM(napi_v, type, dest, enum_type)      \
    if (typeid(type) == typeid(int32_t))  {    \
        dest = static_cast<enum_type>(pxt->SwapJs2CInt32(napi_v));     \
    }                                           \
    else if (typeid(type) == typeid(uint32_t)) { \
        dest = static_cast<enum_type>(pxt->SwapJs2CUint32(napi_v));    \
    }                                          \
    else if (typeid(type) == typeid(int64_t)) { \
        dest = static_cast<enum_type>(pxt->SwapJs2CInt64(napi_v));     \
    }                                           \
    else if (typeid(type) == typeid(double_t)) { \
        dest = static_cast<enum_type>(pxt->SwapJs2CDouble(napi_v));    \
    }
    
#define BOOLEAN_JS_2_C(napi_v, type, dest) {    \
    dest = pxt->SwapC2JsBool(napi_v);          \
}

#define C_DELETE(p)  \
    if ((p)) {         \
        delete (p);    \
    }

__attribute__((unused)) static napi_value number_c_to_js(XNapiTool *pxt, const std::type_info &n, DataPtr num)
{
    if (n == typeid(int32_t))
        return pxt->SwapC2JsInt32(*(int32_t *)num);
    else if (n == typeid(uint32_t))
        return pxt->SwapC2JsUint32(*(uint32_t *)num);
    else if (n == typeid(int64_t))
        return pxt->SwapC2JsInt64(*(int64_t *)num);
    else if (n == typeid(double_t))
        return pxt->SwapC2JsDouble(*(double_t *)num);
    return nullptr;
}
#define NUMBER_C_2_JS(pxt, n) \
    number_c_to_js(pxt, typeid((n)), reinterpret_cast<DataPtr>(&(n)))

namespace adcnapi {
struct get_adc_value_value_struct {
    NUMBER_TYPE_1 in0;
    NUMBER_TYPE_2 out;
};

napi_value get_adc_value_middle(napi_env env, napi_callback_info info)
{
    XNapiTool *pxt = std::make_unique<XNapiTool>(env, info).release();
    if (pxt->IsFailed()) {
        napi_value err = pxt->GetError();
        delete pxt;
        return err;
    }
    
    struct get_adc_value_value_struct *vio = new get_adc_value_value_struct();
        NUMBER_JS_2_C(pxt->GetArgv(XNapiTool::ZERO), NUMBER_TYPE_1, vio->in0);
    get_adc_value(vio->in0, vio->out);
    napi_value result = nullptr;
    result = NUMBER_C_2_JS(pxt, vio->out);
    delete vio;
    if (pxt->IsFailed()) {
        result = pxt->GetError();
    }
    delete pxt; // release
    return result;
}}
static napi_value init(napi_env env, napi_value exports)
{
    std::shared_ptr<XNapiTool> pxt = std::make_shared<XNapiTool>(env, exports);
        pxt->DefineFunction("get_adc_value", adcnapi::get_adc_value_middle);

    return exports;
}

static napi_module g_adcnapi_Module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = init,
    .nm_modname = "myadcnapi",
    .nm_priv = ((void *)0),
    .reserved = {(void *)0},
};

extern "C" __attribute__((constructor)) void Register_adcnapi_Module(void)
{
    napi_module_register(&g_adcnapi_Module);
}
