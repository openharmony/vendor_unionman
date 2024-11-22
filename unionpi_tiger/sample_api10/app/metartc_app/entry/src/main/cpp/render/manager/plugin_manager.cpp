/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "plugin_manager.h"

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <cstdint>
#include <cstdio>
#include <hilog/log.h>
#include <string>

#include "common/common.h"

namespace NativeXComponentSample {
static const size_t MAX_FILE_PATH_LENGTH = 512;

PluginManager PluginManager::pluginManager_;
CAudioWrapper* PluginManager::audioWrapper_ = new CAudioWrapper(&pluginManager_);

PluginManager::~PluginManager()
{
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "Callback", "~PluginManager");
    for (auto iter = nativeXComponentMap_.begin(); iter != nativeXComponentMap_.end(); ++iter) {
        if (iter->second != nullptr) {
            delete iter->second;
            iter->second = nullptr;
        }
    }
    nativeXComponentMap_.clear();

    for (auto iter = pluginRenderMap_.begin(); iter != pluginRenderMap_.end(); ++iter) {
        if (iter->second != nullptr) {
            delete iter->second;
            iter->second = nullptr;
        }
    }
    pluginRenderMap_.clear();
}

napi_value PluginManager::GetContext(napi_env env, napi_callback_info info)
{
    if ((env == nullptr) || (info == nullptr)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "GetContext env or info is null");
        return nullptr;
    }

    size_t argCnt = 1;
    napi_value args[1] = { nullptr };
    if (napi_get_cb_info(env, info, &argCnt, args, nullptr, nullptr) != napi_ok) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "GetContext napi_get_cb_info failed");
    }

    if (argCnt != 1) {
        napi_throw_type_error(env, NULL, "Wrong number of arguments");
        return nullptr;
    }

    napi_valuetype valuetype;
    if (napi_typeof(env, args[0], &valuetype) != napi_ok) {
        napi_throw_type_error(env, NULL, "napi_typeof failed");
        return nullptr;
    }

    if (valuetype != napi_number) {
        napi_throw_type_error(env, NULL, "Wrong type of arguments");
        return nullptr;
    }

    int64_t value;
    if (napi_get_value_int64(env, args[0], &value) != napi_ok) {
        napi_throw_type_error(env, NULL, "napi_get_value_int64 failed");
        return nullptr;
    }

    napi_value exports;
    if (napi_create_object(env, &exports) != napi_ok) {
        napi_throw_type_error(env, NULL, "napi_create_object failed");
        return nullptr;
    }

    return exports;
}

napi_value PluginManager::initPush(napi_env env, napi_callback_info info){
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "initPush is called");
    size_t requireArgc = 2;
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    napi_value result = nullptr;
    char buffer[100];
    size_t length = 0;

    napi_get_cb_info(env, info, &argc, args , nullptr, nullptr);

    // get xcomponent name
    napi_get_value_string_utf8(env, args[0], buffer, 100, &length);
    std::string pushUrl = std::string(buffer);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "initPush pushUrl:%{public}s", pushUrl.c_str());

    PluginPush *instance = PluginPush::GetInstance();
    instance->InitPush(pushUrl);
    instance->setPushInitFlag(true);
    
    napi_get_undefined(env, &result);
    
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "initPush end");
    return result;
}

napi_value PluginManager::DisconnectPush(napi_env env, napi_callback_info info) {
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "DisconnectPush is called");
    size_t requireArgc = 2;
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    napi_value result = nullptr;
    char buffer[100];
    size_t length = 0;

    napi_get_cb_info(env, info, &argc, args , nullptr, nullptr);

    PluginPush *instance = PluginPush::GetInstance();
    instance->DisconnectPush();
    
    napi_get_undefined(env, &result);
    
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "DisconnectPush end");
    return result;
}

napi_value PluginManager::deInitPush(napi_env env, napi_callback_info info){
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "deInitPush is called");
    size_t requireArgc = 2;
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    napi_value result = nullptr;
    char buffer[100];
    size_t length = 0;

    napi_get_cb_info(env, info, &argc, args , nullptr, nullptr);

    PluginPush *instance = PluginPush::GetInstance();
    instance->setPushInitFlag(false);
    instance->DeInit();
    
    napi_get_undefined(env, &result);
    
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "deInitPush end");
    return result;
}

napi_value PluginManager::setXCoponentUrl(napi_env env, napi_callback_info info){
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "setXCoponentUrl is called");
    size_t requireArgc = 2;
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    napi_value result = nullptr;
    char buffer[100];
    size_t length = 0;

    napi_get_cb_info(env, info, &argc, args , nullptr, nullptr);

    // get xcomponent name
    napi_get_value_string_utf8(env, args[0], buffer, 100, &length);
    std::string xComponentId = std::string(buffer);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "setXCoponentUrl xComponentId:%{public}s", buffer);

    // get url
    napi_get_value_string_utf8(env, args[1], buffer, 100, &length);
    std::string url = std::string(buffer);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "setXCoponentUrl url:%{public}s", buffer);

    // get play pcmfile
    napi_get_value_string_utf8(env, args[2], buffer, 100, &length);
    std::string pcmfile = std::string(buffer);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "setXCoponentUrl pcmfile:%{public}s", buffer);
    
    auto context = PluginRender::GetInstance(xComponentId);
    context->play(url, pcmfile);
    
    napi_get_undefined(env, &result);
    
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "setXCoponentUrl end");
    return result;
}

napi_value PluginManager::deInitPlay(napi_env env, napi_callback_info info) {
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "deInitPlay is called 22");
    size_t requireArgc = 2;
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    napi_value result = nullptr;
    char buffer[100];
    size_t length = 0;

    napi_get_cb_info(env, info, &argc, args , nullptr, nullptr);

    // get xcomponent name
    napi_get_value_string_utf8(env, args[0], buffer, 100, &length);
    std::string xComponentId = std::string(buffer);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "deInitPlay xComponentId:%{public}s", buffer);

    auto context = PluginRender::GetInstance(xComponentId);
    context->stopAvPlayer();
    // context->Release(xComponentId);
    
    napi_get_undefined(env, &result);
    
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "deInitPlay end");
    return result;
}

napi_value PluginManager::play(napi_env env, napi_callback_info info){
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "play is called");
    napi_value result = nullptr;
    size_t argc = ARGS_ONE;
    char buffer[SIZE];
    size_t res = 0;
    napi_value argv[ARGS_THREE] = {0};
    napi_value thisVar = nullptr;

    napi_get_undefined(env, &result);

    if (napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr) != napi_ok) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "napi_get_cb_info failed");
        return nullptr;
    }

    // get xcomponent name
    napi_get_value_string_utf8(env, argv[PARAM0], buffer, SIZE, &res);
    std::string xComponentId = std::string(buffer);

    // get url
    napi_get_value_string_utf8(env, argv[PARAM1], buffer, SIZE, &res);
    std::string url = std::string(buffer);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "url:%{public}s", buffer);

    // get play pcmfile
    napi_get_value_string_utf8(env, argv[PARAM2], buffer, SIZE, &res);
    std::string pcmfile = std::string(buffer);
    
    std::string idStr1("xcomponent1");
    auto context = PluginRender::GetInstance(idStr1);
    context->play(url, pcmfile);
    // std::string idStr2("xcomponent2");
    // auto context2 = PluginRender::GetInstance(idStr2);
    // context2->play(url);
    
    return result;
}

napi_value PluginManager::sendFrameBuffer(napi_env env, napi_callback_info info) {
    size_t requireArgc = 2;
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_value result = nullptr;
    void *buff_data = NULL;
    size_t byte_length = 0;

    napi_get_undefined(env, &result);

    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    napi_get_arraybuffer_info(env, args[0], &buff_data, &byte_length);

    PluginPush *instance = PluginPush::GetInstance();
    instance->pushVideoFrame(buff_data, byte_length);
    
    return result;
}

napi_value PluginManager::startAudioRecord(napi_env env, napi_callback_info info){
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "startAudioRecord is called");
    size_t requireArgc = 2;
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    napi_value result = nullptr;
    char buffer[100];
    size_t length = 0;

    napi_get_undefined(env, &result);
    napi_get_cb_info(env, info, &argc, args , nullptr, nullptr);

    char szDstRecordPCMPath[MAX_FILE_PATH_LENGTH + 1] = {};
    napi_get_value_string_utf8(env, args[0], szDstRecordPCMPath, MAX_FILE_PATH_LENGTH, &length);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "szDstRecordPCMPath:%{public}s", szDstRecordPCMPath);
    
    if(!audioWrapper_->StartAudioRecorder(szDstRecordPCMPath))
    {
        //LOGE("Start(), audioWrapper_.Start() failed");
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "Start(), audioWrapper_.Start() failed");
        return result;        
    }
    
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "startAudioRecord end");
    return result;
}

napi_value PluginManager::stopAudioRecord(napi_env env, napi_callback_info info){
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "stopAudioRecord is called");
    size_t requireArgc = 2;
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    napi_value result = nullptr;
    char buffer[100];
    size_t length = 0;

    napi_get_undefined(env, &result);
    napi_get_cb_info(env, info, &argc, args , nullptr, nullptr);

    audioWrapper_->StopAudioRecorder();
    
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "stopAudioRecord end");
    return result;
}

void PluginManager::Export(napi_env env, napi_value exports)
{
    if ((env == nullptr) || (exports == nullptr)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "Export: env or exports is null");
        return;
    }
    
    napi_value exportInstance = nullptr;
    if (napi_get_named_property(env, exports, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance) != napi_ok) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "Export: napi_get_named_property fail");
        return;
    }

    OH_NativeXComponent* nativeXComponent = nullptr;
    if (napi_unwrap(env, exportInstance, reinterpret_cast<void**>(&nativeXComponent)) != napi_ok) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "Export: napi_unwrap fail");
        return;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = { '\0' };
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        OH_LOG_Print(
            LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "Export: OH_NativeXComponent_GetXComponentId fail");
        return;
    }

    std::string id(idStr);
    auto context = PluginManager::GetInstance();
    if ((context != nullptr) && (nativeXComponent != nullptr)) {
        context->SetNativeXComponent(id, nativeXComponent);
        auto render = context->GetRender(id);
        if (render != nullptr) {
            render->RegisterCallback(nativeXComponent);
            render->Export(env, exports);
        }
    }

}

void PluginManager::SetNativeXComponent(std::string& id, OH_NativeXComponent* nativeXComponent)
{
    if (nativeXComponent == nullptr) {
        return;
    }

    if (nativeXComponentMap_.find(id) == nativeXComponentMap_.end()) {
        nativeXComponentMap_[id] = nativeXComponent;
        return;
    }

    if (nativeXComponentMap_[id] != nativeXComponent) {
        OH_NativeXComponent* tmp = nativeXComponentMap_[id];
        delete tmp;
        tmp = nullptr;
        nativeXComponentMap_[id] = nativeXComponent;
    }
}

void PluginManager::OnRecordData(SLuint8 *buffer, SLuint32 size)
{
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "OnRecordData is called");
    #if 0
    static FILE *fp = fopen("/data/storage/el2/base/haps/entry/files/record_2.pcm", "wb");
    if(!buffer || !size)
        return;
    if(fp) {
        fwrite(buffer, 1, size, fp);
        fflush(fp);
    } else {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_PRINT_DOMAIN, "PluginManager", "fp is null");
    }
    #endif
    PluginPush *instance = PluginPush::GetInstance();
    instance->pushAudioFrame(buffer, size);
}
PluginRender* PluginManager::GetRender(std::string& id)
{
    if (pluginRenderMap_.find(id) == pluginRenderMap_.end()) {
        PluginRender* instance = PluginRender::GetInstance(id);
        pluginRenderMap_[id] = instance;
        return instance;
    }

    return pluginRenderMap_[id];
}
} // namespace NativeXComponentSample
