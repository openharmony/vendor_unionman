//
// Created on 2024/8/9.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef DlnaPlayer_dlna_dmr_manager_napi_H
#define DlnaPlayer_dlna_dmr_manager_napi_H

#include "dlna_manager.h"
#include <js_native_api_types.h>
#include <memory>

static const char DLNA_DMR_MANAGER_NAPI_CLASS_NAME[] = "DlnaDmrManager";

class DlnaDmrManagerNapi {
public:
    DlnaDmrManagerNapi();
    ~DlnaDmrManagerNapi();
    static napi_value Init(napi_env env, napi_value exports);
    static napi_value CreateDlnaDmrManager(napi_env env);
    static napi_value OnDataRecieve(napi_env env, napi_callback_info info);
    
private:
    static void DlnaDmrManagerNapiDestructor(napi_env env, void *nativeObject, void *finalize_hint);
    static napi_value DlnaDmrManagerNapiConstructor(napi_env env, napi_callback_info info);
    static napi_value JsCreateDmr(napi_env env, napi_callback_info info);
    static thread_local napi_ref sConstructor_;
    std::shared_ptr<DlnaManager> dlnaManager_;

    napi_env env_;
    napi_ref wrapper_;
};

#endif //DlnaPlayer_dlna_dmr_manager_napi_H
