/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef VIDEOENC_NDK_SAMPLE_H
#define VIDEOENC_NDK_SAMPLE_H

#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <atomic>
#include <fstream>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include <unordered_map>
//#include "securec.h"
#include <napi/native_api.h>
#include "multimedia/player_framework/native_avcodec_videoencoder.h"
#include "nocopyable.h"
#include <multimedia/player_framework/native_avcodec_base.h>
#include "multimedia/player_framework/native_avmemory.h"
#include "multimedia/player_framework/native_avformat.h"
#include "multimedia/player_framework/native_averrors.h"
//#include "surface/window.h"
//#include "media_description.h"
//#include "av_common.h"
#include "native_window/external_window.h"
#include "native_buffer/native_buffer.h"
#include <yangutil/yangavinfotype.h>
#include <yangutil/yangavctype.h>
namespace OHOS {
namespace Media {
class VEncSignal {
public:
    std::mutex inMutex_;
    std::mutex outMutex_;
    std::condition_variable inCond_;
    std::condition_variable outCond_;
    std::queue<uint32_t> inIdxQueue_;
    std::queue<uint32_t> outIdxQueue_;
    std::queue<OH_AVCodecBufferAttr> attrQueue_;
    std::queue<OH_AVMemory *> inBufferQueue_;
    std::queue<OH_AVMemory *> outBufferQueue_;
};

class VEncNdkSample : public NoCopyable {
public:
    VEncNdkSample(YangContext *pcontext);
    ~VEncNdkSample();
    const char *INP_DIR = "/data/test/media/1280_720_nv.yuv";
    const char *OUT_DIR = "/data/test/media/VEncTest.h264";
    uint32_t DEFAULT_WIDTH = 640;
    uint32_t DEFAULT_HEIGHT = 480;
    uint32_t DEFAULT_BITRATE = 10000000;
    double DEFAULT_FRAME_RATE = 30.0;
    uint32_t DEFAULT_KEY_FRAME_INTERVAL = 1000;
    uint32_t repeat_time = 0;
    int32_t CreateVideoEncoder(const char *codecName);
    int32_t ConfigureVideoEncoder();
    int32_t ConfigureVideoEncoder_fuzz(int32_t data);
    int32_t SetVideoEncoderCallback();
    int32_t CreateSurface();
    int32_t OpenFileFail();
    int32_t StartVideoEncoder();
    void SetParameter(OH_AVFormat *format);
    void testApi();
    void WaitForEOS();
    int64_t GetSystemTimeUs();
    int32_t Start();
    int32_t Flush();
    int32_t Reset();
    int32_t Stop();
    int32_t Release();
    void Flush_buffer();
    void RepeatStartBeforeEOS();
    bool RandomEOS(uint32_t index);
    void SetEOS(uint32_t index);
    int32_t PushData(OH_AVMemory *buffer, uint32_t index, int32_t &result);
    int32_t CheckResult(bool isRandomEosSuccess, int32_t pushResult);
    void InputFunc();
    int32_t state_EOS();
    void InputFuncSurface();
    uint32_t ReadOneFrameYUV420SP(uint8_t *dst);
    int32_t CheckAttrFlag(OH_AVCodecBufferAttr attr);
    void OutputFuncFail();
    void OutputFunc();
    uint32_t FlushSurf(OHNativeWindowBuffer *ohNativeWindowBuffer, OH_NativeBuffer *nativeBuffer);
    void ReleaseSignal();
    void ReleaseInFile();
    void StopInloop();
    void StopOutloop();

    VEncSignal *signal_;
    uint32_t errCount = 0;
    bool enableForceIDR = false;
    uint32_t outCount = 0;
    uint32_t frameCount = 0;

    bool sleepOnFPS = false;
    bool SURFACE_INPUT = false;
    bool repeatRun = false;
    bool showLog = false;
    int64_t encode_count = 0;
    bool enable_random_eos = false;
    uint32_t REPEAT_START_STOP_BEFORE_EOS = 0;  // 1200 测试用例
    uint32_t REPEAT_START_FLUSH_BEFORE_EOS = 0; // 1300 测试用例
    int64_t start_time = 0;
    int64_t end_time = 0;

private:
    std::atomic<bool> isRunning_ { false };
    std::unique_ptr<std::ifstream> inFile_;
    FILE *inFileYUV;
    std::unique_ptr<std::thread> inputLoop_;
    std::unique_ptr<std::thread> outputLoop_;
    std::unordered_map<uint32_t, OH_AVMemory *> inBufferMap_;
    std::unordered_map<uint32_t, OH_AVMemory *> outBufferMap_;
    OH_AVCodec *venc_;
    OH_AVCodecAsyncCallback cb_;
    int64_t timeStamp_ { 0 };
    int64_t lastRenderedTimeUs_ { 0 };
    bool isFirstFrame_ = true;
    OHNativeWindow *nativeWindow;
    int stride_;
    static constexpr uint32_t SAMPLE_RATIO = 2;
    YangContext *m_context;
};
} // namespace Media
} // namespace OHOS

#endif // VIDEODEC_NDK_SAMPLE_H
