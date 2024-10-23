//
// Created on 2024/8/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "ThreadPool.h"
#include "napi/native_api.h"
#include "dlna_controller.h"
#include "dlna_log.h"
#include <bits/alltypes.h>
#include <uv.h>
#include "json.hpp"
#include "udp_server.h"
#include "dlnaplayer_listener.h"
#include "dlna_manager.h"


//DEFINE_DLNA_LABEL("dlna_dmr_manager");
#define TAG "Dlna_dmr_manager"

using namespace OHOS::CastEngine::CastEngineService::CastSessionDlna;
using nlohmann::json;

std::shared_ptr<DlnaManager> DlnaManager::dlnaManager_;
std::mutex DlnaManager::instanceMutex_;
std::queue<std::string> DlnaManager::dataQueue_;
pthread_t DlnaManager::dlnaThreadfd_;
DlnaAsyncContext *DlnaManager::dlnaAsyncContext_;


static std::shared_ptr<DlnaPlayer_Listener> listener_;
std::shared_ptr<IDlnaController> controler_;

void* data_cbk(int msgType, const void *data) {
    char *url = (char *)data;
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG,"%{public}s data:%{public}s", __FUNCTION__ , url);
}

DlnaManager::DlnaManager() {
    
}

DlnaManager::~DlnaManager() { 
    dlnaManager_ = nullptr; 
}

std::shared_ptr<DlnaManager>& DlnaManager::GetInstance(napi_env env) {
    DlnaManager::dlnaAsyncContext_ = new DlnaAsyncContext(env);
    if (DlnaManager::dlnaManager_ == nullptr) {
        std::unique_lock<std::mutex> lock(instanceMutex_);
        DlnaManager::dlnaManager_ = std::shared_ptr<DlnaManager>();
    }
    return DlnaManager::dlnaManager_;
}

void* DlnaManager::ThreadStartDmr(void *) {
//    std::shared_ptr<DlnaPlayer_Listener> listener = std::make_shared<DlnaPlayer_Listener>(DlnaManager::GetInstance());
//    std::shared_ptr<IDlnaController> controler = IDlnaController::GetInstance(listener);
//    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG, "create dmr");
//
////    controler->DmrCreate();
//    controler->DmrCreateExt("");
//
//    while (true) {
//        usleep(1 * 1000 * 1000);
//    }
}

napi_value DlnaManager::StartDmr(napi_env env, napi_callback_info info) {
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG, "DlnaManager::Init");
    napi_value result = nullptr;
    size_t argc = 2;
    size_t res = 0;
    char ip[16];
    napi_value args[2] = { nullptr };

    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    
    napi_get_value_string_utf8(env, args[0], ip, 16, &res);

    listener_ = std::make_shared<DlnaPlayer_Listener>(DlnaManager::GetInstance(env));
    controler_ = IDlnaController::GetInstance(listener_);
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG, "tt==> create dmr. set ip:%{public}s", ip);
    controler_->DmrCreateExt((const char *)ip);

    napi_ref callbackRef;
    napi_create_reference(env, args[1], 1, &callbackRef);
    dlnaAsyncContext_->env_ = env;
    dlnaAsyncContext_->recvCallbackRef = callbackRef;

    napi_get_undefined(env, &result);

    return result;
}

napi_value DlnaManager::NotifyPositionChanged(napi_env env, napi_callback_info info) {
    size_t res = 0;
    size_t argCount = 2;
    napi_value args[2] = { nullptr };
    napi_value result = nullptr;
    int64_t position;
    int64_t duration;

    napi_get_undefined(env, &result);
    napi_get_cb_info(env, info, &argCount, args, nullptr, nullptr);
    
    napi_get_value_int64(env, args[0], &position);
    napi_get_value_int64(env, args[1], &duration);
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG, "event:%{public}d, data%{public}d", position, duration);
    
    json body;
    body["POSITION"] = position;
    body["DURATION"] = duration;
    json data;
    data["CALLBACK_ACTION"] = "onPositionChanged";
    data["DATA"] = body;
    controler_->SendEventChange(0, 0, data.dump());
    
    return result;
}

void DlnaManager::notify(int actionType, std::string data) { 
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG, "notify in");
    DlnaAsyncContext *ctx = (DlnaAsyncContext *)dlnaAsyncContext_;

    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(ctx->env_, &loop);
    if (loop == nullptr) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG, "fail to napi_get_uv_event_loop");
        return;
    }

    ctx->data = data;
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "tt==> ctx->data:%{public}s", ctx->data.c_str());

    uv_work_t *work = new (std::nothrow) uv_work_t;
    work->data = reinterpret_cast<void *>(ctx);
    int ret = uv_queue_work(
        loop, work, [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            DlnaAsyncContext *ctx = reinterpret_cast<DlnaAsyncContext *>(work->data);
            if (ctx != nullptr) {
                ctx->UvWork();
            }
        }
    );
}

napi_value DlnaManager::getDmrData(napi_env env, napi_callback_info info) {
    //OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG, "getDmrData in");
    napi_value result = nullptr;
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    string buffer;
    
    napi_get_undefined(env, &result);

    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if(DlnaManager::dataQueue_.empty()) {
        napi_create_string_utf8(env, "", NAPI_AUTO_LENGTH, &result);
        //OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG, "dataQueue_ is empty");
    } else {
        buffer = DlnaManager::dataQueue_.front();
        DlnaManager::dataQueue_.pop();
        napi_create_string_utf8(env, buffer.c_str(), NAPI_AUTO_LENGTH, &result);
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, TAG, "buffer:%{public}s", buffer.c_str());
    }
    
    return result;
}

//napi_value ExecuteCallback(napi_env env, napi_callback_info info) {
//    size_t argc = 1;
//    napi_value argv[1];
//    napi_value result;
//    char data[1024];
//    
//    napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
//    
//    napi_create_buffer(env, 1024, (void **)&data, &result);
//    
//    napi_value js_global;
//    napi_get_global(env, &js_global);
//    napi_value js_function;
////    napi_get_reference_value(env, argv[0], &js_function);
//}
//
//napi_value DlnaManager::Init(napi_env env, napi_value exports) {
//    napi_value callback;
//    napi_create_function(env, NULL, 0, ExecuteCallback, NULL, &callback);
//
//    napi_ref callback_ref;
//    napi_create_reference(env, callback, 1, &callback_ref);
//    
//    
//    napi_set_named_property(env, exports, "onDataRecieve", callback);
//    
//    return exports;
//}