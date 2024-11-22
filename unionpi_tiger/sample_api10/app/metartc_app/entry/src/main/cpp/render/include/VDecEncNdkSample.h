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

#ifndef VIDEODECENC_NDK_SAMPLE_H
#define VIDEODECENC_NDK_SAMPLE_H

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
#include <yangutil/yangavinfotype.h>

namespace OHOS {
namespace Media {
class VDecEncSignal {
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

class VDecEncNdkSample : public NoCopyable {
public:
    VDecEncNdkSample(YangContext* pcontext);
    ~VDecEncNdkSample();

    void SetEosState(bool needSetEos);
    void CreateVideoDecoderByName(std::string name);
    int32_t ConfigureDec(struct OH_AVFormat *aformat, struct OH_AVFormat *vformat);
    int32_t SetParameterDec(struct OH_AVFormat *aformat, struct OH_AVFormat *vformat);
    int32_t SetOutputSurface(OHNativeWindow *window);
    int32_t PrepareDec();
    int32_t StartDec();
    int32_t StopDec();
    int32_t FlushDec();
    int32_t ResetDec();
    int32_t ReleaseDec();
    int32_t CalcuError();
    void SetReadPath(std::string filepath);
    void SetSavePath(std::string filepath);
    void ReRead();
    void ResetDecParam();
    int32_t GetFrameCount();
    bool GetEncEosState();
    bool GetDecEosState();
    void PopInqueueVideoDec();
    void PopOutqueueVideoDec();
    int32_t PushInbufferVideoDec(uint32_t index, uint32_t bufferSize);
    VDecEncSignal* vcodecSignal_;
    bool isDecInputEOS = false;
    bool isEncInputEOS = false;
    bool isDecOutputEOS = false;
    bool isEncOutputEOS = false;
    bool setEos = true;
    bool needRender = true;

private:
    OHNativeWindow *nativeWindow_;
    struct OH_AVCodec* vdec_;
    void InputFuncVideoDec();
    void OutputFuncVideoDec();
    int32_t WriteToFile();
    int32_t WriteToFile_pcm();
    std::atomic<bool> isDecRunning_;
    std::unique_ptr<std::ifstream> testFile_;
    std::unique_ptr<std::thread> inputLoopVideoDec_;
    std::unique_ptr<std::thread> outputLoopVideoDec_;
    struct OH_AVCodecAsyncCallback cbVideoDec_;
    int64_t timeStampVdec_ = 0;
    uint32_t decInCnt_ = 0;
    uint32_t decOutCnt_ = 0;

    bool isFirstDecFrame_ = true;
    bool isFirstEncFrame_ = true;
    int64_t timeStampEnc_ = 0;
    uint32_t encInCnt_ = 0;
    uint32_t encOutCnt_ = 0;
    std::string inFile_ = "/data/media/out_320_240_10s.h264";
    std::string outFile_ = "/data/media/video_out.es";
	
    YangContext *m_context;
    bool decStartFlag;
};
}
}
#endif // VIDEODECENC_NDK_SAMPLE_H
