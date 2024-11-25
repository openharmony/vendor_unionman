#include "napi/native_api.h"
#include "VDecEncNdkSample.h"
#include <csignal>
#include <hilog/log.h>
#include <multimedia/player_framework/native_avcodec_videodecoder.h>
#include <multimedia/player_framework/native_avcodec_videoencoder.h>
#include <multimedia/player_framework/native_avcodec_base.h>
#include <multimedia/player_framework/native_avformat.h>
#include <multimedia/player_framework/native_avmemory.h>
#include <multimedia/player_framework/native_avcapability.h>
#include "VDecEncNdkSample.h"

#undef LOG_DOMAIN
#undef LOG_TAG
#define LOG_DOMAIN 0x3566 // 全局domain宏，标识业务领域
#define LOG_TAG "OH_AV"   // 全局tag宏，标识模块日志tag
using namespace OHOS;
using namespace OHOS::Media;
using namespace std;

OH_AVCodec *videoDec;
OH_AVCodec *videoEnc;
std::unique_ptr<std::ifstream> inputFile;
std::unique_ptr<std::ofstream> outputFile;
std::unique_ptr<std::ofstream> outFile;
const string MIME_TYPE_AVC = "video/avc";
const string MIME_TYPE_MPEG4 = "video/mp4v-es";
static OHOS::Media::VDecEncNdkSample *vEncCodec = nullptr;
static OHOS::Media::VDecEncNdkSample *vDecCodec = nullptr;
// VDecSignal *signal_ = nullptr;
static OH_AVFormat *videoFormat = nullptr;
namespace {
constexpr int64_t NANOS_IN_SECOND = 1000000000L;
constexpr int64_t NANOS_IN_MICRO = 1000L;
constexpr uint32_t FRAME_INTERVAL = 16666;
constexpr uint32_t IDR_FRAME_INTERVAL = 10;
static const size_t MAX_FILE_PATH_LENGTH = 512;

void clearIntqueue(std::queue<uint32_t> &q) {
    std::queue<uint32_t> empty;
    swap(empty, q);
}

void clearBufferqueue(std::queue<OH_AVCodecBufferAttr> &q) {
    std::queue<OH_AVCodecBufferAttr> empty;
    swap(empty, q);
}
} // namespace

inline bool GetUTF8StringValue(napi_env env, napi_value value, char *buf, size_t bufsize) {
    napi_valuetype valuetype;
    napi_typeof(env, value, &valuetype);
    if (napi_string != valuetype)
        return false;

    size_t result = 0;
    if (napi_ok != napi_get_value_string_utf8(env, value, buf, bufsize, &result))
        return false;

    return true;
}

int64_t GetSystemTimeUs() {
    struct timespec now;
    (void)clock_gettime(CLOCK_BOOTTIME, &now);
    int64_t nanoTime = (int64_t)now.tv_sec * NANOS_IN_SECOND + now.tv_nsec;

    return nanoTime / NANOS_IN_MICRO;
}

// bool CanUseVideoCodec()
//{
//     return canIUse("SystemCapability.Multimedia.Media.CodecBase") &&
//         canIUse("SystemCapability.Multimedia.Media.VideoDecoder") &&
//         canIUse("SystemCapability.Multimedia.Media.VideoEncoder");
// }

// 编码异常回调OH_AVCodecOnError实现
static void VEncError(OH_AVCodec *codec, int32_t errorCode, void *userData) {
    OH_LOG_ERROR(LOG_APP, "OnError, errorCode:%{public}d", errorCode);
}

// 编码数据流变化回调OH_AVCodecOnStreamChanged实现
static void VEncFormatChanged(OH_AVCodec *codec, OH_AVFormat *format, void *userData) {
    OH_LOG_INFO(LOG_APP, "VideoEncoder onStreamChanged");
}

// 编码输入回调OH_AVCodecOnNeedInputData实现
// static void VEncInputDataReady(OH_AVCodec *codec, uint32_t index, OH_AVMemory *data, void *userData) {
//    VEncSignal *signal = static_cast<VEncSignal *>(userData);
//    unique_lock<mutex> lock(signal->inMutex_);
//    signal->inIdxQueue_.push(index);
//    signal->inBufferQueue_.push(data);
//    signal->inCond_.notify_all();
//}

// 编码输出回调OH_AVCodecOnNewOutputData实现
// static void VEncOutputDataReady(OH_AVCodec *codec, uint32_t index, OH_AVMemory *data, OH_AVCodecBufferAttr *attr,
//                                void *userData) {
//    VEncSignal *signal = static_cast<VEncSignal *>(userData);
//    unique_lock<mutex> lock(signal->outMutex_);
//    signal->outIdxQueue_.push(index);
//    signal->attrQueue_.push(*attr);
//    signal->outBufferQueue_.push(data);
//    signal->outCond_.notify_all();
//}

// 解码异常回调OH_AVCodecOnError实现
static void VDecError(OH_AVCodec *codec, int32_t errorCode, void *userData) {
    OH_LOG_ERROR(LOG_APP, "OnError, errorCode:%{public}d", errorCode);
}

// 解码数据流变化回调OH_AVCodecOnStreamChanged实现
static void VDecFormatChanged(OH_AVCodec *codec, OH_AVFormat *format, void *userData) {
    OH_LOG_INFO(LOG_APP, "metaRtc onStreamChanged");
}

// 解码输入回调OH_AVCodecOnNeedInputData实现
// static void OnDecoderNeedInputData(OH_AVCodec *codec, uint32_t index, OH_AVMemory *data, void *userData) {
//    OH_LOG_INFO(LOG_APP, "OnDecoderNeedInputData");
//    VDecSignal *signal_ = static_cast<VDecSignal *>(userData);
//    std::unique_lock<std::mutex> lock(signal_->inMutex_);
//    // 解码输入帧id送入 inQueueDec_
//    signal_->inIdxQueue_.push(index);
//    // 解码输入帧数据送入 inBufferQueueDec_
//    signal_->inBufferQueue_.push(data);
//    signal_->inCond_.notify_all();
//
//    // 配置 buffer info 信息
//    OH_AVCodecBufferAttr info;
//    // 调用 FFmpeg 接口 av_packet_alloc 进行初始化并返回一个容器 pkt
//    //    AVPacket pkt = av_packet_alloc();
//    // 配置 info 的输入尺寸、偏移量、时间戳等字段信息
//    info.size = 6921;
//    info.offset = 0;
//    info.pts = GetSystemTimeUs();
//    info.flags = AVCODEC_BUFFER_FLAGS_NONE;
//    // 送入解码输入队列进行解码, index 为对应队列下标
//    int32_t ret = OH_VideoDecoder_PushInputData(videoDec, index, info);
//    if (ret != AV_ERR_OK) {
//        // 异常处理
//        OH_LOG_ERROR(LOG_APP, "OH_VideoDecoder_PushInputData failed");
//    }
//    OH_LOG_INFO(LOG_APP, "OH_VideoDecoder_PushInputData success");
//}

// 解码输出回调OH_AVCodecOnNewOutputData实现
// static void OnDecoderNeedOutputData(OH_AVCodec *codec, uint32_t index, OH_AVMemory *data, OH_AVCodecBufferAttr *attr,
//                                    void *userData) {
//    OH_LOG_INFO(LOG_APP, "OnDecoderNeedOutputData");
//    VDecSignal *signal_ = static_cast<VDecSignal *>(userData);
//    std::unique_lock<std::mutex> lock(signal_->outMutex_);
//    // 将对应输出 buffer 的 index 送入 outQueueDec_
//    signal_->outIdxQueue_.push(index);
//    // 将对应解码完成的数据 data 送入 outBufferQueueDec_ (注： Surface模式下data为空)
//    signal_->outBufferQueue_.push(data);
//    signal_->attrQueue_.push(*attr);
//    signal_->outCond_.notify_all();
//
//    int32_t ret;
//    // 将解码完成数据 data 写入到对应输出文件中
//    outFile->write(reinterpret_cast<char *>(OH_AVMemory_GetAddr(data)), attr->size);
//    // buffer 模式, 释放已完成写入的数据, index 为对应 surface/buffer 队列下标
//    ret = OH_VideoDecoder_FreeOutputData(videoDec, index);
//    if (ret != AV_ERR_OK) {
//        // 异常处理
//        OH_LOG_ERROR(LOG_APP, "OH_VideoDecoder_FreeOutputData failed");
//    }
//    OH_LOG_INFO(LOG_APP, "OH_VideoDecoder_FreeOutputData success");
//}

//// 编码输入回调OH_AVCodecOnNeedInputData实现
// static void OnNeedInputData(OH_AVCodec *codec, uint32_t index, OH_AVMemory *mem, void *userData) {
//     (void)userData;
//     VDecSignal *signal_ = static_cast<VDecSignal *>(userData);
//     std::unique_lock<std::mutex> lock(signal_->inMutexEnc_);
//     // 输入帧buffer对应的index，送入InIndexQueue队列
//     signal_->inQueueEnc_.push(index);
//     // 输入帧的数据mem送入InBufferQueue队列
//     signal_->inBufferQueueEnc_.push(mem);
//     // 写入编码码流
//     signal_->inCondEnc_.notify_all();
//     // 处理文件流得到帧的长度，再将需要编码的数据写入到对应index的mem中
//     //    int32_t frameSize = GetOneFrameSize();
//     int32_t frameSize = 480 * 320 * 3 / 2;
//     inputFile->read(reinterpret_cast<char *>(OH_AVMemory_GetAddr(mem)), frameSize);
//     // 配置buffer info信息
//     OH_AVCodecBufferAttr info;
//     info.size = frameSize;
//     info.offset = 0;
//     info.pts = 0;
//     info.flags = AVCODEC_BUFFER_FLAGS_CODEC_DATA;
//     // 送入编码输入队列进行编码，index为对应输入队列的下标
//     OH_LOG_INFO(LOG_APP, "index:%{public}d,size:%{public}d,offset:%{public}d,pts:%{public}lld,flags:%{public}d", index, info.size, info.offset, info.pts, info.flags);
//     int32_t ret = OH_VideoEncoder_PushInputData(videoEnc, index, info);
//     if (ret != AV_ERR_OK) {
//         // 异常处理
//         OH_LOG_ERROR(LOG_APP, "OH_VideoEncoder_PushInputData FLAGS_CODEC_DATA failed,ret:%{public}d", ret);
//     }
//     // 通知编码器码流结束
//     //    int32_t ret;
//     //    OH_AVCodecBufferAttr info;
//     info.size = 0;
//     info.offset = 0;
//     info.pts = 0;
//     info.flags = AVCODEC_BUFFER_FLAGS_EOS;
//     OH_LOG_INFO(LOG_APP, "index:%{public}d,size:%{public}d,offset:%{public}d,pts:%{public}lld,flags:%{public}d", index, info.size, info.offset, info.pts, info.flags);
//     ret = OH_VideoEncoder_PushInputData(videoEnc, index, info);
//     if (ret != AV_ERR_OK) {
//         // 异常处理
//         OH_LOG_ERROR(LOG_APP, "OH_VideoEncoder_PushInputData FLAGS_EOS failed,ret:%{public}d", ret);
//     }
// }
//
//// 编码输出回调OH_AVCodecOnNewOutputData实现
// static void OnNeedOutputData(OH_AVCodec *codec, uint32_t index, OH_AVMemory *mem,
//                              OH_AVCodecBufferAttr *attr, void *userData) {
//     (void)userData;
//     VDecSignal *signal_ = static_cast<VDecSignal *>(userData);
//     std::unique_lock<std::mutex> lock(signal_->outMutexEnc_);
//     // 完成帧buffer对应的index，送入outIndexQueue队列
//     signal_->outQueueEnc_.push(index);
//     // 完成帧的数据mem送入outBufferQueue队列
//     signal_->outBufferQueueEnc_.push(mem);
//     // 完成帧的数据格式送入outAttrQueue队列
//     signal_->flagQueueEnc_.push(attr->flags);
//     signal_->outCondEnc_.notify_all();
//     // 输出编码帧
//     OH_LOG_INFO(LOG_APP, "OH_VideoEncoder_FreeOutputData success");
// }

static napi_value OH_VideoDecoder_Init(napi_env env, napi_callback_info info) {
    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    //
    //    videoDec = OH_VideoDecoder_CreateByName(codecName);
    //    if (videoDec == nullptr) {
    //        OH_LOG_ERROR(LOG_APP, "OH_VideoDecoder_CreateByName failed");
    //    }
    //    bool isValid;
    //    OH_VideoDecoder_IsValid(videoDec, &isValid);
    //    if (isValid) {
    //        OH_LOG_INFO(LOG_APP, "OH_VideoDecoder_CreateByName success");
    //    }
    //    OH_AVCodecAsyncCallback cb = {&VDecError, &VDecFormatChanged, &OnDecoderNeedInputData, &OnDecoderNeedOutputData};
    //    // 配置异步回调
    //    int32_t ret = OH_VideoDecoder_SetCallback(videoDec, cb, signal_);
    //    if (ret != AV_ERR_OK) {
    //        // 异常处理
    //        OH_LOG_ERROR(LOG_APP, "OH_VideoDecoder_SetCallback failed");
    //    }

    size_t argc = 2;
    napi_value args[2] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    char inputFilePath[MAX_FILE_PATH_LENGTH + 1] = {};
    if (!GetUTF8StringValue(env, args[0], inputFilePath, MAX_FILE_PATH_LENGTH)) {
        OH_LOG_ERROR(LOG_APP, "get inputFilePath failed");
    }
    OH_LOG_INFO(LOG_APP, "get inputFilePath success, inputFilePath:%{public}s", inputFilePath);
    char outputFilePath[MAX_FILE_PATH_LENGTH + 1] = {};
    if (!GetUTF8StringValue(env, args[1], outputFilePath, MAX_FILE_PATH_LENGTH)) {
        OH_LOG_ERROR(LOG_APP, "get outputFilePath failed");
    }
    OH_LOG_INFO(LOG_APP, "get outputFilePath success, outputFilePath:%{public}s", outputFilePath);

    vDecCodec = new VDecEncNdkSample();

    OH_AVCapability *capability = OH_AVCodec_GetCapabilityByCategory(OH_AVCODEC_MIMETYPE_VIDEO_AVC, false, SOFTWARE);
    OH_LOG_INFO(LOG_APP, "OH_AVCodec_GetCapabilityByCategory success");
    bool isHardware = OH_AVCapability_IsHardware(capability);
    OH_LOG_INFO(LOG_APP, "OH_AVCapability_IsHardware success isHardware: %{public}d", isHardware);
    const char *codecName = OH_AVCapability_GetName(capability); 
    OH_LOG_INFO(LOG_APP, "OH_AVCapability_GetName success codecName: %{public}s", codecName);

    vDecCodec->CreateVideoDecoderByName(codecName);
//    vDecCodec->CreateVideoDecoderByMime(MIME_TYPE_AVC);

    vDecCodec->SetReadPath(inputFilePath);
    vDecCodec->SetEosState(false);
    vDecCodec->SetSavePath(outputFilePath);
    OH_LOG_INFO(LOG_APP, "OH_VideoDecoder_SetCallback success");
    return result;
}

static napi_value OH_VideoEncoder_Init(napi_env env, napi_callback_info info) {
    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    size_t argc = 2;
    napi_value args[2] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    char inputFilePath[MAX_FILE_PATH_LENGTH + 1] = {};
    if (!GetUTF8StringValue(env, args[0], inputFilePath, MAX_FILE_PATH_LENGTH)) {
        OH_LOG_ERROR(LOG_APP, "get inputFilePath failed");
    }
    OH_LOG_INFO(LOG_APP, "get inputFilePath success, inputFilePath:%{public}s", inputFilePath);
    char outputFilePath[MAX_FILE_PATH_LENGTH + 1] = {};
    if (!GetUTF8StringValue(env, args[1], outputFilePath, MAX_FILE_PATH_LENGTH)) {
        OH_LOG_ERROR(LOG_APP, "get outputFilePath failed");
    }
    OH_LOG_INFO(LOG_APP, "get outputFilePath success, outputFilePath:%{public}s", outputFilePath);

    vEncCodec = new VDecEncNdkSample();

    OH_AVCapability *capability = OH_AVCodec_GetCapabilityByCategory(OH_AVCODEC_MIMETYPE_VIDEO_AVC, true, HARDWARE);
    OH_LOG_INFO(LOG_APP, "OH_AVCodec_GetCapabilityByCategory success");
    bool isHardware = OH_AVCapability_IsHardware(capability);
    OH_LOG_INFO(LOG_APP, "OH_AVCapability_IsHardware success isHardware: %{public}d", isHardware);
    const char *codecName = OH_AVCapability_GetName(capability);
    OH_LOG_INFO(LOG_APP, "OH_AVCapability_GetName success codecName: %{public}s", codecName);
    
    vEncCodec->CreateVideoEncoderByName(codecName);

    vEncCodec->SetReadPath(inputFilePath);
    vEncCodec->SetEosState(false);
    vEncCodec->SetSavePath(outputFilePath);

    OH_LOG_INFO(LOG_APP, "OH_VideoEncoder_SetCallback success");
    return result;
}

static napi_value OH_VideoDecoder_Configure(napi_env env, napi_callback_info info) {
    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    // 配置视频帧宽度（必须）
    constexpr uint32_t DEFAULT_WIDTH = 320;
    // 配置视频帧高度（必须）
    constexpr uint32_t DEFAULT_HEIGHT = 240;
    videoFormat = OH_AVFormat_Create();
    // 写入 format
    OH_AVFormat_SetIntValue(videoFormat, OH_MD_KEY_WIDTH, DEFAULT_WIDTH);
    OH_AVFormat_SetIntValue(videoFormat, OH_MD_KEY_HEIGHT, DEFAULT_HEIGHT);
    OH_AVFormat_SetIntValue(videoFormat, OH_MD_KEY_PIXEL_FORMAT, AV_PIXEL_FORMAT_YUVI420);
    // 配置解码器
    vDecCodec->ConfigureDec(videoFormat);
    //    int32_t ret = OH_VideoDecoder_Configure(videoDec, format);
    //    if (ret != AV_ERR_OK) {
    //        // 异常处理
    //        OH_LOG_ERROR(LOG_APP, "OH_VideoDecoder_Configure failed");
    //    }
    //    OH_AVFormat_Destroy(format);
    OH_LOG_INFO(LOG_APP, "OH_VideoDecoder_Configure success");
    return result;
}

static napi_value OH_VideoEncoder_Configure(napi_env env, napi_callback_info info) {
    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    // 配置视频帧宽度（必须）
    constexpr uint32_t DEFAULT_WIDTH = 320;
    // 配置视频帧高度（必须）
    constexpr uint32_t DEFAULT_HEIGHT = 240;
    // 配置视频像素格式（必须）
    constexpr OH_AVPixelFormat DEFAULT_PIXELFORMAT = AV_PIXEL_FORMAT_YUVI420;
    videoFormat = OH_AVFormat_Create();
    // 写入format
    OH_AVFormat_SetIntValue(videoFormat, OH_MD_KEY_WIDTH, DEFAULT_WIDTH);
    OH_AVFormat_SetIntValue(videoFormat, OH_MD_KEY_HEIGHT, DEFAULT_HEIGHT);
    OH_AVFormat_SetIntValue(videoFormat, OH_MD_KEY_PIXEL_FORMAT, DEFAULT_PIXELFORMAT);
    // 配置编码器
    int32_t ret = vEncCodec->ConfigureEnc(videoFormat);
    if (ret == AV_ERR_OK) {
        OH_LOG_INFO(LOG_APP, "OH_VideoEncoder_Configure success");
    }
    return result;
}

static napi_value OH_VideoDecoder_Prepare(napi_env env, napi_callback_info info) {
    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    //    int32_t ret = OH_VideoDecoder_Prepare(videoDec);
    //    if (ret != AV_ERR_OK) {
    //        // 异常处理
    //        OH_LOG_ERROR(LOG_APP, "OH_VideoDecoder_Prepare failed");
    //    }
    vDecCodec->PrepareDec();

    OH_LOG_INFO(LOG_APP, "OH_VideoDecoder_Prepare success");
    return result;
}

static napi_value OH_VideoEncoder_Prepare(napi_env env, napi_callback_info info) {
    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    vEncCodec->PrepareEnc();

    OH_LOG_INFO(LOG_APP, "OH_VideoEncoder_Prepare success");
    return result;
}

static napi_value OH_VideoDecoder_Start(napi_env env, napi_callback_info info) {
    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    //    size_t argc = 2;
    //    napi_value args[2] = {nullptr};
    //    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    //    char inputFilePath[MAX_FILE_PATH_LENGTH + 1] = {};
    //    if (!GetUTF8StringValue(env, args[0], inputFilePath, MAX_FILE_PATH_LENGTH)) {
    //        OH_LOG_ERROR(LOG_APP, "get inputFilePath failed");
    //    }
    //    OH_LOG_INFO(LOG_APP, "get inputFilePath success, inputFilePath:%{public}s", inputFilePath);
    //    char outputFilePath[MAX_FILE_PATH_LENGTH + 1] = {};
    //    if (!GetUTF8StringValue(env, args[1], outputFilePath, MAX_FILE_PATH_LENGTH)) {
    //        OH_LOG_ERROR(LOG_APP, "get outputFilePath failed");
    //    }
    //    OH_LOG_INFO(LOG_APP, "get outputFilePath success, outputFilePath:%{public}s", outputFilePath);
    //    std::unique_ptr<std::ifstream> inFile = std::make_unique<std::ifstream>();
    //    // 打开待解码二进制文件路径
    //    inFile->open(inputFilePath, std::ios::in | std::ios::binary);
    //    // buffer 模式下需要配置
    //    // buffer 模式: 配置解码文件输出路径
    //    outFile = std::make_unique<std::ofstream>();
    //    outFile->open(outputFilePath, std::ios::out | std::ios::binary);
    //    // 开始解码
    //    int32_t ret = OH_VideoDecoder_Start(videoDec);
    //    if (ret != AV_ERR_OK) {
    //        // 异常处理
    //        OH_LOG_ERROR(LOG_APP, "OH_VideoDecoder_Start failed");
    //    }
    vDecCodec->StartDec();

    OH_LOG_INFO(LOG_APP, "OH_VideoDecoder_Start success");
    return result;
}

static napi_value OH_VideoEncoder_Start(napi_env env, napi_callback_info info) {
    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    vEncCodec->StartEnc();

    OH_LOG_INFO(LOG_APP, "OH_VideoEncoder_Start success");
    return result;
}

static napi_value OH_VideoDecoder_Stop(napi_env env, napi_callback_info info) {
    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    //    int32_t ret;
    //    // 终止解码器 videoDec
    //    ret = OH_VideoDecoder_Stop(videoDec);
    //    if (ret != AV_ERR_OK) {
    //        // 异常处理
    //        OH_LOG_ERROR(LOG_APP, "OH_VideoEncoder_Stop failed");
    //    }
    while (!vDecCodec->GetDecEosState()) {
    };
    vDecCodec->StopDec();

    OH_LOG_INFO(LOG_APP, "OH_VideoDecoder_Stop success");
    return result;
}

static napi_value OH_VideoEncoder_Stop(napi_env env, napi_callback_info info) {
    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    while (!vEncCodec->GetEncEosState()) {
    };
    vEncCodec->StopEnc();

    OH_LOG_INFO(LOG_APP, "OH_VideoEncoder_Stop success");
    return result;
}

static napi_value OH_VideoDecoder_Destroy(napi_env env, napi_callback_info info) {
    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    //    int32_t ret;
    //    // 调用 OH_VideoDecoder_Destroy, 注销解码器
    //    ret = OH_VideoDecoder_Destroy(videoDec);
    //    if (ret != AV_ERR_OK) {
    //        // 异常处理
    //        OH_LOG_ERROR(LOG_APP, "OH_VideoEncoder_Destroy failed");
    //    }
    vDecCodec->ReleaseDec();
    OH_AVFormat_Destroy(videoFormat);

    OH_LOG_INFO(LOG_APP, "OH_VideoEncoder_Destroy success");
    return result;
}

static napi_value OH_VideoEncoder_Destroy(napi_env env, napi_callback_info info) {
    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    vEncCodec->ReleaseEnc();
    OH_AVFormat_Destroy(videoFormat);

    OH_LOG_INFO(LOG_APP, "OH_VideoEncoder_Destroy success");
    return result;
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        {"OH_VideoDecoder_Init", nullptr, OH_VideoDecoder_Init, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"OH_VideoEncoder_Init", nullptr, OH_VideoEncoder_Init, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"OH_VideoDecoder_Configure", nullptr, OH_VideoDecoder_Configure, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"OH_VideoEncoder_Configure", nullptr, OH_VideoEncoder_Configure, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"OH_VideoDecoder_Prepare", nullptr, OH_VideoDecoder_Prepare, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"OH_VideoEncoder_Prepare", nullptr, OH_VideoEncoder_Prepare, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"OH_VideoDecoder_Start", nullptr, OH_VideoDecoder_Start, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"OH_VideoEncoder_Start", nullptr, OH_VideoEncoder_Start, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"OH_VideoDecoder_Stop", nullptr, OH_VideoDecoder_Stop, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"OH_VideoEncoder_Stop", nullptr, OH_VideoEncoder_Stop, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"OH_VideoDecoder_Destroy", nullptr, OH_VideoDecoder_Destroy, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"OH_VideoEncoder_Destroy", nullptr, OH_VideoEncoder_Destroy, nullptr, nullptr, nullptr, napi_default, nullptr}};
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "entry",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterEntryModule(void) {
    napi_module_register(&demoModule);
}
