/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: dlna controller
 * Author: 
 * Create: 2023-10-20
 */

#include <iterator>
//#include "dlna_log.h"
#include <hilog/log.h>
#include "dmr_listener.h"
#include "dmr_manager.h"
#include "dlna_controller.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {
//DEFINE_DLNA_LABEL("Dlna_dmr-Controller");
#define DLNA_LOG_TAG "Dlna_dmr-Controller"

    DmrManager DlnaController::dmrManager_ = DmrManager::getInstance();

    std::shared_ptr<IDlnaController> IDlnaController::GetInstance(std::shared_ptr<IDlnaListener> listener) {
        return std::static_pointer_cast<IDlnaController>(std::make_shared<DlnaController>(listener));
    }

    DlnaController::DlnaController(std::shared_ptr<IDlnaListener> listener) : listener_(listener) {

        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "constructor set listener");
    }

    DlnaController::~DlnaController() { OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "~RtspController in."); }

    bool DlnaController::ProcessRenderReadyRequest() {
        OH_LOG_Print(LOG_APP, LOG_DEBUG, 0xFF00, DLNA_LOG_TAG, "Process render ready request in.");
        // return listener_->OnPlayerReady("123", 1);
        return true;
    }

    bool DlnaController::DmrCreate() {
        auto DmrListener = DmrListener::getInstance();
        OH_LOG_Print(LOG_APP, LOG_DEBUG, 0xFF00, DLNA_LOG_TAG, "set listener");
        DmrListener.SetListener(listener_);
        dmrManager_.Init();
        dmrManager_.Create("eth0", "12:34:45:56:56:78");
        OH_LOG_Print(LOG_APP, LOG_DEBUG, 0xFF00, DLNA_LOG_TAG, "Out");

        return true;
    }

    bool DlnaController::DmrCreateExt(const char *ipAddr) {
        auto DmrListener = DmrListener::getInstance();
        OH_LOG_Print(LOG_APP, LOG_DEBUG, 0xFF00, DLNA_LOG_TAG, "set listener");
        DmrListener.SetListener(listener_);
        dmrManager_.Init();
        dmrManager_.CreateExt("eth0", ipAddr, "12:34:45:56:56:78");
        OH_LOG_Print(LOG_APP, LOG_DEBUG, 0xFF00, DLNA_LOG_TAG, "Out");

        return true;
    }

    bool DlnaController::DmrDestroy() {
        dmrManager_.Destroy();
        dmrManager_.DeInit();

        return true;
    }

    bool DlnaController::SendEventChange(int moduleId, int event, const std::string &param) {
        auto dmrListener = DmrListener::getInstance();
        dmrListener.SendEventChange(moduleId, event, param);
        return true;
    }

} // namespace CastSessionDlna
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS