#pragma once

#include <stddef.h>
#include <SLES/OpenSLES.h>
#include "napi/native_api.h"

const char* GetSLErrorString(size_t code);

// Configures an SL_DATAFORMAT_PCM structure based on native audio parameters.
SLDataFormat_PCM CreatePCMConfiguration(size_t channels, int sample_rate, size_t bits_per_sample);

// Helper class for using SLObjectItf interfaces.
template <typename SLType, typename SLDerefType>
class ScopedSLObject {
 public:
  ScopedSLObject() : obj_(nullptr) {}

  ~ScopedSLObject() { Reset(); }

  SLType* Receive() {
    return &obj_;
  }

  SLDerefType operator->() { return *obj_; }

  SLType Get() const { return obj_; }

  void Reset() {
    if (obj_) {
      (*obj_)->Destroy(obj_);
      obj_ = nullptr;
    }
  }

 private:
  SLType obj_;
};

typedef ScopedSLObject<SLObjectItf, const SLObjectItf_*> ScopedSLObjectItf;

int64_t GetTimeMS();


