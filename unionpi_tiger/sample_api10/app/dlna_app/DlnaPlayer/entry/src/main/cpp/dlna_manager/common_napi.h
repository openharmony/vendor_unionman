//
// Created on 2024/8/14.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef DlnaPlayer_common_napi_H
#define DlnaPlayer_common_napi_H

#include <hilog/log.h>
#include <js_native_api.h>
#include <js_native_api_types.h>
#include <node_api_types.h>
#include <string>

class CommonNapi {
public:
    CommonNapi() = delete;
    ~CommonNapi() = delete;
    static napi_ref CreateReference(napi_env env, napi_value arg);
    static napi_deferred CreatePromise(napi_env env, napi_ref ref, napi_value &result);
    static std::string GetStringArgument(napi_env env, napi_value value);
};

struct DlnaAsyncContext {
public:
    explicit DlnaAsyncContext(napi_env env);
    virtual ~DlnaAsyncContext();
    static void CompleteCallback(napi_env env, napi_status status, void *data);
    static void Callback(napi_env env, const DlnaAsyncContext *context, const napi_value *args);
    napi_env env_ = nullptr;
    napi_ref recvCallbackRef = nullptr;
    napi_value resource;
    napi_async_work work;
    napi_deferred deferred = nullptr;
    std::string data;
    bool ctorFlag = false;
    
    void UvWork() {
        napi_handle_scope scope = nullptr;
        napi_open_handle_scope(env_, &scope);
        if (scope == nullptr) {
            OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, "Dlna_dmr_common", "napi_open_handle_scope failed");
            return ;
        }
        
        napi_value jsCallback = nullptr;
        napi_status status = napi_get_reference_value(env_, recvCallbackRef, &jsCallback);
        if (status != napi_ok) {
            OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, "Dlna_dmr_common", "UvWork napi_get_reference_value failed");
            napi_close_handle_scope(env_, scope);
            return ;
        }
        
        napi_value result = nullptr;
        napi_value args[1];
        napi_create_string_utf8(env_, data.c_str(), NAPI_AUTO_LENGTH, &args[0]);
        status = napi_call_function(env_, nullptr, jsCallback, 1, &args[0], &result);
        if (status != napi_ok) {
            OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, "Dlna_dmr_common", "UvWork napi_call_function failed");
            napi_close_handle_scope(env_, scope);
            return ;
        }
    }
};

struct AutoRef {
    AutoRef(napi_env env, napi_ref cb)
        : env_(env), cb_(cb)
    {
    }
    
    ~AutoRef()
    {
        if (env_ != nullptr && cb_ != nullptr) {
            (void) napi_delete_reference(env_, cb_);
        }
    }
    
    napi_env env_;
    napi_ref cb_;
};

#endif //DlnaPlayer_common_napi_H
