//
// Created on 2024/8/9.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include <asm-generic/stat.h>
#include <js_native_api.h>
#include "dlna_dmr_manager_napi.h"
#include "dlna_log.h"
#include "dlna_napi_utils.h"
#include "native_common.h"

thread_local napi_ref DlnaDmrManagerNapi::sConstructor_ = nullptr;


napi_value DlnaDmrManagerNapi::DlnaDmrManagerNapiConstructor(napi_env env, napi_callback_info info) {
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "%{public}s:%{public}d In", __FUNCTION__, __LINE__);
    napi_status status;
    napi_value result = nullptr;
    napi_value thisVar = nullptr;
    
    napi_get_undefined(env, &result);
    DLNA_NAPI_GET_JS_OBJ_WITH_ZERO_ARGS(env, info, status, thisVar);
    
    if (status == napi_ok && thisVar != nullptr) {
    std::unique_ptr<DlnaDmrManagerNapi> obj = std::make_unique<DlnaDmrManagerNapi>();
        obj->env_ = env;
        obj->dlnaManager_ = DlnaManager::GetInstance();
        if (obj->dlnaManager_ == nullptr) {
            OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "%{public}s:%{public}d obj->dlnaManager_ null", __FUNCTION__, __LINE__);
            return result;
        }
        status = napi_wrap(env, thisVar, reinterpret_cast<void *>(obj.get()),
                            DlnaDmrManagerNapi::DlnaDmrManagerNapiDestructor, nullptr, nullptr);
        if (status == napi_ok) {
            obj.release();
            return thisVar;
        } else {
            OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "%{public}s:%{public}d Failure wrapping js to native napi", __FUNCTION__, __LINE__);
        }
    }
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "%{public}s:%{public}d call failed",
                 __FUNCTION__, __LINE__);
    return result;
}

napi_value DlnaDmrManagerNapi::Init(napi_env env, napi_value exports) {
//    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "IN");
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "%{public}s:%{public}d In", __FUNCTION__ , __LINE__);
    napi_status status;
    napi_value ctorObj;
    int32_t retCount = 1;
    
    napi_property_descriptor staticProperty[] = {
        DECLARE_NAPI_STATIC_PROPERTY("createDmr", JsCreateDmr);
    };

    napi_property_descriptor dlna_dmr_mgr_propertis[] = {
        DECLARE_NAPI_FUNCTION("on", JsSetOnCallback),
    };
    
    status = napi_define_class(env, DLNA_DMR_MANAGER_NAPI_CLASS_NAME, NAPI_AUTO_LENGTH,
                                DlnaDmrManagerNapiConstructor, nullptr,
                                sizeof(dlna_dmr_mgr_propertis) / sizeof(dlna_dmr_mgr_propertis[0]),
                                dlna_dmr_mgr_propertis, &ctorObj);
    if (status != napi_ok) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "Failed to define Dmr Class");
        return nullptr;
    }

    status = napi_create_reference(env, ctorObj, retCount, &sConstructor_);
    if (status != napi_ok) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exports, DLNA_DMR_MANAGER_NAPI_CLASS_NAME, ctorObj);
    if (status != napi_ok) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "Failed to set constructor");
        return nullptr;
    }

    status = napi_define_properties(env, exports, sizeof(staticProperty) / sizeof(staticProperty[0]), staticProperty);
    if (status != napi_ok) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "Failed to define static function");
        return nullptr;
    }
    return exports;
}