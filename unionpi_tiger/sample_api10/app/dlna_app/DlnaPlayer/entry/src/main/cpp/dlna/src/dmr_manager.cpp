/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: dlna manager
 * Author: 
 * Create: 2023-10-20
 */

//#include "dlna_log.h"
#include <hilog/log.h>
#include "dmr_manager.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

//DEFINE_DLNA_LABEL("Cast-dmr-manager");
#define DLNA_LOG_TAG "Dlna_dmr-manager"

    static pthread_mutex_t g_nativeMutex = PTHREAD_MUTEX_INITIALIZER;

#define NATIVE_LOCK do {                                          \
        (void)pthread_mutex_lock(&g_nativeMutex); \
    } while (0)

#define NATIVE_UNLOCK do {                                            \
        (void)pthread_mutex_unlock(&g_nativeMutex); \
    } while (0)

DmrManager &DmrManager::getInstance()
{
	static DmrManager instance;
	return instance;
}

DmrManager::DmrManager()
{
	//dmrCommon_ = DlnaCommon::getInstance();
    //dlnaDmr_ = DlnaDmr::getInstance();
}

int DmrManager::Init()
{
    if (g_s32DmrInitState) {
        g_s32DmrInitState = 0;
        return 0;
    }
    return 0;
}
int DmrManager::DeInit()
{
    g_s32DmrInitState = 1;
    return 0;
}

int DmrManager::Create(const char *adapterName, const char *macAdress)
{
    NATIVE_LOCK;
    unsigned int rc;

    if (g_s32DmrInitState) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[%{public}s][%{public}d]\n", __FUNCTION__, __LINE__);
        NATIVE_UNLOCK;
        return -1;
    }

    rc = DlnaCommon::getInstance().Create(adapterName, macAdress, DlnaInitModeType::DLNA_INIT_MODE_DMR);
    if (rc != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "DLNA_InitStackFailed\n");
        NATIVE_UNLOCK;
        return -1;
    }

    /* Register the callbacks for DMR */
    rc = RegisterListener();
    if (rc != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "Failed To Register Callbacks For DMR\r\n");
        NATIVE_UNLOCK;
        DlnaCommon::getInstance().Destroy();
        return -1;
    }
    NATIVE_UNLOCK;
    return 0;
}

int DmrManager::CreateExt(const char *adapterName, const char *ipAddr, const char *macAdress) {
    NATIVE_LOCK;
    unsigned int rc;

    if (g_s32DmrInitState) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG, "[%{public}s][%{public}d]\n", __FUNCTION__, __LINE__);
        NATIVE_UNLOCK;
        return -1;
    }

    rc = DlnaCommon::getInstance().CreateExt(adapterName, ipAddr, macAdress, DlnaInitModeType::DLNA_INIT_MODE_DMR);
    if (rc != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG, "DLNA_InitStackFailed\n");
        NATIVE_UNLOCK;
        return -1;
    }

    /* Register the callbacks for DMR */
    rc = RegisterListener();
    if (rc != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG, "Failed To Register Callbacks For DMR\r\n");
        NATIVE_UNLOCK;
        DlnaCommon::getInstance().Destroy();
        return -1;
    }
    NATIVE_UNLOCK;
    return 0;
}

int DmrManager::Destroy()
{
    NATIVE_LOCK;
    g_s32DmrInitState = 1;
    DlnaCommon::getInstance().Destroy();
    NATIVE_UNLOCK;
    return 0;
}

void DmrManager::SetDescriptionPath(const char *descriptionPath)
{
    return DlnaDmr::getInstance().SetDescriptionPath(descriptionPath);
}

unsigned int DmrManager::SetDmrName(const char *puDmrName)
{
    return DlnaDmr::getInstance().SetName(puDmrName);
}

unsigned int DmrManager::SetDeviceName(const char *pcNewName)
{
    return DlnaCommon::getInstance().SetDeviceName((const char *)pcNewName);
}

 DlnaRegisterAction dmrRegisterActionFuncTable[] = {
	{ DlnaDmrListenerType::DMR_LISTENER_SET_MEDIA_URI, reinterpret_cast<void *>(DmrListener::SetMediaUri) },
	{ DlnaDmrListenerType::DMR_LISTENER_PLAY, reinterpret_cast<void *>(DmrListener::Play) },
	{ DlnaDmrListenerType::DMR_LISTENER_PAUSE, reinterpret_cast<void *>(DmrListener::Pause) },
	{ DlnaDmrListenerType::DMR_LISTENER_STOP, reinterpret_cast<void *>(DmrListener::Stop) },
	{ DlnaDmrListenerType::DMR_LISTENER_SEEK, reinterpret_cast<void *>(DmrListener::Seek) },
	{ DlnaDmrListenerType::DMR_LISTENER_CURTIME, reinterpret_cast<void *>(DmrListener::GetCurrTime) },
	{ DlnaDmrListenerType::DMR_LISTENER_ALLTIME, reinterpret_cast<void *>(DmrListener::GetAllTime) },
	{ DlnaDmrListenerType::DMR_LISTENER_SETVOL, reinterpret_cast<void *>(DmrListener::SetVol) },
	{ DlnaDmrListenerType::DMR_LISTENER_GETVOL, reinterpret_cast<void *>(DmrListener::GetVol) },
	{ DlnaDmrListenerType::DMR_LISTENER_SETMUTE, reinterpret_cast<void *>(DmrListener::SetMute) }
};


unsigned int DmrManager::RegisterListener()
{
	unsigned int uiRet;
	int actionFnTblCount = sizeof(dmrRegisterActionFuncTable) / sizeof(DlnaRegisterAction);
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "Register Callbacks For DMR size:%{public}d",
                 actionFnTblCount);
    for(int i = 0;i < actionFnTblCount;i++){
		uiRet = DlnaDmr::getInstance().RegisterListener((DlnaDmrListenerType)dmrRegisterActionFuncTable[i].pszActionName,dmrRegisterActionFuncTable[i].DlnaActionFn);
        
	    if (uiRet != DLNA_RET_SUCCESS) {
	        return DLNA_RET_FAILURE;
	    }
	}
	return uiRet;
}
}
}
}
}