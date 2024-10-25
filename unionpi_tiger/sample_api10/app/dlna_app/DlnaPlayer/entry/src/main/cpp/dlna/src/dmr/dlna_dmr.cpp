/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: dlna dmr
 * Author: 
 * Create: 2023-10-20
 */

//#include "dlna_log.h"
#include "upnptools.h"
#include <hilog/log.h>
#include "upnpapi.h"
#include "ixmlmembuf.h"
#include "dlna_common.h"
#include "dlna_buffer_manager.h"
#include "dmr_device_desc.h"
#include "securec.h"
#include "dlna_dmr.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

//DEFINE_DLNA_LABEL("Dlna_dmr-dmr");
#define DLNA_LOG_TAG "Dlna_dmr-dmr"

    char dmrDescriptionFilePath_[DESCRIPTION_PATH_LENGTH];

    DlnaDmr &DlnaDmr::getInstance() {
        static DlnaDmr manager{};
        return manager;
    }

    unsigned int DlnaDmr::RegisterControlListener(DlnaDmrListenerType enDmrListenerType, const void *pvListenerFunc) {
        if (pvListenerFunc == DLNA_NULL_PTR) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,
                         "[CastEngineDlna][%{public}s][:%{public}d]-:NULL Callback function pointer is \r\n",
                         __FUNCTION__, __LINE__);
            return DLNA_RET_FAILURE;
        }

        /* since there is nothing suspecious here */
        switch (enDmrListenerType) {
        case DlnaDmrListenerType::DMR_LISTENER_SET_MEDIA_URI:
            DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtSetMediaUri = (pFnaDmrAvTransportSetMediaUri)pvListenerFunc;
            break;
        case DlnaDmrListenerType::DMR_LISTENER_PLAY:
            DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtPlay = (pFnDmrAvTransportPlay)pvListenerFunc;
            break;

        case DlnaDmrListenerType::DMR_LISTENER_PAUSE:
            DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtPause = (pFnDmrAvTransportPause)pvListenerFunc;
            break;

        case DlnaDmrListenerType::DMR_LISTENER_SEEK:
            DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtSeek = (pFnDmrAvTransportSeek)pvListenerFunc;
            break;

        case DlnaDmrListenerType::DMR_LISTENER_STOP:
            DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtStop = (pFnDmrAvTransportStop)pvListenerFunc;
            break;
		default:
//            OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Cannot Register for Invalid Listener type:%{public}d\n", __FUNCTION__, __LINE__, enDmrListenerType);
            return DLNA_RET_FAILURE;
    }
    return DLNA_RET_SUCCESS;
}

unsigned int DlnaDmr::RegisterListener(DlnaDmrListenerType enDmrListenerType, const void *pvListenerFunc)
{
    if (pvListenerFunc == DLNA_NULL_PTR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:NULL Callback function pointer is \r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    /* since there is nothing suspecious here */
    if (RegisterControlListener(enDmrListenerType, pvListenerFunc) == DLNA_RET_SUCCESS) {
        return DLNA_RET_SUCCESS;
    }
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "enDmrListenerType = %{public}d", enDmrListenerType);
    switch (enDmrListenerType) {
        case DlnaDmrListenerType::DMR_LISTENER_CURTIME:
            DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtGetCurrtime = (pFnDmrAvTransportGetCurrtime)pvListenerFunc;
            break;
        case DlnaDmrListenerType::DMR_LISTENER_ALLTIME:
            DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtGetAlltime = (pFnDmrAvTransportGetAlltime)pvListenerFunc;
            break;
        case DlnaDmrListenerType::DMR_LISTENER_SETVOL:
            DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtSetVol = (pFnDmrAvTransportSetVol)pvListenerFunc;
            break;
        case DlnaDmrListenerType::DMR_LISTENER_GETVOL:
            DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtGetVol = (pFnDmrAvTransportGetVol)pvListenerFunc;
            break;
        case DlnaDmrListenerType::DMR_LISTENER_SETMUTE:
            DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtGetSetMute = (pFnDmrAvTransportSetMute)pvListenerFunc;
            break;
        default:
            OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Cannot Register for Invalid Listener type:%{public}d\n", __FUNCTION__, __LINE__, enDmrListenerType);
            return DLNA_RET_FAILURE;
    }

    return DLNA_RET_SUCCESS;
}


void DlnaDmr::DeInit(void)
{
    HandleDataFree();
    /* Set the ulDeviceInit = NULL; */
    DlnaCommon::getInstance().GetDmrHdl()->ulDeviceInit = DLNA_FALSE;

    if (DlnaCommon::getInstance().GetDmrHdl()->iAvtTmThdId != -1) {
        ThreadPoolJob jobAvt = { 0 };
        (void)TimerThreadRemove(&gTimerThread, DlnaCommon::getInstance().GetDmrHdl()->iAvtTmThdId, &jobAvt);
        DlnaCommon::getInstance().GetDmrHdl()->iAvtTmThdId = -1;
    }

    if (DlnaCommon::getInstance().GetDmrHdl()->iRcsTmThdId != -1) {
        ThreadPoolJob jobRcs = { 0 };
        (void)TimerThreadRemove(&gTimerThread, DlnaCommon::getInstance().GetDmrHdl()->iRcsTmThdId, &jobRcs);
        DlnaCommon::getInstance().GetDmrHdl()->iRcsTmThdId = -1;
    }

    ithread_mutex_destroy(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));

    (void)UpnpUnRegisterRootDevice(DlnaCommon::getInstance().GetDmrHdl()->dev);
    DlnaCommon::getInstance().GetDmrHdl()->dev = 0;

    DLNA_FREE(DlnaCommon::getInstance().GetDmrHdl()->pszCurrentTransportActions);
    DLNA_FREE(DlnaCommon::getInstance().GetDmrHdl()->currentduration);

    DlnaCommon::getInstance().GetDmrHdl()->setURIactionDone = DLNA_FALSE;
    DlnaCommon::getInstance().GetDmrHdl()->stopActionDone = DLNA_FALSE;

    return;
}

int DlnaDmr::SubConnectionManagerServiceServiceId(UpnpSubscriptionRequest *event)
{
    IXML_Document *PropSet = DLNA_NULL;
    int retVal;
    char *pcProtInfo = DLNA_NULL;
    void *pvNotifyReq = DLNA_NULL;
    char* pSID = NULL;
    char* pUDN = NULL;
    pcProtInfo = DlnaCommon::getInstance().ConnectionManagerServiceCollectProtoInfo(DlnaLocalDeviceType::DLNA_LOCAL_DEVICE_TYPE_DMR);
    /* As dmrMimeStr_ is static, this will never be null */
    retVal = UpnpAddToPropertySet(&PropSet, "SourceProtocolInfo", "");
    if (retVal != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Call UpnpAddToPropertySet failed. "
            "Return value = %{public}d \r\n",
            __FUNCTION__, __LINE__, retVal);
        return DLNA_RET_FAILURE;
    }

    retVal = UpnpAddToPropertySet(&PropSet, "CurrentConnectionIDs", "0");
    if (retVal != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Call UpnpAddToPropertySet failed. "
            "Return value = %{public}d \r\n",
            __FUNCTION__, __LINE__, retVal);
        ixmlDocument_free(PropSet);
        return DLNA_RET_FAILURE;
    }

    retVal = UpnpAddToPropertySet(&PropSet, "SinkProtocolInfo", pcProtInfo);
    if (retVal != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Call UpnpAddToPropertySet failed. "
            "Return value = %{public}d \r\n",
            __FUNCTION__, __LINE__, retVal);
        ixmlDocument_free(PropSet);
        return DLNA_RET_FAILURE;
    }

    pvNotifyReq = (void *)PropSet;
    pUDN = (char*)UpnpString_get_String(UpnpSubscriptionRequest_get_UDN(event));
    pSID = (char*)UpnpString_get_String(UpnpSubscriptionRequest_get_SID(event));
    const char *serviceID = UpnpString_get_String(UpnpSubscriptionRequest_get_ServiceId(event));
    UpnpDevice_Handle Hnd = DlnaCommon::getInstance().GetDmrHdl()->dev;
    //OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d] Call UpnpAcceptSubscriptionExt s "
    //    "Hnd=%{public}d pUDN=[%{public}s] serviceID=[%{public}s] len=%{public}d pSID=[%{public}s]",
    //    __FUNCTION__, __LINE__, Hnd, pUDN, serviceID, strlen(pSID), pSID );
    retVal = UpnpAcceptSubscriptionExt(Hnd, pUDN, serviceID,
        (IXML_Document *)pvNotifyReq, pSID);
    ixmlDocument_free(PropSet);
    if (retVal != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Call UpnpAcceptSubscriptionExt failed."
            " Return value = %{public}d\r\n",
            __FUNCTION__, __LINE__, retVal);
        return DLNA_RET_FAILURE;
    }
    return UPNP_E_SUCCESS;
}

int DlnaDmr::SubRenderingControlServiceId(UpnpSubscriptionRequest *event)
{
    IXML_Document *PropSet = DLNA_NULL;
    int retVal;
    char *respText = DLNA_NULL;
    void *pvNotifyReq = DLNA_NULL_PTR;
    char* pSID = NULL;
    char* pUDN = NULL;
    retVal = DmrRenderingControl::CreateNotifyResponse(DLNA_TRUE, &respText);
    if (retVal != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Failed to create response for Rendering Control Service "
            "Subscription request. Return value = %{public}d\r\n",
            __FUNCTION__, __LINE__, retVal);
        return DLNA_RET_FAILURE;
    }
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d] rcsSubVariable_=[%{public}s] respText=[%{public}s]",
        __FUNCTION__, __LINE__, rcsSubVariable_, respText );
    retVal = UpnpAddToPropertySet(&PropSet, rcsSubVariable_, respText);
    DLNA_FREE(respText);
    if (retVal != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Failed to add property set."
            " Return value = %{public}d\r\n",
            __FUNCTION__, __LINE__, retVal);
        return DLNA_RET_FAILURE;
    }

    pvNotifyReq = (void *)PropSet;
    pUDN = (char*)UpnpString_get_String(UpnpSubscriptionRequest_get_UDN(event));
    pSID = (char*)UpnpString_get_String(UpnpSubscriptionRequest_get_SID(event));
    const char *serviceID = UpnpString_get_String(UpnpSubscriptionRequest_get_ServiceId(event));
    UpnpDevice_Handle Hnd = DlnaCommon::getInstance().GetDmrHdl()->dev;
    //OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d] Call UpnpAcceptSubscriptionExt s "
    //    "Hnd=%{public}d pUDN=[%{public}s] serviceID=[%{public}s] len=%{public}d pSID=[%{public}s]",
    //    __FUNCTION__, __LINE__, Hnd, pUDN, serviceID, strlen(pSID), pSID );
    retVal = UpnpAcceptSubscriptionExt(Hnd, pUDN, serviceID,
        (IXML_Document *)pvNotifyReq, pSID);
    ixmlDocument_free(PropSet);
    if (retVal != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Call UpnpAcceptSubscriptionExt fail."
            " Return value = %{public}d\r\n",
            __FUNCTION__, __LINE__, retVal);
        return DLNA_RET_FAILURE;
    }
    return UPNP_E_SUCCESS;
}

int DlnaDmr::HandleSubRequest(UpnpSubscriptionRequest *event)
{
    int retVal;
    void *pvNotifyReq = DLNA_NULL_PTR;
    IXML_Document *pDocNode = DLNA_NULL;
    Upnp_SID SubsId;
    char* pSID = NULL;
    char* pUDN = NULL;

    if (event == DLNA_NULL_PTR ||
        UpnpSubscriptionRequest_get_ServiceId(event) == NULL ||
        UpnpString_get_String(UpnpSubscriptionRequest_get_UDN(event)) == NULL) {
        return DLNA_RET_FAILURE;
    }

    if (strcmp(UpnpString_get_String(UpnpSubscriptionRequest_get_UDN(event)), DlnaCommon::getInstance().GetDmrHdl()->udn) != DLNA_NULL) {
        return UPNP_E_SUCCESS;
    }

    const char *serviceID = UpnpString_get_String(UpnpSubscriptionRequest_get_ServiceId(event));
    if (strcmp(serviceID, DMR_CONNECTION_MANAGER_SERVICE_ID) == DLNA_NULL) {
        if (SubConnectionManagerServiceServiceId(event) != UPNP_E_SUCCESS) {
            return DLNA_RET_FAILURE;
        }
    } else if (strcmp(serviceID, DMR_RENDERING_CONTROL_SERVICE_ID) == DLNA_NULL) {
        if (SubRenderingControlServiceId(event) != UPNP_E_SUCCESS) {
            return DLNA_RET_FAILURE;
        }
    } else if (strcmp(serviceID, DMR_AV_TRANSPORT_SERVICE_ID) == DLNA_NULL) {
        void *pstEventBuff = NULL;

        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: DMR:Subscription AVTransport "
            "service.\r\n",
            __FUNCTION__, __LINE__);

        pstEventBuff = DlnaBufferManager::AllocStrBuff();
        if (pstEventBuff == DLNA_NULL_PTR) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: AllocStrBuff Failed\r\n ", __FUNCTION__, __LINE__);
            return DLNA_RET_FAILURE;
        }

        /*
            First Check whether the eventing thread is already
            running or not.
        */
        DmrAVTransport::getInstance().CreateAvtNotifyResponse(DLNA_TRUE, pstEventBuff);

        pvNotifyReq = (void *)DlnaBufferManager::StrBuffGetBuff(pstEventBuff);
        pDocNode = ixmlParseBuffer((const char *)pvNotifyReq);
        if (pDocNode == NULL){
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:pDocNode is NULL!",
                __FUNCTION__, __LINE__);
        }
        pUDN = (char*)UpnpString_get_String(UpnpSubscriptionRequest_get_UDN(event));
        pSID = (char*)UpnpString_get_String(UpnpSubscriptionRequest_get_SID(event));
        UpnpDevice_Handle Hnd = DlnaCommon::getInstance().GetDmrHdl()->dev;
        //OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d] pvNotifyReq=%{public}s",
        //    __FUNCTION__, __LINE__, pvNotifyReq);
        //OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d] Call UpnpAcceptSubscriptionExt s "
        //    "Hnd=%{public}d pUDN=[%{public}s] serviceID=[%{public}s] len=%{public}d pSID=[%{public}s]",
        //    __FUNCTION__, __LINE__, Hnd, pUDN, serviceID, strlen(pSID), pSID );
        //android::CallStack stack("dump-test");
        memset(&SubsId, 0, sizeof(Upnp_SID));
        memcpy(&SubsId, pSID, strlen(pSID));
        retVal = UpnpAcceptSubscriptionExt(Hnd,
            pUDN, serviceID, pDocNode, SubsId);
        //OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d] Call UpnpAcceptSubscriptionExt e serviceID=%{public}s",
        //    __FUNCTION__, __LINE__, serviceID);
        DlnaBufferManager::FreeStrBuff(pstEventBuff);
        if (retVal != UPNP_E_SUCCESS) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Call UpnpAcceptSubscriptionExt failed."
                " Return value = %{public}d\r\n",
                __FUNCTION__, __LINE__, retVal);
            return DLNA_RET_FAILURE;
        }
    }

    return UPNP_E_SUCCESS;
}

void DlnaDmr::HandleActionRequestError(UpnpActionRequest *request, char *pServiceType)
{
    UpnpActionRequest_set_ActionResult(request, NULL);
    UpnpActionRequest_set_ErrCode(request, DLNA_E_UPNP_401);
    char *errStr = (char *)UpnpString_get_String(UpnpActionRequest_get_ErrStr(request));
    size_t errStrLen = UpnpString_get_Length(UpnpActionRequest_get_ErrStr(request));
    if (memset_s(errStr, errStrLen, 0, errStrLen) != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:memset_s failed.\r\n", __FUNCTION__, __LINE__);
        return;
    }
    if (pServiceType != DLNA_NULL_PTR) {
        if (strncpy_s(errStr, errStrLen, DLNA_E_UPNP_401_STRING,
            errStrLen - 1) != EOK) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:strncpy_s failed.\r\n", __FUNCTION__, __LINE__);
            return;
        }
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Could not find the Action %{public}s\r\n", __FUNCTION__, __LINE__,
            UpnpString_get_String(UpnpActionRequest_get_ActionName(request)));
        return;
    }
    if (strncpy_s(errStr, errStrLen, "Unknown Service ID",
        strlen("Unknown Service ID")) != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:strncpy_s failed.\r\n", __FUNCTION__, __LINE__);
        return;
    }
    return;
}

void DlnaDmr::SetRequestError(UpnpActionRequest *request, DmrActionParam stActionParam)
{
    if (request == DLNA_NULL_PTR) {
        return;
    }

    char *errStr = (char *)UpnpString_get_String(UpnpActionRequest_get_ErrStr(request));
    /* Incase the Error is Not Set by the Called Function , return 401 always */
    if (stActionParam.iErrorCode == UPNP_E_SUCCESS) {
        UpnpActionRequest_set_ErrCode(request, DLNA_E_UPNP_401);
        errStr = DLNA_E_UPNP_401_STRING;
        return;
    }
    UpnpActionRequest_set_ErrCode(request, stActionParam.iErrorCode);
    if (stActionParam.pErrorStr != DLNA_NULL_PTR) {
        errStr = stActionParam.pErrorStr;
    }
}

/* This function will be invoked to handle all the S */
void DlnaDmr::HandleActionRequest(UpnpActionRequest *request)
{
    const char *devUDN = NULL;
    const char *serviceID = NULL;
    const char *actionName = NULL;
    char *pServiceType = DLNA_NULL;
    DmrServiceAction *pstAction = DLNA_NULL;
    DmrActionParam stActionParam = { 0 };

    //2022.08.02 add s
    devUDN = UpnpString_get_String(UpnpActionRequest_get_DevUDN(request));
    serviceID = UpnpString_get_String(
        UpnpActionRequest_get_ServiceID(request));
    actionName = UpnpString_get_String(
        UpnpActionRequest_get_ActionName(request));
    OH_LOG_Print(LOG_APP, LOG_DEBUG, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:devUDN=[%{public}s] serviceID=[%{public}s] actionName=[%{public}s]\r\n",
        __FUNCTION__, __LINE__, devUDN, serviceID, actionName );

    UpnpActionRequest_set_ErrCode(request, 0);
    UpnpActionRequest_set_ActionResult(request, NULL);

    if (request == DLNA_NULL) {
        return;
    }

    if (DlnaCommon::getInstance().FindServiceAndAction(request, dmrServiceTable_, &pServiceType, &pstAction) == DLNA_RET_SUCCESS) {
        stActionParam.pstActionRequest = request;
        stActionParam.pcServiceType = pServiceType;
        stActionParam.iErrorCode = UPNP_E_SUCCESS;

        stActionParam.pErrorStr = DLNA_NULL;
        if (pstAction == DLNA_NULL) {
            return;
        }

        /* process this action request success */
        if (pstAction->DlnaActionFn(&stActionParam) == DLNA_RET_SUCCESS) {
            UpnpActionRequest_set_ErrCode(request, UPNP_E_SUCCESS);
            return;
        }
        if (UpnpActionRequest_get_ActionResult(request) != NULL) {
            ixmlDocument_free(UpnpActionRequest_get_ActionResult(request));
            UpnpActionRequest_set_ActionResult(request, NULL);
        }
        SetRequestError(request, stActionParam);
        return;
    }
    HandleActionRequestError(request, pServiceType);
}

int HandleCallback(Upnp_EventType eventType, const void *event, void *cookie)
{
    if (cookie != NULL) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:DMR received UPNP event \r\n", __FUNCTION__, __LINE__);
    }

    if (DlnaCommon::getInstance().GetDmrHdl()->ulDeviceInit != DLNA_TRUE) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:DMR Not Init !! \r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_SUCCESS;
    }

    switch (eventType) {
        case UPNP_EVENT_SUBSCRIPTION_REQUEST:
            DlnaDmr::getInstance().HandleSubRequest((UpnpSubscriptionRequest *)event);
            break;

        case UPNP_CONTROL_ACTION_REQUEST:
            DlnaDmr::getInstance().HandleActionRequest((UpnpActionRequest *)event);
            break;
        default:
            break;
    }

    return (0);
}

void DlnaDmr::HandleDataFree(void)
{
    DLNA_FREE(DlnaCommon::getInstance().GetDmrHdl()->udn);
    DLNA_FREE(DlnaCommon::getInstance().GetDmrHdl()->pszPlayURL);
    DLNA_FREE(DlnaCommon::getInstance().GetDmrHdl()->pszTrackMetaData);

    return;
}

#define DEFAULT_DMR_NAME "OHOSPlayer"
unsigned int DlnaDmr::SetName(const char *pcDmrName)
{
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[%{public}s][:%{public}d]enter", __FUNCTION__, __LINE__);
    /* Check the state of stack */
    if (DlnaCommon::getInstance().GetState() == DlnaStateType::DLNA_STATE_ACTIVE) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[%{public}s][:%{public}d]-:Stack is already initialzed, "
            "Invalid Option !!! \r\n",
            __FUNCTION__, __LINE__);
        return DLNA_RET_COMM_INVALID_OPTION;
    }

    if (pcDmrName != NULL) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[%{public}s][%{public}d] resave name.", __FUNCTION__, __LINE__);
        DlnaCommon::getInstance().FreePcDeviceName();
        DlnaCommon::getInstance().SetPcDeviceName((char *)strdup(pcDmrName));
    } else {
        return DLNA_RET_FAILURE;
    }
    return DLNA_RET_SUCCESS;
}

void DlnaDmr::SetDescriptionPath(const char *descriptionPath)
{
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]", __FUNCTION__, __LINE__);
    if (descriptionPath != NULL) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d] Set path :%{public}s", __FUNCTION__, __LINE__, descriptionPath);
        if (strncpy_s(dmrDescriptionFilePath_, sizeof(dmrDescriptionFilePath_), descriptionPath,
            (strlen(descriptionPath) + 1)) != EOK) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:strncpy_s failed.\r\n", __FUNCTION__, __LINE__);
            return;
        }
    } else {
        if (memset_s(dmrDescriptionFilePath_, DESCRIPTION_PATH_LENGTH, 0, DESCRIPTION_PATH_LENGTH) != EOK) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:memset_s failed.\r\n", __FUNCTION__, __LINE__);
            return;
        }
    }
}

void DlnaDmr::HdInit()
{
    DLNA_LOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
    DlnaCommon::getInstance().GetDmrHdl()->ulDeviceInit = DLNA_TRUE;
    DlnaCommon::getInstance().GetDmrHdl()->dmrState = static_cast<int>(DmrAvState::DLNA_DMR_STATE_NO_MEDIA_PRESENT);

    DlnaCommon::getInstance().GetDmrHdl()->iAvtTmThdId = -1;
    DlnaCommon::getInstance().GetDmrHdl()->iRcsTmThdId = -1;
    DlnaCommon::getInstance().GetDmrHdl()->bSendQuickNotify = DLNA_TRUE;
    DlnaCommon::getInstance().GetDmrHdl()->bRCSSendQuickNotify = DLNA_TRUE;
    DlnaCommon::getInstance().GetDmrHdl()->pszCurrentTransportActions = (char *)malloc(DLNA_BUFF_SIZE);
    if (DlnaCommon::getInstance().GetDmrHdl()->pszCurrentTransportActions == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:malloc faile", __FUNCTION__, __LINE__);
        DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
        return;
    }
    if (memset_s(DlnaCommon::getInstance().GetDmrHdl()->pszCurrentTransportActions, DLNA_BUFF_SIZE, 0, DLNA_BUFF_SIZE) != EOK) {
        DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
        return;
    }
    DlnaCommon::getInstance().GetDmrHdl()->currentduration = (char *)malloc(DLNA_BUFF_SIZE);
    if (DlnaCommon::getInstance().GetDmrHdl()->currentduration == NULL) {
        free(DlnaCommon::getInstance().GetDmrHdl()->pszCurrentTransportActions);
        DlnaCommon::getInstance().GetDmrHdl()->pszCurrentTransportActions = NULL;
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:malloc faile", __FUNCTION__, __LINE__);
        DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
        return;
    }
    if (memset_s(DlnaCommon::getInstance().GetDmrHdl()->currentduration, DLNA_BUFF_SIZE, 0, DLNA_BUFF_SIZE) != EOK) {
        DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
        return;
    }
    if (strncpy_s(DlnaCommon::getInstance().GetDmrHdl()->currentduration, DLNA_BUFF_SIZE, "00:00:00", (strlen("00:00:00") + 1)) != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:strncpy_s failed.\r\n", __FUNCTION__, __LINE__);
        DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
        free(DlnaCommon::getInstance().GetDmrHdl()->currentduration);
        DlnaCommon::getInstance().GetDmrHdl()->currentduration = NULL;
        return;
    }
    DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
}

int DlnaDmr::GetDefaultDes(char **description, const char *name)
{
    int len;
    len = strlen(DMR_DESCRIPTION) + strlen(name) + 1;
    *description = (char *)malloc(len * sizeof(char));
    if (*description == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "%{public}s,%{public}d,malloc failed when init dmr\n", __FUNCTION__, __LINE__);
        return UPNP_E_OUTOF_MEMORY;
    }
    if (memset_s(*description, len, 0, len) != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "%{public}s,%{public}d,memset_s failed\n", __FUNCTION__, __LINE__);
        DLNA_FREE(*description);
        return UPNP_E_OUTOF_MEMORY;
    }
    if (snprintf_s(*description, len, len - 1, DMR_DESCRIPTION, name) < EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "%{public}s,%{public}d,snprintf_s failed\n", __FUNCTION__, __LINE__);
        DLNA_FREE(*description);
        return UPNP_E_OUTOF_MEMORY;
    }
    return UPNP_E_SUCCESS;
}

int DlnaDmr::Init(const char *pcMacAddress)
{
    int iRet;
    char *pszDmrDescription = DLNA_NULL;
    char *description = NULL;
    const char *name = DEFAULT_DMR_NAME;
    ithread_mutex_init(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex), NULL);
    IXML_Document *dlnadmrDeviceDesc = NULL;
	
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:s mac=%{public}s\n", __FUNCTION__, __LINE__, pcMacAddress);
    if (1){//(strlen(dmrDescriptionFilePath_) > 0) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]before ixmlLoadDocument from: %{public}s", __FUNCTION__, __LINE__,
            dmrDescriptionFilePath_);
        //dlnadmrDeviceDesc = ixmlLoadDocument(dmrDescriptionFilePath_);
        ixmlParseBufferEx(DEV_DESC, &dlnadmrDeviceDesc);
        if (dlnadmrDeviceDesc == NULL) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][%{public}d]ixmlLoadDocument error! can not load file", __FUNCTION__, __LINE__);
            return UPNP_E_INVALID_URL;
        } else {
            DlnaCommon::getInstance().ChangeDescriptionFriendlyName(dlnadmrDeviceDesc, name);
            description = ixmlDocumenttoString(dlnadmrDeviceDesc);
            ixmlDocument_free(dlnadmrDeviceDesc);
        }
    } else {
        if (GetDefaultDes(&description, name) != UPNP_E_SUCCESS) {
            return UPNP_E_OUTOF_MEMORY;
        }
    }
    ChangeDeviceDes changeDeviceDes = {description, &(DlnaCommon::getInstance().GetDmrHdl()->udn), &pszDmrDescription, "1111",
        DlnaLocalDeviceType::DLNA_LOCAL_DEVICE_TYPE_DMR, pcMacAddress};
    iRet = (int)DlnaCommon::getInstance().ChangeDeviceDescription(changeDeviceDes);
    if (iRet != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Changing Device Description  fail."
            " Return value = %{public}d\r\n", __FUNCTION__, __LINE__, iRet);

        free(description);
        return iRet;
    }
	
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Call UpnpRegisterRootDevice2\n"
        " pszDmrDescription = %{public}s\n", __FUNCTION__, __LINE__, pszDmrDescription);
    iRet = UpnpRegisterRootDevice2(UPNPREG_BUF_DESC, pszDmrDescription, strlen(pszDmrDescription), 1,
        HandleCallback, &DlnaCommon::getInstance().GetDmrHdl()->dev, &DlnaCommon::getInstance().GetDmrHdl()->dev);
    OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "%{public}s, %{public}d, DlnaCommon::getInstance().GetDmrHdl()->dev:%{public}d", __FUNCTION__, __LINE__, DlnaCommon::getInstance().GetDmrHdl()->dev);
    if (iRet != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Call UpnpRegisterRootDevice fail."
            " Return value = %{public}d\r\n", __FUNCTION__, __LINE__, iRet);
        ixmlFreeDOMString(pszDmrDescription);
        free(description);
        return iRet;
    }

    ixmlFreeDOMString(pszDmrDescription);
    HdInit();
    free(description);

    return UPNP_E_SUCCESS;
}

}
}
}
}