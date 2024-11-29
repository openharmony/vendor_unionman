#include "time.h"

#include "hilog/log.h"
#define LOGI(...) ((void)OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "OpenSLESCommon", __VA_ARGS__))
#define LOGD(...) ((void)OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "OpenSLESCommon", __VA_ARGS__))
#define LOGW(...) ((void)OH_LOG_Print(LOG_APP, LOG_WARN, LOG_DOMAIN, "OpenSLESCommon", __VA_ARGS__))
#define LOGE(...) ((void)OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "OpenSLESCommon", __VA_ARGS__))

#include "opensles_common.h"

const char* GetSLErrorString(size_t code)
{
    static const char* sl_error_strings[] = {
      "SL_RESULT_SUCCESS",                 // 0
      "SL_RESULT_PRECONDITIONS_VIOLATED",  // 1
      "SL_RESULT_PARAMETER_INVALID",       // 2
      "SL_RESULT_MEMORY_FAILURE",          // 3
      "SL_RESULT_RESOURCE_ERROR",          // 4
      "SL_RESULT_RESOURCE_LOST",           // 5
      "SL_RESULT_IO_ERROR",                // 6
      "SL_RESULT_BUFFER_INSUFFICIENT",     // 7
      "SL_RESULT_CONTENT_CORRUPTED",       // 8
      "SL_RESULT_CONTENT_UNSUPPORTED",     // 9
      "SL_RESULT_CONTENT_NOT_FOUND",       // 10
      "SL_RESULT_PERMISSION_DENIED",       // 11
      "SL_RESULT_FEATURE_UNSUPPORTED",     // 12
      "SL_RESULT_INTERNAL_ERROR",          // 13
      "SL_RESULT_UNKNOWN_ERROR",           // 14
      "SL_RESULT_OPERATION_ABORTED",       // 15
      "SL_RESULT_CONTROL_LOST",            // 16
    };

    if (code > 16)
        return "SL_RESULT_UNKNOWN_ERROR";
    
    return sl_error_strings[code];
}

SLDataFormat_PCM CreatePCMConfiguration(size_t channels, int sample_rate, size_t bits_per_sample)
{
  SLDataFormat_PCM format;
  format.formatType = SL_DATAFORMAT_PCM;
  format.numChannels = static_cast<SLuint32>(channels);

  switch (sample_rate) {
    case 8000:
      format.samplesPerSec = SL_SAMPLINGRATE_8;
      break;
    case 16000:
      format.samplesPerSec = SL_SAMPLINGRATE_16;
      break;
    case 22050:
      format.samplesPerSec = SL_SAMPLINGRATE_22_05;
      break;
    case 32000:
      format.samplesPerSec = SL_SAMPLINGRATE_32;
      break;
    case 44100:
      format.samplesPerSec = SL_SAMPLINGRATE_44_1;
      break;
    case 48000:
      format.samplesPerSec = SL_SAMPLINGRATE_48;
      break;
    case 64000:
      format.samplesPerSec = SL_SAMPLINGRATE_64;
      break;
    case 88200:
      format.samplesPerSec = SL_SAMPLINGRATE_88_2;
      break;
    case 96000:
      format.samplesPerSec = SL_SAMPLINGRATE_96;
      break;
    default:
      break;
  }
  format.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
  format.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
  format.endianness = SL_BYTEORDER_LITTLEENDIAN;
  if (format.numChannels == 1) {
    format.channelMask = SL_SPEAKER_FRONT_CENTER;
  } else if (format.numChannels == 2) {
    format.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
  } else {
    LOGE("Unsupported number of channels: %{public}lu", format.numChannels);
  }
    
  return format;
}

int64_t GetTimeMS()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}
