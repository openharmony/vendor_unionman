/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include <multimedia/player_framework/native_avcodec_audiodecoder.h>
#include <multimedia/player_framework/native_avcodec_audioencoder.h>
#include <multimedia/player_framework/native_avcodec_videodecoder.h>
#include <multimedia/player_framework/native_avcodec_videoencoder.h>
#include <multimedia/player_framework/native_avcodec_base.h>
#include <multimedia/player_framework/native_avformat.h>
#include <multimedia/player_framework/native_avmemory.h>
#include <multimedia/player_framework/native_avcapability.h>
#include "log.h"
#include "VDecEncNdkSample.h"

using namespace OHOS;
using namespace OHOS::Media;
using namespace std;

namespace {
constexpr uint32_t VIDEO_SAMPLE_DURATION_US = 23000;
constexpr int32_t STOPNUM = 10000;
FILE *outFileYUV = nullptr;
uint32_t wirte_yuv_count = 0;

//video
void VdecAsyncError(OH_AVCodec *codec, int32_t errorCode, void *userData) {
    LOGI("%{public}s, %{public}d, DEC Error errorCode=%{public}d", __FUNCTION__, __LINE__, errorCode);
    VDecEncSignal *vcodecSignal_ = static_cast<VDecEncSignal *>(userData);
    vcodecSignal_->errorNum_ += 1;
}

void VdecAsyncStreamChanged(OH_AVCodec *codec, OH_AVFormat *format, void *userData) {
    LOGI("%{public}s, %{public}d, DEC Format Changed", __FUNCTION__, __LINE__);
}
void VdecAsyncNeedInputData(OH_AVCodec *codec, uint32_t index, OH_AVMemory *data, void *userData) {
    VDecEncSignal *vcodecSignal_ = static_cast<VDecEncSignal *>(userData);
    unique_lock<mutex> lock(vcodecSignal_->inMutexDec_);
    if (vcodecSignal_->isVdecFlushing_.load()) {
        LOGI("%{public}s, %{public}d, VdecAsyncNeedInputData isVdecFlushing_ is true, return", __FUNCTION__, __LINE__);
        return;
    }
    vcodecSignal_->inQueueDec_.push(index);
    vcodecSignal_->inBufferQueueDec_.push(data);
    vcodecSignal_->inCondDec_.notify_all();
    //LOGI("VdecAsyncNeedInputData: index: %{public}d", index);
}
void VdecAsyncNewOutputData(OH_AVCodec *codec, uint32_t index, OH_AVMemory *data,
                            OH_AVCodecBufferAttr *attr, void *userData) {
    //LOGI("VdecAsyncNewOutputData enter.");
    VDecEncSignal *vcodecSignal_ = static_cast<VDecEncSignal *>(userData);
    unique_lock<mutex> lock(vcodecSignal_->outMutexDec_);
    if (vcodecSignal_->isVdecFlushing_.load()) {
        LOGI("VdecAsyncNewOutputData isVdecFlushing_ is true, return");
        return;
    }
    //将对应输出buffer的index送入outQueue_
    vcodecSignal_->outQueueDec_.push(index);
    vcodecSignal_->sizeQueueEnc_.push(attr->size);
    vcodecSignal_->flagQueueDec_.push(attr->flags);
    //（surface模式下data为空）将对应解码完成的数据data送入outbufferQueue_
    vcodecSignal_->outBufferQueueEnc_.push(data);
    vcodecSignal_->outCondDec_.notify_all();
}

void clearIntqueue(std::queue<uint32_t> &q) {
    std::queue<uint32_t> empty;
    swap(empty, q);
}

void clearBufferqueue(std::queue<OH_AVMemory *> &q) {
    std::queue<OH_AVMemory *> empty;
    swap(empty, q);
}
} // namespace

int memcpy_s(void *dest, size_t destSize, const void *src, size_t count) {
    if (dest == nullptr || src == nullptr) {
        return -1;
    }

    if (destSize < count) {
        return -1;
    }

    char *dst = (char *)dest;
    const char *srcPtr = (const char *)src;

    while (count-- > 0) {
        *dst++ = *srcPtr++;
    }

    return 0;
}

VDecEncNdkSample::VDecEncNdkSample(YangContext* pcontext)
{
	m_context = pcontext;
    decStartFlag = false;
    vcodecSignal_ = nullptr;
}

VDecEncNdkSample::~VDecEncNdkSample() {
    if(outFileYUV != nullptr)
        fclose(outFileYUV);
    yang_delete(vcodecSignal_);
}

void VDecEncNdkSample::SetReadPath(std::string filepath) {
    LOGI("SetReadPath %{public}s", filepath.c_str());
    inFile_ = filepath;
    if (testFile_ == nullptr) {
        testFile_ = std::make_unique<std::ifstream>();
    }
    testFile_->open(inFile_, std::ios::in | std::ios::binary);
}

void VDecEncNdkSample::SetSavePath(std::string filepath) {
    outFile_ = filepath;
}

void VDecEncNdkSample::SetEosState(bool needSetEos) {
    setEos = needSetEos;
}

void VDecEncNdkSample::ReRead() {
    if (testFile_ != nullptr) {
        testFile_->close();
        LOGI("%{public}s, %{public}d, ReRead close before file success", __FUNCTION__, __LINE__);
    }
    LOGI("%{public}s, %{public}d, ReRead inFile is", __FUNCTION__, __LINE__);
    testFile_->open(inFile_, std::ios::in | std::ios::binary);
    if (testFile_ != nullptr) {
        LOGI("%{public}s, %{public}d, testFile open success", __FUNCTION__, __LINE__);
    }
    decInCnt_ = 0;
    isFirstDecFrame_ = true;
    timeStampVdec_ = 0;
}

void VDecEncNdkSample::ResetDecParam() {
    decInCnt_ = 0;
    decOutCnt_ = 0;
    isDecOutputEOS = false;

    unique_lock<mutex> lockVideoIn(vcodecSignal_->inMutexDec_);
    clearIntqueue(vcodecSignal_->inQueueDec_);
    clearBufferqueue(vcodecSignal_->inBufferQueueDec_);
    vcodecSignal_->inCondDec_.notify_all();
    unique_lock<mutex> lockVideoOut(vcodecSignal_->outMutexDec_);
    clearIntqueue(vcodecSignal_->outQueueDec_);
    clearIntqueue(vcodecSignal_->flagQueueDec_);
    clearBufferqueue(vcodecSignal_->outBufferQueueDec_);
    vcodecSignal_->outCondDec_.notify_all();
    LOGI("%{public}s, %{public}d, isDecRunning_.load()=%{public}d", __FUNCTION__, __LINE__, isDecRunning_.load());
}

void VDecEncNdkSample::CreateVideoDecoderByName(std::string name) {
    LOGI("%{public}s, %{public}d, name.c_str() = %{public}s", __FUNCTION__, __LINE__, name.c_str());
    vdec_ = OH_VideoDecoder_CreateByName(name.c_str());
    if(vdec_ == nullptr) {
        LOGI("%{public}s, %{public}d, Fatal: OH_VideoDecoder_CreateByName", __FUNCTION__, __LINE__);
        return;
    }
    if (vcodecSignal_ == nullptr) {
        vcodecSignal_ = new VDecEncSignal();
        if(vcodecSignal_ == nullptr) {
            LOGI("%{public}s, %{public}d, Fatal: vcodecSignal_", __FUNCTION__, __LINE__);
            return;
        }
    }
    cbVideoDec_.onError = VdecAsyncError;
    cbVideoDec_.onStreamChanged = VdecAsyncStreamChanged;
    cbVideoDec_.onNeedInputData = VdecAsyncNeedInputData;
    cbVideoDec_.onNeedOutputData = VdecAsyncNewOutputData;
    int32_t ret = OH_VideoDecoder_SetCallback(vdec_, cbVideoDec_, static_cast<void *>(vcodecSignal_));
    if(ret != AV_ERR_OK) {
        LOGI("%{public}s, %{public}d, Fatal: OH_VideoDecoder_SetCallback", __FUNCTION__, __LINE__);
        return;
    }
    return;
}

int32_t VDecEncNdkSample::ConfigureDec(struct OH_AVFormat *aformat, struct OH_AVFormat *vformat) {
    LOGI("%{public}s, %{public}d, start", __FUNCTION__, __LINE__);
    OH_VideoDecoder_Configure(vdec_, vformat);
    return 0;
}

int32_t VDecEncNdkSample::SetParameterDec(struct OH_AVFormat *aformat, struct OH_AVFormat *vformat) {
    LOGI("%{public}s, %{public}d, start", __FUNCTION__, __LINE__);
    OH_VideoDecoder_SetParameter(vdec_, vformat);
    return 0;
}

int32_t VDecEncNdkSample::PrepareDec() {
    LOGI("%{public}s, %{public}d, start", __FUNCTION__, __LINE__);
    OH_VideoDecoder_Prepare(vdec_);
    return 0;
}

int32_t VDecEncNdkSample::StartDec() {
    LOGI("%{public}s, %{public}d, Enter dec start", __FUNCTION__, __LINE__);
    isDecRunning_.store(true);
    #if 1
    if (inputLoopVideoDec_ == nullptr) {
        inputLoopVideoDec_ = make_unique<thread>(&VDecEncNdkSample::InputFuncVideoDec, this);
        CHECK_AND_RETURN_RET_LOG(inputLoopVideoDec_ != nullptr, AV_ERR_UNKNOWN, "Fatal: No memory");
    }
    if (outputLoopVideoDec_ == nullptr) {
        outputLoopVideoDec_ = make_unique<thread>(&VDecEncNdkSample::OutputFuncVideoDec, this);
        CHECK_AND_RETURN_RET_LOG(outputLoopVideoDec_ != nullptr, AV_ERR_UNKNOWN, "Fatal: No memory");
    }
    #endif
    LOGI("%{public}s, %{public}d,  outFile_.c_str():%{public}s", __FUNCTION__, __LINE__, outFile_.c_str());
    outFileYUV = fopen(outFile_.c_str(), "wb+");
    if(outFileYUV == nullptr)
        LOGI("%{public}s, %{public}d, outFileYUV fail", __FUNCTION__, __LINE__);
    OH_VideoDecoder_Start(vdec_);
    LOGI("Exit dec start");
    return 0;
}

int32_t VDecEncNdkSample::StopDec() {
    LOGI("%{public}s, %{public}d, EXIT DEC stop", __FUNCTION__, __LINE__);
    unique_lock<mutex> videolock(vcodecSignal_->inMutexDec_);
    unique_lock<mutex> videolock2(vcodecSignal_->outMutexDec_);
    vcodecSignal_->isVdecFlushing_.store(true);
    videolock.unlock();
    videolock2.unlock();
    OH_VideoDecoder_Stop(vdec_);
    unique_lock<mutex> lockVideoIn(vcodecSignal_->inMutexDec_);
    clearIntqueue(vcodecSignal_->inQueueDec_);
    clearBufferqueue(vcodecSignal_->inBufferQueueDec_);
    vcodecSignal_->inCondDec_.notify_all();
    lockVideoIn.unlock();
    unique_lock<mutex> lockVideoOut(vcodecSignal_->outMutexDec_);
    clearIntqueue(vcodecSignal_->outQueueDec_);
    clearIntqueue(vcodecSignal_->flagQueueDec_);
    clearBufferqueue(vcodecSignal_->outBufferQueueDec_);
    vcodecSignal_->outCondDec_.notify_all();
    lockVideoOut.unlock();
    vcodecSignal_->isVdecFlushing_.store(false);
    LOGI("%{public}s, %{public}d, EXIT DEC stop", __FUNCTION__, __LINE__);
    return 0;
}

int32_t VDecEncNdkSample::FlushDec() {
    LOGI("%{public}s, %{public}d, EXIT DEC FLUSH", __FUNCTION__, __LINE__);
    unique_lock<mutex> videolock(vcodecSignal_->inMutexDec_);
    unique_lock<mutex> videolock2(vcodecSignal_->outMutexDec_);
    vcodecSignal_->isVdecFlushing_.store(true);
    videolock.unlock();
    videolock2.unlock();
    OH_VideoDecoder_Flush(vdec_);
    unique_lock<mutex> lockVideoIn(vcodecSignal_->inMutexDec_);
    clearIntqueue(vcodecSignal_->inQueueDec_);
    clearBufferqueue(vcodecSignal_->inBufferQueueDec_);
    vcodecSignal_->inCondDec_.notify_all();
    lockVideoIn.unlock();
    unique_lock<mutex> lockVideoOut(vcodecSignal_->outMutexDec_);
    clearIntqueue(vcodecSignal_->outQueueDec_);
    clearIntqueue(vcodecSignal_->flagQueueDec_);
    clearBufferqueue(vcodecSignal_->outBufferQueueDec_);
    vcodecSignal_->outCondDec_.notify_all();
    lockVideoOut.unlock();
    vcodecSignal_->isVdecFlushing_.store(false);
    LOGI("%{public}s, %{public}d, EXIT DEC FLUSH", __FUNCTION__, __LINE__);
    return 0;
}

int32_t VDecEncNdkSample::ResetDec() {
    LOGI("%{public}s, %{public}d, Enter DEC reset", __FUNCTION__, __LINE__);
    unique_lock<mutex> videolock(vcodecSignal_->inMutexDec_);
    unique_lock<mutex> videolock2(vcodecSignal_->outMutexDec_);
    vcodecSignal_->isVdecFlushing_.store(true);
    videolock.unlock();
    videolock2.unlock();
    OH_VideoDecoder_Reset(vdec_);
    unique_lock<mutex> lockVideoIn(vcodecSignal_->inMutexDec_);
    clearIntqueue(vcodecSignal_->inQueueDec_);
    clearBufferqueue(vcodecSignal_->inBufferQueueDec_);
    vcodecSignal_->inCondDec_.notify_all();
    lockVideoIn.unlock();
    unique_lock<mutex> lockVideoOut(vcodecSignal_->outMutexDec_);
    clearIntqueue(vcodecSignal_->outQueueDec_);
    clearIntqueue(vcodecSignal_->flagQueueDec_);
    clearBufferqueue(vcodecSignal_->outBufferQueueDec_);
    vcodecSignal_->outCondDec_.notify_all();
    lockVideoOut.unlock();
    vcodecSignal_->isVdecFlushing_.store(false);
    LOGI("%{public}s, %{public}d, Enter DEC reset", __FUNCTION__, __LINE__);
    return 0;
}

int32_t VDecEncNdkSample::ReleaseDec() {
    LOGI("%{public}s, %{public}d, Enter DEC release", __FUNCTION__, __LINE__);
    isDecRunning_.store(false);
    if (inputLoopVideoDec_ != nullptr && inputLoopVideoDec_->joinable()) {
        unique_lock<mutex> lock(vcodecSignal_->inMutexDec_);
        vcodecSignal_->inQueueDec_.push(STOPNUM);
        vcodecSignal_->inCondDec_.notify_all();
        lock.unlock();
        inputLoopVideoDec_->join();
        inputLoopVideoDec_.reset();
    }
    if (outputLoopVideoDec_ != nullptr && outputLoopVideoDec_->joinable()) {
        unique_lock<mutex> lock(vcodecSignal_->outMutexDec_);
        vcodecSignal_->outQueueDec_.push(STOPNUM);
        vcodecSignal_->outCondDec_.notify_all();
        lock.unlock();
        outputLoopVideoDec_->join();
        outputLoopVideoDec_.reset();
    }
    OH_VideoDecoder_Destroy(vdec_);
    LOGI("%{public}s, %{public}d, Exit DEC release", __FUNCTION__, __LINE__);
    return AV_ERR_OK;
}

void VDecEncNdkSample::PopInqueueVideoDec() {
    if (vcodecSignal_ == nullptr) {
        return;
    }
    vcodecSignal_->inQueueDec_.pop();
    vcodecSignal_->inBufferQueueDec_.pop();
}

int32_t VDecEncNdkSample::PushInbufferVideoDec(uint32_t index, uint32_t bufferSize) {
    if (vdec_ == nullptr) {
        return AV_ERR_INVALID_VAL;
    }
    struct OH_AVCodecBufferAttr attr;
    attr.offset = 0;
    attr.pts = timeStampVdec_;
    attr.size = bufferSize;
    if (isFirstDecFrame_) {
        attr.flags = AVCODEC_BUFFER_FLAGS_CODEC_DATA;
        isFirstDecFrame_ = false;
    } else {
        attr.flags = AVCODEC_BUFFER_FLAGS_NONE;
    }
    return OH_VideoDecoder_PushInputData(vdec_, index, attr);
}

void VDecEncNdkSample::InputFuncVideoDec() {
    LOGE("%{public}s, %{public}d start", __FUNCTION__, __LINE__);
    uint8_t *srcVideo = new uint8_t[1024*1024];
    while (true) {
        if (!isDecRunning_.load()) {
            break;
        }
        unique_lock<mutex> lock(vcodecSignal_->inMutexDec_);
        vcodecSignal_->inCondDec_.wait(lock, [this]() { return vcodecSignal_->inQueueDec_.size() > 0; });
        if (!isDecRunning_.load()) {
            break;
        }
        uint32_t index = vcodecSignal_->inQueueDec_.front();
        OH_AVMemory *buffer = reinterpret_cast<OH_AVMemory *>(vcodecSignal_->inBufferQueueDec_.front());
        if (vcodecSignal_->isVdecFlushing_.load() || buffer == nullptr) {
            PopInqueueVideoDec();
            continue;
        }
        uint32_t bufferSize = 0;
        if (m_context->metaVideoBuffer && m_context->metaVideoBuffer->size() == 0)
        {
            //LOGE("%{public}s, %{public}d NO DATA", __FUNCTION__, __LINE__);
            usleep(20*1000);
            continue;
        } else {
            YangFrame videoFrame;
	        memset(&videoFrame,0,sizeof(YangFrame));
            videoFrame.payload = srcVideo;
            m_context->metaVideoBuffer->getEVideo(&videoFrame);
            if(videoFrame.frametype == YANG_Frametype_Spspps) {
                decStartFlag = true;        
            }
            if(decStartFlag == true) {
                char *videoBuffer = nullptr;
                if (videoFrame.frametype == YANG_Frametype_Spspps) {
                    int sps_len = videoFrame.payload[12];
                    int pps_len = videoFrame.payload[12+sps_len+3];
                    bufferSize = 4 + sps_len + 4 + pps_len;
                    videoBuffer = (char *)malloc(sizeof(char)*(bufferSize));    
					videoBuffer[0] = 0x00;
					videoBuffer[1] = 0x00;
					videoBuffer[2] = 0x00;
					videoBuffer[3] = 0x01;
                    memcpy(&videoBuffer[4], &videoFrame.payload[13], sps_len);
                    videoBuffer[sps_len+4] = 0x00;
					videoBuffer[sps_len+5] = 0x00;
					videoBuffer[sps_len+6] = 0x00;
					videoBuffer[sps_len+7] = 0x01;
                    memcpy(&videoBuffer[sps_len+8], &videoFrame.payload[13+sps_len+3], pps_len); 
                } else {
                    bufferSize = videoFrame.nb;
                    videoBuffer = (char *)malloc(sizeof(char)*(bufferSize + 1));
                    memcpy(videoBuffer, videoFrame.payload, videoFrame.nb);
                    videoBuffer[0] = 0x00;
					videoBuffer[1] = 0x00;
					videoBuffer[2] = 0x00;
					videoBuffer[3] = 0x01;
                }
                //continue;
                //LOGI("%{public}s, %{public}d, index:%{public}d, videoFrame.frametype:%{public}d, videoFrame.nb:%{public}d", __FUNCTION__, __LINE__, index, videoFrame.frametype, videoFrame.nb);
                if (memcpy_s(OH_AVMemory_GetAddr(buffer), OH_AVMemory_GetSize(buffer), videoBuffer, bufferSize) != 0 || buffer == nullptr) {
                    free(videoBuffer);
                    PopInqueueVideoDec();
                    LOGE("%{public}s, %{public}d memcpy_s fail", __FUNCTION__, __LINE__);
                    continue;
                }
                free(videoBuffer);
            } else {
                LOGE("%{public}s, %{public}d wariting the sps and pps", __FUNCTION__, __LINE__);
                continue;
            }
        }
        if (PushInbufferVideoDec(index, bufferSize) != AV_ERR_OK) {
            LOGE("Fatal: OH_VideoDecoder_PushInputData fail, exit");
            vcodecSignal_->errorNum_ += 1;
        } else {
            decInCnt_++;
        }
        timeStampVdec_ += VIDEO_SAMPLE_DURATION_US;
        PopInqueueVideoDec();
    }
    yang_deleteA(srcVideo);
    LOGE("%{public}s, %{public}d end", __FUNCTION__, __LINE__);
}

void VDecEncNdkSample::PopOutqueueVideoDec() {
    if (vcodecSignal_ == nullptr) {
        return;
    }
    vcodecSignal_->outQueueDec_.pop();
    vcodecSignal_->flagQueueDec_.pop();
}

void VDecEncNdkSample::OutputFuncVideoDec() {
    LOGE("%{public}s, %{public}d start", __FUNCTION__, __LINE__);
    while (true) {
        if (!isDecRunning_.load()) {
            break;
        }
        unique_lock<mutex> lock(vcodecSignal_->outMutexDec_);
        vcodecSignal_->outCondDec_.wait(lock, [this]() { return vcodecSignal_->outQueueDec_.size() > 0; });
        if (!isDecRunning_.load()) {
            break;
        }
        if (vcodecSignal_->isVdecFlushing_.load() || isEncInputEOS) {
            PopOutqueueVideoDec();
            continue;
        }

        uint32_t index = vcodecSignal_->outQueueDec_.front();
        uint32_t outflag = vcodecSignal_->flagQueueDec_.front();
        if (outflag == 0) {
            uint32_t ret;
            if (needRender) {
                ret = OH_VideoDecoder_RenderOutputData(vdec_, index);
            } else {
                if (WriteToFile() != 0) {
                    LOGE("WriteToFile failed");
                    PopOutqueueVideoDec();
                    continue;
                }
                ret = OH_VideoDecoder_FreeOutputData(vdec_, index);
            }
            if (ret == 0) {
                decOutCnt_ += 1;
            } else {
                LOGE("OutputFuncVideoDec. Fatal: ReleaseOutputBuffer fail");
                vcodecSignal_->errorNum_ += 1;
                break;
            }
        } else {
            LOGI("%{public}s, %{public}d, DEC OUT.: output EOS", __FUNCTION__, __LINE__);
            isDecOutputEOS = true;
        }
        PopOutqueueVideoDec();
    }
}


int32_t VDecEncNdkSample::SetOutputSurface(OHNativeWindow *window) {
    nativeWindow_ = window;
    LOGI("%{public}s, %{public}d 111", __FUNCTION__, __LINE__);
    //nativeWindow_->surface->SetQueueSize(32);
    int32_t setSurfaceRet = OH_VideoDecoder_SetSurface(vdec_, nativeWindow_);
    OH_AVFormat *format = OH_AVFormat_Create();
    // 配置显示旋转角度
    OH_AVFormat_SetIntValue(format, OH_MD_KEY_ROTATION, 90);
    // 配置视频与显示屏匹配模式(缩放与显示窗口适配, 裁剪与显示窗口适配)
    OH_AVFormat_SetIntValue(format, OH_MD_KEY_SCALING_MODE, SCALING_MODE_SCALE_CROP);
    OH_AVFormat_Destroy(format);
    return setSurfaceRet;
}

int32_t VDecEncNdkSample::WriteToFile() {
    auto buffer = vcodecSignal_->outBufferQueueEnc_.front();
    uint32_t size = vcodecSignal_->sizeQueueEnc_.front();
    if(wirte_yuv_count > 100)
        return 0;
    if (buffer == nullptr) {
        LOGE("getOutPut Buffer fail");
        return AV_ERR_INVALID_VAL;
    }
    
    if (outFileYUV == nullptr) {
        LOGE("dump data fail");
        return AV_ERR_INVALID_VAL;
    } else {
        LOGI("%{public}s, %{public}d, size:%{public}d", __FUNCTION__, __LINE__, size);
        fwrite(OH_AVMemory_GetAddr(buffer), 1, size, outFileYUV);
        wirte_yuv_count++;
    }
    return 0;
}

int32_t VDecEncNdkSample::CalcuError() {
    LOGI("%{public}s, %{public}d, errorNum_ is= %{public}d", __FUNCTION__, __LINE__, vcodecSignal_->errorNum_);
    LOGI("%{public}s, %{public}d, decInCnt_ is= %{public}d", __FUNCTION__, __LINE__, decInCnt_);
    LOGI("%{public}s, %{public}d, decOutCnt_ is= %{public}d", __FUNCTION__, __LINE__, decOutCnt_);
    LOGI("%{public}s, %{public}d, encOutCnt_ is= %{public}d", __FUNCTION__, __LINE__, encOutCnt_);
    LOGI("%{public}s, %{public}d, DEC inQueueDec_.size() is= %{public}lu", __FUNCTION__, __LINE__, vcodecSignal_->inQueueDec_.size());
    LOGI("%{public}s, %{public}d, DEC outQueueDec_.size() is= %{public}lu", __FUNCTION__, __LINE__, vcodecSignal_->outQueueDec_.size());
    LOGI("%{public}s, %{public}d, DEC outBufferQueueDec_.size() is= %{public}lu", __FUNCTION__, __LINE__, vcodecSignal_->outBufferQueueDec_.size());
    LOGI("%{public}s, %{public}d, DEC outQueueEnc_.size() is = %{public}lu", __FUNCTION__, __LINE__, vcodecSignal_->outQueueEnc_.size());
    return vcodecSignal_->errorNum_;
}

int32_t VDecEncNdkSample::GetFrameCount() {
    return encOutCnt_;
}
bool VDecEncNdkSample::GetEncEosState() {
    return isEncOutputEOS;
}
bool VDecEncNdkSample::GetDecEosState() {
    return isDecOutputEOS;
}