/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: dmr manager
 * Author: 
 * Create: 2023-10-20
 */

#ifndef DMR_MANAGER_H
#define DMR_MANAGER_H

#include <vector>
#include <unistd.h>
#include <pthread.h>
#include "dlna_common.h"
#include "dlna_dmr.h"
#include "dmr_listener.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

struct DlnaRegisterAction{
    DlnaDmrListenerType pszActionName;
    void *DlnaActionFn;
};

class DmrManager{
	public:
		static DmrManager &getInstance();
		DmrManager();
		int Init();
		int DeInit();
		int Create(const char *adapterName, const char *macAdress);
        int CreateExt(const char *adapterName, const char *ipAddr, const char *macAdress);
        int Destroy();

	private:
		unsigned int RegisterListener();
		void SetDescriptionPath(const char *descriptionPath);
		unsigned int SetDmrName(const char *puDmrName);
		unsigned int SetDeviceName(const char *pcNewName);
		DlnaCommon dmrCommon_;
		DlnaDmr dlnaDmr_;
		int g_s32DmrInitState = 1;
};

}
}
}
}
#endif