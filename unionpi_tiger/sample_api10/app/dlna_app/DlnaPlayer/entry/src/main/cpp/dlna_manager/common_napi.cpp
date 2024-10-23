//
// Created on 2024/8/14.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "common_napi.h"
#include <hilog/log.h>
#include <js_native_api.h>
#include <node_api.h>

#define TAG "Dlna_dmr_commonnapi"

DlnaAsyncContext::DlnaAsyncContext(napi_env env) : env_(env)
{
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG, "DlnaAsyncContext Create");
}

DlnaAsyncContext::~DlnaAsyncContext()
{
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG, "DlnaAsyncContext Destroy");
}

void DlnaAsyncContext::CompleteCallback(napi_env env, napi_status status, void *data) {
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG, "CompleteCallback In");
    
    auto asyncContext = reinterpret_cast<DlnaAsyncContext *>(data);
    if(asyncContext == nullptr) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG, "asyncContext is null");
        return ;
    }
    
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    
    napi_value args[2] = { nullptr };
    napi_get_undefined(env, &args[0]);
    napi_get_undefined(env, &args[1]);
    
    Callback(env, asyncContext, args);
    napi_delete_async_work(env, asyncContext->work);
    
    delete asyncContext;    
}

void DlnaAsyncContext::Callback(napi_env env, const DlnaAsyncContext *context, const napi_value *args) {
    if (context->deferred) {
        napi_resolve_deferred(env, context->deferred, args[1]);
    } else if (context->recvCallbackRef != nullptr) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG, "callback napi_call_function");
        napi_value callback = nullptr;
        napi_get_reference_value(env, context->recvCallbackRef, &callback);
        if(callback == nullptr) {
            OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG, "callback is null");
            return ;
        }
        constexpr size_t argCount = 2;
        napi_value retVal;
        napi_get_undefined(env, &retVal);
        napi_call_function(env, nullptr, callback, argCount, args, &retVal);
        napi_delete_reference(env, context->recvCallbackRef);
    } else {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG, "invalid promise and callback");
    }
}

napi_ref CommonNapi::CreateReference(napi_env env, napi_value arg) {
    napi_ref ref = nullptr;
    napi_valuetype valueType = napi_undefined;
    if (arg != nullptr && napi_typeof(env, arg, &valueType) == napi_ok && valueType == napi_function) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG,  "napi_create_reference");
        napi_create_reference(env, arg, 1, &ref);
    }
    return ref;
}

napi_deferred CommonNapi::CreatePromise(napi_env env, napi_ref ref, napi_value &result) {
    napi_deferred deferred = nullptr;
    if (ref == nullptr) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG, "napi_create_promise");
        napi_create_promise(env, &deferred, &result);
    }
    return deferred;
}

std::string CommonNapi::GetStringArgument(napi_env env, napi_value value) {
    std::string strValue = "";
    size_t bufLength = 0;
    napi_status status = napi_get_value_string_utf8(env, value, nullptr, 0, &bufLength);
    if (status == napi_ok && bufLength > 0 && bufLength < PATH_MAX) {
        char *buffer = static_cast<char *>(malloc((bufLength + 1) * sizeof(char)));
        if (buffer == nullptr) {
            OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG, "no memory");
            return strValue;
        }
        status = napi_get_value_string_utf8(env, value, buffer, bufLength + 1, &bufLength);
        if (status == napi_ok) {
            OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG, "argument = %{public}s", buffer);
            strValue = buffer;
        }
        free(buffer);
        buffer = nullptr;
    }
    return strValue;
}