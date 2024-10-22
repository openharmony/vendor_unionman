/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: dlna controller External interfaces
 * Author: 
 * Create: 2023-10-20
 */

#ifndef I_DLNA_CONTROLLER_H
#define I_DLNA_CONTROLLER_H

#include <memory>
#include <iostream>
#include <string>
#include <set>

#include "i_dlna_listener.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

class IDlnaController {
public:
    IDlnaController() = default;
    virtual ~IDlnaController() = default;

    static std::shared_ptr<IDlnaController> GetInstance2();
    static std::shared_ptr<IDlnaController> GetInstance(std::shared_ptr<IDlnaListener> listener);
    static std::shared_ptr<IDlnaController> rf_GetInstance(std::shared_ptr<IDlnaListener> listener);

    virtual bool DmrCreate() = 0;
    virtual bool DmrCreateExt(const char *ipAddr) = 0;
    virtual bool DmrDestroy() = 0;
	virtual bool ProcessRenderReadyRequest() = 0;
    virtual bool SendEventChange(int moduleId, int event, const std::string &param) = 0;
};
} // namespace CastSessionDlna
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // I_DLNA_CONTROLLER_H
