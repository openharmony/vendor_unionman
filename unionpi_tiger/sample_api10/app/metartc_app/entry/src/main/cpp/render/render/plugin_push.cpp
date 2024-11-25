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

#include <cstdint>
#include <hilog/log.h>
#include <js_native_api.h>
#include <js_native_api_types.h>
#include <string>

#include "common/common.h"
#include "manager/plugin_manager.h"
#include "plugin_push.h"

#include "napi/native_api.h"

#undef LOG_DOMAIN
#undef LOG_TAG
#define LOG_DOMAIN 0x3566 // 全局domain宏，标识业务领域
#define LOG_TAG "metaRtc"   // 全局tag宏，标识模块日志tag
using namespace OHOS;
using namespace OHOS::Media;
using namespace std;

namespace NativeXComponentSample {
    namespace {
        constexpr int64_t NANOS_IN_SECOND = 1000000000L;
        constexpr int64_t NANOS_IN_MICRO = 1000L;
        constexpr uint32_t FRAME_INTERVAL = 16666;
        constexpr uint32_t IDR_FRAME_INTERVAL = 10;
        static const size_t MAX_FILE_PATH_LENGTH = 512;
        const char *codecName = nullptr;
    }

    int64_t GetSystemTimeUs() {
        struct timespec now;
        (void)clock_gettime(CLOCK_BOOTTIME, &now);
        int64_t nanoTime = (int64_t)now.tv_sec * NANOS_IN_SECOND + now.tv_nsec;

        return nanoTime / NANOS_IN_MICRO;
    }
    
    PluginPush PluginPush::instance;
    
    PluginPush::PluginPush():
        isInit(false) {
    }

    
    bool PluginPush::isPluginPushInit(){
        return isInit;
    }
    void PluginPush::setPushInitFlag(bool start) {
        isInit = start;
    }

    void PluginPush::InitPush(std::string pushUrl){
        if(isInit == true)
            return;
        
        OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d", __FUNCTION__, __LINE__);
        yang_reindex(m_push_context_->metaVideoEncoderBuffer);
        yang_reindex(m_push_context_->metaCaptureVideoBuffer);
        yang_reindex(m_push_context_->metaCaptureAudioBuffer);
        m_push_context_->init();
        m_push_context_->avinfo.video.videoEncoderFormat = YangI420;
        m_push_context_->avinfo.enc.createMeta = 0;
        m_push_context_->avinfo.audio.enableMono = yangfalse;
        m_push_context_->avinfo.audio.sample = 48000;
        m_push_context_->avinfo.audio.channel = 2;
        m_push_context_->avinfo.audio.enableAec = yangfalse;
        m_push_context_->avinfo.audio.audioCacheNum = 8;
        m_push_context_->avinfo.audio.audioCacheSize = 8;
        m_push_context_->avinfo.audio.audioPlayCacheNum = 8;
        m_push_context_->avinfo.video.videoCacheNum = 10;
        m_push_context_->avinfo.video.evideoCacheNum = 10;
        m_push_context_->avinfo.video.videoPlayCacheNum = 10;
        m_push_context_->avinfo.audio.audioEncoderType = Yang_AED_OPUS;
        m_push_context_->avinfo.rtc.rtcLocalPort = 17000;
        m_push_context_->avinfo.enc.enc_threads = 4;
        OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d", __FUNCTION__, __LINE__);
        memcpy(&m_screenInfo_, &m_push_context_->avinfo.video, sizeof(YangVideoInfo));
        m_screenInfo_.width = 640;
        m_screenInfo_.height = 480;
        m_screenInfo_.outWidth = 640;
        m_screenInfo_.outHeight = 480;
        OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d", __FUNCTION__, __LINE__);
        m_push_context_->avinfo.sys.mediaServer = Yang_Server_Srs; // Yang_Server_Srs/Yang_Server_Zlm/Yang_Server_Whip_Whep
        m_push_context_->avinfo.rtc.rtcLocalPort = 10000 + yang_random() % 15000;
        OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d", __FUNCTION__, __LINE__);
        m_videoType = Yang_VideoSrc_Camera; // Yang_VideoSrc_Camera/Yang_VideoSrc_Screen;
        char s[128] = {0};
        OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d", __FUNCTION__, __LINE__);
        memset(m_push_context_->avinfo.rtc.localIp, 0, sizeof(m_push_context_->avinfo.rtc.localIp));
        yang_getLocalInfo(m_push_context_->avinfo.sys.familyType, m_push_context_->avinfo.rtc.localIp);
        OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d", __FUNCTION__, __LINE__);
        m_hasAudio = m_videoType == Yang_VideoSrc_Screen ? false : true;
        m_push_context_->avinfo.rtc.enableDatachannel = yangfalse;
        // using h264 h265
        m_push_context_->avinfo.video.videoEncoderType = Yang_VED_264; // Yang_VED_265;
        if (m_push_context_->avinfo.video.videoEncoderType == Yang_VED_265) {
            m_push_context_->avinfo.enc.createMeta = 1;
            m_push_context_->avinfo.rtc.enableDatachannel = yangfalse;
        }
        OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d", __FUNCTION__, __LINE__);
        m_push_context_->avinfo.rtc.iceCandidateType = YangIceHost;
        // srs do not use audio fec
        m_push_context_->avinfo.audio.enableAudioFec = yangfalse;
        OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d", __FUNCTION__, __LINE__);
        mf_ = new YangPushFactory();
        sysmessage_ = mf_->createPushMessageHandle(m_hasAudio, false, m_videoType, &m_screenInfo_, &m_outInfo_,
                                                m_push_context_, nullptr, nullptr);
        OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d", __FUNCTION__, __LINE__);
        sysmessage_->start();
        usleep(1000 * 1000);
        yang_post_message(YangM_Push_Connect, 0, NULL, (void *)pushUrl.c_str());
        // endoder
        OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d", __FUNCTION__, __LINE__);
        vEncCodec_ = new OHOS::Media::VEncNdkSample(m_push_context_);
        capability = OH_AVCodec_GetCapabilityByCategory(OH_AVCODEC_MIMETYPE_VIDEO_AVC, true, HARDWARE);
        isHardware = OH_AVCapability_IsHardware(capability);
        OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d, OH_AVCapability_IsHardware success isHardware: %{public}d", __FUNCTION__, __LINE__, isHardware);
        codecName = OH_AVCapability_GetName(capability);
        OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d, OH_AVCapability_GetName success codecName: %{public}s", __FUNCTION__, __LINE__, codecName);
        vEncCodec_->CreateVideoEncoder(codecName);
        if (vEncCodec_->SetVideoEncoderCallback() != 0) {
            OH_LOG_INFO(LOG_APP, "SetVideoEncoderCallback fail");
            return ;
        }
        OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d putVideo end", __FUNCTION__, __LINE__);
        if (vEncCodec_->ConfigureVideoEncoder() != 0) {
            OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d, ConfigureVideoEncoder fail", __FUNCTION__, __LINE__);
            return ;
        }
        OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d putVideo end", __FUNCTION__, __LINE__);
        if (vEncCodec_->StartVideoEncoder() != 0) {
            OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d, StartVideoEncoder fail",  __FUNCTION__, __LINE__);
            return ;
        }

        OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d putVideo end", __FUNCTION__, __LINE__);
    }

    void PluginPush::DisconnectPush() {
        yang_post_message(YangM_Push_Disconnect,0,NULL);
    }

    void PluginPush::DeInit() {   
        OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d, start", __FUNCTION__, __LINE__);
        yang_post_message(YangM_Push_Disconnect,0,NULL);
        if(vEncCodec_) {
            vEncCodec_->state_EOS();
            vEncCodec_->Stop();
            vEncCodec_->Release();
            //yang_delete(vEncCodec_);  //打开会崩溃thread
            OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d, vEncCodec_ success!", __FUNCTION__, __LINE__);
        }
        usleep(200 * 1000);
        yang_delete(mf_);
        OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d, end", __FUNCTION__, __LINE__);
    }

    void PluginPush::pushVideoFrame(void *buff, int32_t len){
        
        //OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d start!len%{public}d", __FUNCTION__, __LINE__, len);
        YangFrame videoFrame;
        memset(&videoFrame,0,sizeof(YangFrame));
        videoFrame.payload = (uint8_t*)buff;
        videoFrame.nb = 640*480*3/2;
        videoFrame.pts = GetSystemTimeUs();
        m_push_context_->metaCaptureVideoBuffer->putVideo(&videoFrame);
        return ;
    }

    void PluginPush::pushAudioFrame(void *buff, unsigned int len){
        
        //OH_LOG_INFO(LOG_APP, "%{public}s, %{public}d start!len%{public}d", __FUNCTION__, __LINE__, len);
        
        YangFrame audioFrame;
        memset(&audioFrame,0,sizeof(YangFrame));
        audioFrame.payload = (uint8_t*)buff;
        audioFrame.nb = len;
        audioFrame.pts = GetSystemTimeUs();
        m_push_context_->metaCaptureAudioBuffer->putAudio(&audioFrame);
        return ;
    }
} // namespace NativeXComponentSample
