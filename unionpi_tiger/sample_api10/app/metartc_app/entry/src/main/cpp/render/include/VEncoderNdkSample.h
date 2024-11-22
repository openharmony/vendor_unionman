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

#ifndef VIDEO_ENCODER_NDK_SAMPLE_H
#define VIDEO_ENCODER_NDK_SAMPLE_H

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <atomic>
#include <fstream>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include "nocopyable.h"
#include <multimedia/player_framework/native_avcodec_base.h>
#include <multimedia/player_framework/native_avcodec_videodecoder.h>
#include <multimedia/player_framework/native_avcodec_videoencoder.h>

namespace OHOS {
namespace Media {
class VEncoderSignal {
public:
    std::mutex inMutexDec_;
    std::mutex outMutexDec_;
    std::condition_variable inCondDec_;
    std::condition_variable outCondDec_;
    std::queue<uint32_t> inQueueDec_;
    std::queue<uint32_t> outQueueDec_;
    std::queue<uint32_t> flagQueueDec_;
    std::queue<OH_AVMemory *> inBufferQueueDec_;
    std::queue<OH_AVMemory *> outBufferQueueDec_;

    std::mutex inMutexEnc_;
    std::mutex outMutexEnc_;
    std::condition_variable inCondEnc_;
    std::condition_variable outCondEnc_;
    std::queue<uint32_t> inQueueEnc_;
    std::queue<uint32_t> outQueueEnc_;
    std::queue<uint32_t> sizeQueueEnc_;
    std::queue<uint32_t> flagQueueEnc_;
    std::queue<OH_AVMemory *> inBufferQueueEnc_;
    std::queue<OH_AVMemory *> outBufferQueueEnc_;
    int32_t errorNum_ = 0;
    std::atomic<bool> isVdecFlushing_;
    std::atomic<bool> isVencFlushing_;
};

class VEncoderNdkSample : public NoCopyable {
public:
    VEncoderNdkSample() = default;
    ~VEncoderNdkSample();

    void SetEosState(bool needSetEos);
    int32_t SetOutputSurface(OHNativeWindow *window);
    struct OH_AVCodec* CreateVideoEncoderByMime(std::string mimetype);
    struct OH_AVCodec* CreateVideoEncoderByName(std::string name);
    int32_t ConfigureEnc(struct OH_AVFormat *format);
    int32_t SetParameterEnc(struct OH_AVFormat *format);
    int32_t GetSurface();
    int32_t PrepareEnc();
    int32_t StartEnc();
    int32_t StopEnc();
    int32_t FlushEnc();
    int32_t ResetEnc();
    int32_t ReleaseEnc();
    
    int32_t CalcuError();
    void ResetEncParam();
    int32_t GetFrameCount();
    bool GetEncEosState();
    void PopInqueueEnc();
    void PopOutqueueEnc();
    void SendEncEos();
    int32_t PushInbufferEnc(uint32_t index, uint32_t bufferSize);
    VEncoderSignal* vencoderSignal_;
    bool isEncInputEOS = false;
    bool isEncOutputEOS = false;
    bool setEos = true;
    bool needRender = true;

private:
    OHNativeWindow *nativeWindow_;
    int32_t WriteToFile();
    std::unique_ptr<std::ifstream> testFile_;

    struct OH_AVCodec* venc_;
    void InputFuncEnc();
    void OutputFuncEnc();
    std::atomic<bool> isEncRunning_;
    std::unique_ptr<std::thread> inputLoopEnc_;
    std::unique_ptr<std::thread> outputLoopEnc_;
    struct OH_AVCodecAsyncCallback cbEnc_;
    bool isFirstEncFrame_ = true;
    int64_t timeStampEnc_ = 0;
    uint32_t encInCnt_ = 0;
    uint32_t encOutCnt_ = 0;
    std::string inFile_ = "/data/media/out_320_240_10s.h264";
    std::string outFile_ = "/data/media/video_out.es";
//    std::string inFile_ = "/data/media/in_1920_1080_10s.h264";
//    std::string outFile_ = "/data/media/video_out.es";
};
}
}
#endif // VIDEODECENC_NDK_SAMPLE_H
