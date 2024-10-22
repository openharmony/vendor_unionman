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
#ifndef DLNA_CONTROLLER_H
#define DLNA_CONTROLLER_H


#include "i_dlna_listener.h"
#include "i_dlna_controller.h"
#include "dmr_manager.h"
namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {
class DlnaController : public IDlnaController{
public:
    DlnaController(std::shared_ptr<IDlnaListener> listener);
    ~DlnaController() override;

    bool DmrCreate() override;
    bool DmrCreateExt(const char *ipAddr) override;
    bool DmrDestroy() override;
	bool SendEventChange(int moduleId, int event, const std::string &param) override;
	bool ProcessRenderReadyRequest() override;

private:
    std::shared_ptr<IDlnaListener> listener_;
	static DmrManager dmrManager_;
	
};
} // namespace CastSessionDlna
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // DLNA_CONTROLLER_H
