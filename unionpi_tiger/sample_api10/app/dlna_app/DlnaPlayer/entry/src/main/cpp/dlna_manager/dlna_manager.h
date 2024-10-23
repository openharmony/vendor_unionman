//
// Created on 2024/8/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef TT2_DLNA_MANAGER_H
#define TT2_DLNA_MANAGER_H

#include "common_napi.h"
#include <bits/alltypes.h>
#include <js_native_api_types.h>
#include <memory>
#include <mutex>
#include <node_api_types.h>
#include <queue>

class DlnaManager {
public:
    
    DlnaManager();
    ~DlnaManager();
    static std::shared_ptr<DlnaManager>& GetInstance(napi_env env);
    static napi_value StartDmr(napi_env env, napi_callback_info info);
    static napi_value NotifyPositionChanged(napi_env env, napi_callback_info info);
    static napi_value Init(napi_env env, napi_value exports);
    static napi_value getDmrData(napi_env, napi_callback_info info);
    static void notify(int actionType, std::string data);
private:
    static void *ThreadStartDmr(void *);
    static std::shared_ptr<DlnaManager> dlnaManager_;
    static std::mutex instanceMutex_;
    static pthread_t dlnaThreadfd_;
    static std::queue<std::string> dataQueue_;
    static DlnaAsyncContext *dlnaAsyncContext_;
};

#endif //TT2_DLNA_MANAGER_H
