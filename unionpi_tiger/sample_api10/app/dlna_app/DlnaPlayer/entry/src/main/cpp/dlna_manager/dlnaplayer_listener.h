//
// Created on 2024/8/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef DlnaPlayer_dlnaplayer_listener_H
#define DlnaPlayer_dlnaplayer_listener_H

#include "dlna_manager.h"
#include "i_dlna_listener.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {
    class DlnaPlayer_Listener : public IDlnaListener {
    public:
        explicit DlnaPlayer_Listener(std::shared_ptr<DlnaManager> manager);
        bool OnPlayerReady(const std::string &deviceId, int readyFlag) override;
        void NotifyEventChange(int moduleId, int event, const std::string &param) override;
        bool NotifyEvent(int event) override;
    
    private:
        std::shared_ptr<DlnaManager> dlnaManager_;
        std::mutex jsonMutex_;
    };
} // namespace CastSessionDlna
}     // namespace CastEngineService
}         // namespace CastEngine
} // namespace OHOS
#endif //DlnaPlayer_dlnaplayer_listener_H
