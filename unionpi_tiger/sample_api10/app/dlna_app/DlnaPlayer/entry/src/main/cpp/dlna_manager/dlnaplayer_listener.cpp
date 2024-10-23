//
// Created on 2024/8/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "dlnaplayer_listener.h"
#include "dlna_log.h"
#include "json.hpp"
#include <bits/alltypes.h>

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

    using nlohmann::json;

    #define DLNA_LOG_TAG "Dlna_dmr_player_listener"

    DlnaPlayer_Listener::DlnaPlayer_Listener(std::shared_ptr<DlnaManager> manager) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "DlnaPlayer_Listener in");
        dlnaManager_ = manager;
    }

    bool DlnaPlayer_Listener::NotifyEvent(int event) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "event:%{public}d", event);

        return true;
    }

    void DlnaPlayer_Listener::NotifyEventChange(int moduleId, int event, const std::string &param) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "modleid:%{public}d, event:%{public}d, param:%{public}s", moduleId, event, param.c_str());
        std::lock_guard<std::mutex> lock(jsonMutex_);
        json data;
        std::string keyAction;
        json keyData;
        data.accept(param);
        data = json::parse(param, nullptr, false);
    
        keyAction = data["ACTION"];
        if (keyAction == "Play") {
            dlnaManager_->notify(1, "PLAY");
        } else if (keyAction == "Pause") {
            dlnaManager_->notify(1, "PAUSE");
        } else if (keyAction == "Stop") {
            dlnaManager_->notify(1, "STOP");
        } else if (keyAction == "SetAVTransportURI") {
            keyData = data["DATA"];
            std::string url = keyData["MEDIA_URL"];
//            OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "url:%{public}s", url.c_str());
            dlnaManager_->notify(1, url);
        } else if (keyAction == "Seek") {
            keyData = data["DATA"];
            int64_t pos = keyData["POSITION"];
            std::string posStr = std::to_string(pos);
            OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "pos:%{public}s", posStr.c_str());
            dlnaManager_->notify(1, posStr);
        }
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "action:%{public}s", keyAction.c_str());
    }

    bool DlnaPlayer_Listener::OnPlayerReady(const std::string &deviceId, int readyFlag) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "deviceid:%{public}s, readyFlag:%{public}d", deviceId.c_str(), readyFlag);

        return true;
    }
} // namespace CastSessionDlna
}     // namespace CastEngineService
}         // namespace CastEngine
} // namespace OHOS