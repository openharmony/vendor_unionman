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
#ifndef NATIVE_XCOMPONENT_PLUGIN_PUSH_H
#define NATIVE_XCOMPONENT_PLUGIN_PUSH_H

#include <csignal>
#include <hilog/log.h>
#include <multimedia/player_framework/native_avcodec_videodecoder.h>
#include <multimedia/player_framework/native_avcodec_videoencoder.h>
#include <multimedia/player_framework/native_avcodec_base.h>
#include <multimedia/player_framework/native_avformat.h>
#include <multimedia/player_framework/native_avmemory.h>
#include <multimedia/player_framework/native_avcapability.h>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <napi/native_api.h>
#include <string>
#include <unordered_map>

#include <yangutil/sys/YangSysMessageHandle.h>
#include <yangpush/YangPushCommon.h>
#include <yangpush/YangPushFactory.h>
#include <yangutil/buffer/YangVideoDecoderBuffer.h>
#include <yangutil/buffer/YangAudioPlayBuffer.h>
#include "yangplayer/YangPlayerHandle.h"
#include "yangstream/YangStreamType.h"
#include <yangutil/yangavinfotype.h>
#include <yangutil/sys/YangSysMessageI.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangMath.h>
#include "videoenc_ndk_sample.h"
#include <yangutil/yangavtype.h>
#include "AudioWrapper.h"
#include "egl_core.h"
#include "VDecEncNdkSample.h"

using namespace OHOS;
using namespace OHOS::Media;

namespace NativeXComponentSample {
class PluginPush {
public:
    PluginPush();
    ~PluginPush()
    {
        
    }
    static PluginPush* GetInstance()
    {
        return &PluginPush::instance;
    }
    void InitPush(std::string pushUrl);
    void DeInit();
    void DisconnectPush();
    void pushVideoFrame(void *buff, int32_t len);
    void pushAudioFrame(void *buff, unsigned int len);
    bool isPluginPushInit();
    void setPushInitFlag(bool start);

private:
    static PluginPush instance;
    bool isInit;

    int m_videoType;
    bool m_hasAudio;
    OH_AVCapability *capability = nullptr;
    bool isHardware = 0;
    OHOS::Media::VEncNdkSample *vEncCodec_;
    YangContext *m_push_context_ = new YangContext();
    YangVideoInfo m_screenInfo_;
    YangVideoInfo m_outInfo_;
    YangPushFactory *mf_;
    YangSysMessageHandle *sysmessage_;
};
} // namespace NativeXComponentSample
#endif // NATIVE_XCOMPONENT_PLUGIN_PUSH_H
