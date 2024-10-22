//
// Created on 2024/7/25.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef DlnaPlayer_dlna_log_H
#define DlnaPlayer_dlna_log_H

#include <hilog/log.h>

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

#define DLNA_LOG_TAG  "[OH_DLNA]"

#define DLNA_LOGI(...)                                                                                                 \
    (OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "%{public}s:%{public}d %{public}s", __func__, __LINE__,   \
                        ##__VA_ARGS__))

#define DLNA_LOGD(...)                                                                                                 \
    ((void)OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "%{public}s:%{public}d %{public}s", __func__, __LINE__,   \
                        ##__VA_ARGS__))

#define DLNA_LOGE(...)                                                                                                 \
    (OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG, "%{public}s:%{public}d %{public}s", __func__, __LINE__,  \
                        ##__VA_ARGS__))

#define DLNA_LOGW(...)                                                                                                 \
    ((void)OH_LOG_Print(LOG_APP, LOG_WARN, 0xFF00, DLNA_LOG_TAG, "%{public}s:%{public}d %{public}s", __func__, __LINE__,  \
                        ##__VA_ARGS__))

} // namespace CastSessionDlna
}     // namespace CastEngineService
}         // namespace CastEngine
} // namespace OHOS
#endif //DlnaPlayer_dlna_log_H
