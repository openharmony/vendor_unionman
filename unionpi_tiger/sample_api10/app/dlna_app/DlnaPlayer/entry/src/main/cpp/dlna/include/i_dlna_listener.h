/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: dlna listener External interfaces
 * Author: 
 * Create: 2023-10-20
 */

#ifndef I_DLNA_LISTENER_H
#define I_DLNA_LISTENER_H

#include <iostream>

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

constexpr int ERROR_CODE_DEFAULT = 0;

class IDlnaListener {
public:
    virtual ~IDlnaListener() {}
	virtual bool OnPlayerReady(const std::string &deviceId, int readyFlag) = 0;
    virtual void NotifyEventChange(int moduleId, int event, const std::string &param) = 0;
    virtual bool NotifyEvent(int event) = 0;
};
} // namespace CastSessionDlna
} // namespace CastEngineService
} // namespace CastEngine
} // namespace OHOS

#endif // I_DLNA_LISTENER_H