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
#ifndef NATIVE_XCOMPONENT_PLUGIN_MANAGER_H
#define NATIVE_XCOMPONENT_PLUGIN_MANAGER_H

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <js_native_api.h>
#include <js_native_api_types.h>
#include <napi/native_api.h>
#include <string>
#include <unordered_map>

#include "render/plugin_render.h"
#include "render/plugin_push.h"

namespace NativeXComponentSample {

/* Constants for array index */
const int32_t PARAM0 = 0;
const int32_t PARAM1 = 1;
const int32_t PARAM2 = 2;

/* Constants for array size */
const int32_t ARGS_ZERO = 0;
const int32_t ARGS_ONE = 1;
const int32_t ARGS_TWO = 2;
const int32_t ARGS_THREE = 3;
const int32_t SIZE = 100;

class PluginManager : public ICAudioWrapperCallback{
public:
    ~PluginManager();

    static PluginManager* GetInstance()
    {
        return &PluginManager::pluginManager_;
    }

    static napi_value GetContext(napi_env env, napi_callback_info info);
    static napi_value initPush(napi_env env, napi_callback_info info);
    static napi_value DisconnectPush(napi_env env, napi_callback_info info);
    static napi_value deInitPush(napi_env env, napi_callback_info info);
    static napi_value setXCoponentUrl(napi_env env, napi_callback_info info);
    static napi_value play(napi_env env, napi_callback_info info);
    static napi_value deInitPlay(napi_env env, napi_callback_info info);
    static napi_value sendFrameBuffer(napi_env env, napi_callback_info info);
    static napi_value startAudioRecord(napi_env env, napi_callback_info info);
    static napi_value stopAudioRecord(napi_env env, napi_callback_info info);

    void SetNativeXComponent(std::string& id, OH_NativeXComponent* nativeXComponent);
    PluginRender* GetRender(std::string& id);
    void Export(napi_env env, napi_value exports);

    virtual void OnRecordData(SLuint8 *buffer, SLuint32 size);

private:
    static PluginManager pluginManager_;
    static CAudioWrapper *audioWrapper_;

    std::unordered_map<std::string, OH_NativeXComponent*> nativeXComponentMap_;
    std::unordered_map<std::string, PluginRender*> pluginRenderMap_;
};
} // namespace NativeXComponentSample
#endif // NATIVE_XCOMPONENT_PLUGIN_MANAGER_H
