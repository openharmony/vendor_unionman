/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: dmr rendering control service
 * Author: 
 * Create: 2023-10-20
 */

//#include "dlna_log.h"
#include "gena.h"
#include "upnptools.h"
#include "upnpapi.h"
#include "TimerThread.h"
#include "dlna_dmr.h"
#include "securec.h"
#include <hilog/log.h>


namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

#define DLNA_LOG_TAG "Dlna_dmr-Rendering-Control"
    // DEFINE_DLNA_LABEL("Cast-dmr-Rendering-Control");


#ifdef __cplusplus
extern "C" {
#endif

char DmrRenderingControl::g_aucDlnaDmrRcsChannel[DMR_RENDER_CONTROL_CHANNEL_NUM] = {'\0'};

DmrRenderingControl &DmrRenderingControl::getInstance(){
    static DmrRenderingControl manager{};
    return manager;
}

int DmrRenderingControl::IsValidChannel(const char *pcChannel)
{
    if ((strcmp(pcChannel, "Master") == 0) || (strcmp(pcChannel, "LFE") == 0) ||
        (strcmp(pcChannel, "LFC") == 0) || (strcmp(pcChannel, "RFC") == 0) || (strcmp(pcChannel, "CF") == 0) ||
        (strcmp(pcChannel, "LF") == 0) || (strcmp(pcChannel, "LS") == 0) || (strcmp(pcChannel, "RF") == 0) ||
        (strcmp(pcChannel, "RS") == 0) || (strcmp(pcChannel, "SD") == 0) || (strcmp(pcChannel, "SL") == 0) ||
        (strcmp(pcChannel, "SR") == 0) || (strcmp(pcChannel, "B") == 0) || (strcmp(pcChannel, "T") == 0)) {
        return DLNA_TRUE;
    }

    return DLNA_FALSE;
}

int DmrRenderingControl::CreateNotifyResponse(int isInitialNotify, char **eventRespText)
{
    char *respText = (char *) DLNA_NULL_PTR;
    int retval;
    int ixmlretval;
    IXML_Element *eleInstanceId = (IXML_Element *) DLNA_NULL_PTR;
    IXML_Document *pEventDoc = (IXML_Document *) DLNA_NULL_PTR;
    IXML_Node *node = (IXML_Node *) DLNA_NULL_PTR;

    respText = strdup(DMR_RENDER_CONTROL_EVENT_ATTRIBUTE);
    if (respText == DLNA_NULL_PTR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Allocation of the RspText Failed!!!\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    retval = ixmlParseBufferEx(respText, &pEventDoc);
    DLNA_FREE(respText);

    if (retval != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Parsing of XMLDoc Failed with error"
            " code-:%{public}d\r\n", __FUNCTION__, __LINE__, retval);
        return DLNA_RET_FAILURE;
    }

    node = ixmlNode_getFirstChild((IXML_Node *)pEventDoc);

    eleInstanceId = ixmlDocument_createElement(pEventDoc, "InstanceID");

    ixmlretval = ixmlElement_setAttribute(eleInstanceId, "val", "0");
    if (ixmlretval != IXML_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Setting attribute of InstanceID failed "
            "with error code-:%{public}d\r\n", __FUNCTION__, __LINE__, retval);
        ixmlDocument_free(pEventDoc);
        ixmlElement_free(eleInstanceId);
        return DLNA_RET_FAILURE;
    }

    retval = CreateOptNotifyContents(isInitialNotify, static_cast<void *>(pEventDoc), static_cast<void *>(eleInstanceId));
    if (retval != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Failed to add optional contents "
            "to the Rendering Control Service notify-:%{public}d\r\n", __FUNCTION__, __LINE__, retval);
        ixmlDocument_free(pEventDoc);
        ixmlElement_free(eleInstanceId);
        return DLNA_RET_FAILURE;
    }

    (void)ixmlNode_appendChild(node, (IXML_Node *)eleInstanceId);

    /* Create Event Attribute , with Instance ID, and lastchange state
        Variables
        [END] */
    *eventRespText = ixmlPrintDocument(pEventDoc);

    ixmlDocument_free(pEventDoc);

    if (*eventRespText == DLNA_NULL_PTR) {
        return DLNA_RET_FAILURE;
    }

    return DLNA_RET_SUCCESS;
}

unsigned int DmrRenderingControl::CreateMuteContents(int isInitialNotify, void *pEventDoc, void *eleInstanceId)
{
    IXML_Element *eleStateVariable = NULL;
    IXML_Document *eventDoc = static_cast<IXML_Document *>(pEventDoc);
    IXML_Node *eleInstanceNode = static_cast<IXML_Node *>(eleInstanceId);
    int retval;
    if ((isInitialNotify == DLNA_TRUE) || (DMR_RENDER_CONTROL_IS_MUTE_TO_NOTIFY(DlnaCommon::getInstance().GetDmrHdl()->ulRcsEventNotifySet))) {
        eleStateVariable = ixmlDocument_createElement(eventDoc, "Mute");

        retval = ixmlElement_setAttribute(eleStateVariable, "channel", g_aucDlnaDmrRcsChannel);
        if (retval != IXML_SUCCESS) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Setting attribute of InstanceID failed "
                "with error code-:%{public}d\r\n",
                __FUNCTION__, __LINE__, retval);
            ixmlElement_free(eleStateVariable);
            return DLNA_RET_FAILURE;
        }

        if (DlnaCommon::getInstance().GetDmrHdl()->bMuteStateVar) {
            retval = ixmlElement_setAttribute(eleStateVariable, "val", "1");
        } else {
            retval = ixmlElement_setAttribute(eleStateVariable, "val", "0");
        }

        if (retval != IXML_SUCCESS) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Setting attribute of InstanceID failed "
                "with error code-:%{public}d\r\n",
                __FUNCTION__, __LINE__, retval);
            ixmlElement_free(eleStateVariable);
            return DLNA_RET_FAILURE;
        }

        (void)ixmlNode_appendChild(eleInstanceNode, (IXML_Node *)eleStateVariable);
    }
    return DLNA_RET_SUCCESS;
}

unsigned int DmrRenderingControl::CreateVolumeContents(int isInitialNotify, void *pEventDoc, void *eleInstanceId)
{
    IXML_Element *eleStateVariable = NULL;
    IXML_Document *eventDoc = static_cast<IXML_Document *>(pEventDoc);
    IXML_Node *eleInstanceNode = static_cast<IXML_Node *>(eleInstanceId);
    int retval;
    if ((isInitialNotify == DLNA_TRUE) || (DMR_RENDER_CONTROL_IS_VOLUME_TO_NOTIFY(DlnaCommon::getInstance().GetDmrHdl()->ulRcsEventNotifySet))) {
        char aVolumeVal[UINT_RANGE];

        eleStateVariable = ixmlDocument_createElement(eventDoc, "Volume");

        retval = ixmlElement_setAttribute(eleStateVariable, "channel", g_aucDlnaDmrRcsChannel);
        if (retval != IXML_SUCCESS) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Setting attribute of Volume failed "
                "with error code-:%{public}d\r\n",
                __FUNCTION__, __LINE__, retval);
            ixmlElement_free(eleStateVariable);
            return DLNA_RET_FAILURE;
        }

        if (memset_s(aVolumeVal, sizeof(aVolumeVal), 0, sizeof(aVolumeVal)) != EOK) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:memset_s failed\r\n", __FUNCTION__, __LINE__);
            ixmlElement_free(eleStateVariable);
            return DLNA_RET_FAILURE;
        }
        if (snprintf_s(aVolumeVal, UINT_RANGE, UINT_RANGE - 1, "%{public}d", DlnaCommon::getInstance().GetDmrHdl()->ulVolumeStateVar) < EOK) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:snprintf_s failed\r\n", __FUNCTION__, __LINE__);
            ixmlElement_free(eleStateVariable);
            return DLNA_RET_FAILURE;
        }

        retval = ixmlElement_setAttribute(eleStateVariable, "val", aVolumeVal);
        if (retval != IXML_SUCCESS) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Setting attribute of Volume failed "
                "with error code-:%{public}d\r\n",
                __FUNCTION__, __LINE__, retval);
            ixmlElement_free(eleStateVariable);
            return DLNA_RET_FAILURE;
        }

        (void)ixmlNode_appendChild(eleInstanceNode, (IXML_Node *)eleStateVariable);
    }
    return DLNA_RET_SUCCESS;
}

int DmrRenderingControl::CreateOptNotifyContents(int isInitialNotify, void *pEventDoc, void *eleInstanceId)
{
    IXML_Element *eleStateVariable = NULL;
    IXML_Document *eventDoc = static_cast<IXML_Document *>(pEventDoc);
    IXML_Node *eleInstanceNode = static_cast<IXML_Node *>(eleInstanceId);
    int retval;

    /* Mute */
    if (CreateMuteContents(isInitialNotify, pEventDoc, eleInstanceId) != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    /* Volume  */
    if (CreateVolumeContents(isInitialNotify, pEventDoc, eleInstanceId) != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    if (isInitialNotify == DLNA_TRUE) {
        eleStateVariable = ixmlDocument_createElement(eventDoc, "PresetNameList");

        retval = ixmlElement_setAttribute(eleStateVariable, "val", "FactoryDefaults,InstallationDefaults");
        if (retval != IXML_SUCCESS) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Setting attribute of  PresetNameList failed "
                "with error code-:%{public}d\r\n",
                __FUNCTION__, __LINE__, retval);
            ixmlElement_free(eleStateVariable);
            return DLNA_RET_FAILURE;
        }

        (void)ixmlNode_appendChild(eleInstanceNode, (IXML_Node *)eleStateVariable);
    }

    /* PresetName */
    if ((isInitialNotify == DLNA_TRUE) || (DMR_RENDER_CONTROL_IS_PRESETNAME_TO_NOTIFY(DlnaCommon::getInstance().GetDmrHdl()->ulRcsEventNotifySet))) {
        eleStateVariable = ixmlDocument_createElement(eventDoc, "A_ARG_TYPE_PresetName");

        if (DlnaCommon::getInstance().GetDmrHdl()->ulPresetNameStateVar == DMR_RENDER_CONTROL_PRESETNAME_FACTORY_DEFAULTS) {
            retval = ixmlElement_setAttribute(eleStateVariable, "val", "FactoryDefaults");
        } else {
            retval = ixmlElement_setAttribute(eleStateVariable, "val", "InstallationDefaults");
        }

        if (retval != IXML_SUCCESS) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Setting attribute of A_ARG_TYPE_PresetName failed "
                "with error code-:%{public}d\r\n",
                __FUNCTION__, __LINE__, retval);
            ixmlElement_free(eleStateVariable);
            return DLNA_RET_FAILURE;
        }

        (void)ixmlNode_appendChild(eleInstanceNode, (IXML_Node *)eleStateVariable);
    }

    return DLNA_RET_SUCCESS;
}

void DmrRenderingControl::EventNotifyTimeout(void *pvArgument)
{
    IXML_Document *pstPropSet = (IXML_Document*) DLNA_NULL_PTR;
    int iretVal;
    ThreadPoolJob job = { 0 };
    char *pcRespText = (char *)DLNA_NULL_PTR;

    DLNA_UNUSED_VAR(pvArgument);

    if (DlnaCommon::getInstance().GetState() != DlnaStateType::DLNA_STATE_ACTIVE) {
        DlnaCommon::getInstance().GetDmrHdl()->bRCSSendQuickNotify = DLNA_TRUE;
        return;
    }

    /* Check Do We Need To Send Notification */
    if (DMR_RENDER_CONTROL_IS_ANY_STATE_VAR_TOBE_NOTIFIED(DlnaCommon::getInstance().GetDmrHdl()->ulRcsEventNotifySet)) {
        iretVal = CreateNotifyResponse(DLNA_FALSE, &pcRespText);
        if (iretVal != DLNA_RET_SUCCESS) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Failed to create response for Rendering Control Service\r\n", __FUNCTION__, __LINE__);
            goto DMR_RENDER_CONTROL_RESTART_TIMER;
        }

        iretVal = UpnpAddToPropertySet(&pstPropSet, rcsSubVariable_, pcRespText);
        if (iretVal != UPNP_E_SUCCESS) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Failed to add property set\r\n", __FUNCTION__, __LINE__);
            DLNA_FREE(pcRespText);
            goto DMR_RENDER_CONTROL_RESTART_TIMER;
        }

        DLNA_FREE(pcRespText);

        /* Here Presently the Argument can be NULL as we don't Use IT */
        /* Time Out Happened. Now Notify Current Rendering Control Service states to the Clients */
        iretVal = UpnpNotifyExt(DlnaCommon::getInstance().GetDmrHdl()->dev, DlnaCommon::getInstance().GetDmrHdl()->udn,
            DMR_RENDERING_CONTROL_SERVICE_ID, (IXML_Document *)pstPropSet);
        if (iretVal != UPNP_E_SUCCESS) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Failed to Send the Notify Msg.\r\n", __FUNCTION__, __LINE__);
            if (iretVal == -1) {
                DlnaCommon::getInstance().GetDmrHdl()->bRCSSendQuickNotify = DLNA_TRUE;
                return;
            }
            goto DMR_RENDER_CONTROL_RESTART_TIMER;
        }

        /* Reset the state variables bits */
        DMR_RESET_STATEVAR_BITSET(&(DlnaCommon::getInstance().GetDmrHdl()->ulRcsEventNotifySet));
    } else {
        DlnaCommon::getInstance().GetDmrHdl()->bRCSSendQuickNotify = DLNA_TRUE;
        return;
    }

/* Reschedule The Timer Thread */
DMR_RENDER_CONTROL_RESTART_TIMER:
    ixmlDocument_free(pstPropSet);

    (void)TPJobInit(&job, EventNotifyTimeout, DLNA_NULL_PTR);
    (void)TPJobSetFreeFunction(&job, NULL);
    if (TimerThreadSchedule(&gTimerThread, DMR_RENDER_CONTROL_AUTO_NOTIFY_TIME, REL_SEC, &job, SHORT_TERM,
        &(DlnaCommon::getInstance().GetDmrHdl()->iRcsTmThdId)) != 0) {
        DlnaCommon::getInstance().GetDmrHdl()->bRCSSendQuickNotify = DLNA_TRUE;
    }

    return;
}

int DmrRenderingControl::GetMute(DmrActionParam *pstActionEvent)
{
    int iRetVal;
    UpnpActionRequest *pstActionRequest = NULL;
    char *pcChannel = (char *)DLNA_NULL_PTR;
    int iInstVal;
    char aMuteVal[UINT_RANGE];

    pstActionRequest = pstActionEvent->pstActionRequest;

    iInstVal = DlnaCommon::getInstance().GetInt(pstActionRequest, INSTANCE_ID);
    if (iInstVal == DLNA_NULL_INT32) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]:incorrect datatype in the GetMute action request. \r\n");

        pstActionEvent->iErrorCode = DLNA_E_UPNP_402;
        pstActionEvent->pErrorStr = DLNA_E_UPNP_402_STRING;

        return DLNA_RET_FAILURE;
    }

    if (!IS_DMR_INSTANCE_ID_VALID(iInstVal) || !(IS_DMR_INSTANCE_ID_SUPPORTED(iInstVal))) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:GetMute action request.\r\n", __FUNCTION__, __LINE__);

        pstActionEvent->iErrorCode = DMR_RENDER_CONTROL_ERROR_702;
        pstActionEvent->pErrorStr = DMR_RENDER_CONTROL_ERROR_702_STRING;

        return DLNA_RET_FAILURE;
    }

    /* Find the Chanel Information From The Request. Validate It */
    pcChannel = DlnaCommon::getInstance().GetStr(pstActionRequest, CHANNEL);
    if (pcChannel == DLNA_NULL_PTR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][%{public}d]:No or Empty Channel\r\n", __FUNCTION__, __LINE__);

        pstActionEvent->iErrorCode = DLNA_E_UPNP_402;
        pstActionEvent->pErrorStr = DLNA_E_UPNP_402_STRING;

        return DLNA_RET_FAILURE;
    }

    if (!IsValidChannel(pcChannel)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][%{public}d]:Invalid Channel Value \r\n", __FUNCTION__, __LINE__);
        DLNA_FREE(pcChannel);

        pstActionEvent->iErrorCode = DMR_RENDER_CONTROL_ERROR_703;
        pstActionEvent->pErrorStr = DMR_RENDER_CONTROL_ERROR_703_STRING;

        return DLNA_RET_FAILURE;
    }

    DLNA_FREE(pcChannel);

    if (memset_s(aMuteVal, sizeof(aMuteVal), 0, sizeof(aMuteVal)) != EOK) {
        return DLNA_RET_FAILURE;
    }
    if (snprintf_s(aMuteVal, UINT_RANGE, UINT_RANGE - 1, "%{public}d", DlnaCommon::getInstance().GetDmrHdl()->bMuteStateVar) < EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: snprintf_s aMuteVal failed \r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    iRetVal = DlnaCommon::getInstance().AddParamToActionResponse(pstActionEvent, DMR_RENDER_CONTROL_CURRENT_MUTE, aMuteVal, sizeof(aMuteVal));
    if (iRetVal != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Failed to add the CurrentMute param\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    return DLNA_RET_SUCCESS;
}

int DmrRenderingControl::NotifyResponseImmediately()
{
    IXML_Document *pstPropSet = (IXML_Document *) DLNA_NULL_PTR;
    int iretVal;
    char *pcRespText = NULL;

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Some State Variables Have Been "
        "Changed Need to send Notify.\r\n",
        __FUNCTION__, __LINE__);

    iretVal = CreateNotifyResponse(DLNA_FALSE, &pcRespText);
    if (iretVal != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Failed to create response for Rendering Control Service "
            "SUBCRIPTION request.Return value = %{public}d\r\n",
            __FUNCTION__, __LINE__, iretVal);
        goto DMR_RENDER_CONTROL_RESTART_TIMER;
    }

    iretVal = UpnpAddToPropertySet(&pstPropSet, rcsSubVariable_, pcRespText);
    if (iretVal != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Failed to add property set."
            " Return value = %{public}d\r\n",
            __FUNCTION__, __LINE__, iretVal);
        DLNA_FREE(pcRespText);
        goto DMR_RENDER_CONTROL_RESTART_TIMER;
    }

    DLNA_FREE(pcRespText);

    if (UpnpNotifyExt(DlnaCommon::getInstance().GetDmrHdl()->dev, DlnaCommon::getInstance().GetDmrHdl()->udn, DMR_RENDERING_CONTROL_SERVICE_ID,
        (IXML_Document *)pstPropSet) != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Failed to Send the Notify Msg.\r\n", __FUNCTION__, __LINE__);
        goto DMR_RENDER_CONTROL_RESTART_TIMER;
    }

    /* Reset the state variables bits */
    DMR_RESET_STATEVAR_BITSET(&(DlnaCommon::getInstance().GetDmrHdl()->ulRcsEventNotifySet));
    return DLNA_RET_SUCCESS;
DMR_RENDER_CONTROL_RESTART_TIMER:
    ixmlDocument_free(pstPropSet);
    ThreadPoolJob job = { 0 };

    (void)TPJobInit(&job, EventNotifyTimeout, DLNA_NULL_PTR);
    (void)TPJobSetFreeFunction(&job, NULL);
    if (TimerThreadSchedule(&gTimerThread, DMR_RENDER_CONTROL_AUTO_NOTIFY_TIME, REL_SEC, &job, SHORT_TERM,
        &(DlnaCommon::getInstance().GetDmrHdl()->iRcsTmThdId)) != 0) {
        DlnaCommon::getInstance().GetDmrHdl()->bRCSSendQuickNotify = DLNA_TRUE;
    }
    return DLNA_RET_SUCCESS;
}

int DmrRenderingControl::GetVolumeInit(DmrActionParam *pstActionEvent, int *iInstVal, char **pcChannel,
    UpnpActionRequest **pstActionRequest)
{
    *pstActionRequest = pstActionEvent->pstActionRequest;

    *iInstVal = DlnaCommon::getInstance().GetInt(*pstActionRequest, INSTANCE_ID);
    if (*iInstVal == DLNA_NULL_INT32) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]:Received Invalid Instance ID with incorrect datatype in the "
            "GetVolume action request. \r\n");

        pstActionEvent->iErrorCode = DLNA_E_UPNP_402;
        pstActionEvent->pErrorStr = DLNA_E_UPNP_402_STRING;

        return DLNA_RET_FAILURE;
    }

    if (!IS_DMR_INSTANCE_ID_VALID(*iInstVal) || !(IS_DMR_INSTANCE_ID_SUPPORTED(*iInstVal))) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Received Invalid Instance ID in the "
            "GetVolume action request. Received Instance ID = %{public}d \r\n",
            __FUNCTION__, __LINE__, *iInstVal);

        pstActionEvent->iErrorCode = DMR_RENDER_CONTROL_ERROR_702;
        pstActionEvent->pErrorStr = DMR_RENDER_CONTROL_ERROR_702_STRING;

        return DLNA_RET_FAILURE;
    }

    *pcChannel = DlnaCommon::getInstance().GetStr(*pstActionRequest, CHANNEL);
    if (*pcChannel == DLNA_NULL_PTR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][%{public}d]:No or Empty Channel Parameter Is being"
            " received In the GetVolume action request  \r\n",
            __FUNCTION__, __LINE__);

        pstActionEvent->iErrorCode = DLNA_E_UPNP_402;
        pstActionEvent->pErrorStr = DLNA_E_UPNP_402_STRING;

        return DLNA_RET_FAILURE;
    }

    if (!IsValidChannel(*pcChannel)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][%{public}d]:Invalid Channel Value Is being "
            "received In the GetVolume action request CHANNEL = %{public}s \r\n",
            __FUNCTION__, __LINE__, *pcChannel);
        DLNA_FREE(*pcChannel);

        pstActionEvent->iErrorCode = DMR_RENDER_CONTROL_ERROR_703;
        pstActionEvent->pErrorStr = DMR_RENDER_CONTROL_ERROR_703_STRING;

        return DLNA_RET_FAILURE;
    }
    return DLNA_RET_SUCCESS;
}

int DmrRenderingControl::GetDesiredMute(UpnpActionRequest **pstActionRequest, int *bDesiredMute,
    DmrActionParam *pstActionEvent, char **pcChannel)
{
    char *pcMuteVal = (char *)DLNA_NULL_PTR;
    /*
        Get the DESIREDMUTE parameter. Its type is of Mute which is
        boolean.
    */
    pcMuteVal = DlnaCommon::getInstance().GetStr(*pstActionRequest, DESIREDMUTE);
    if (pcMuteVal != NULL) {
        *bDesiredMute = DlnaCommon::getInstance().ConvAscToSInt((unsigned char *)pcMuteVal);
        if (*bDesiredMute == DLNA_NULL_INT32) {
            if ((strcmp(pcMuteVal, "true") == 0) || (strcmp(pcMuteVal, "yes") == 0)) {
                *bDesiredMute = DLNA_TRUE;
            } else if ((strcmp(pcMuteVal, "false") == 0) || (strcmp(pcMuteVal, "no") == 0)) {
                *bDesiredMute = DLNA_FALSE;
            }
        }
    } else {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][%{public}d]:No or Empty DesiredMute Parameter Is being"
            " received\r\n",
            __FUNCTION__, __LINE__);
        pstActionEvent->iErrorCode = DLNA_E_UPNP_402;
        pstActionEvent->pErrorStr = DLNA_E_UPNP_402_STRING;
        DLNA_FREE(*pcChannel);
        return DLNA_RET_FAILURE;
    }

    DLNA_FREE(pcMuteVal);

    if (*bDesiredMute != DLNA_TRUE && *bDesiredMute != DLNA_FALSE) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][%{public}d]:Invalid Value For bDesiredMute is being"
            " received In the SetMute action request. Received "
            "DesiredMute = %{public}d\r\n",
            __FUNCTION__, __LINE__, *bDesiredMute);

        /* Need to check what error code we need to return here */
        pstActionEvent->iErrorCode = DLNA_E_UPNP_601;
        pstActionEvent->pErrorStr = DLNA_E_UPNP_601_STRING;
        DLNA_FREE(*pcChannel);
        return DLNA_RET_FAILURE;
    }
    return DLNA_RET_SUCCESS;
}

int DmrRenderingControl::SetMute(DmrActionParam *pstActionEvent)
{
    int iRetVal;
    UpnpActionRequest *pstActionRequest = (UpnpActionRequest *) DLNA_NULL_PTR;
    char *pcChannel = (char *)DLNA_NULL_PTR;
    int iInstVal;
    int bDesiredMute = DLNA_NULL_INT32;

    if (GetVolumeInit(pstActionEvent, &iInstVal, &pcChannel, &pstActionRequest) != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    if (GetDesiredMute(&pstActionRequest, &bDesiredMute, pstActionEvent, &pcChannel) != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    if (DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtGetSetMute) {
        DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtGetSetMute(bDesiredMute, pcChannel);
    }

    iRetVal = DlnaCommon::getInstance().AddParamToActionResponse(pstActionEvent, (char *)DLNA_NULL_PTR, (char *)DLNA_NULL_PTR, 0);
    if (iRetVal != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Failed to add the NULL param in "
            "response result for action request.\r\n",
            __FUNCTION__, __LINE__);
        DLNA_FREE(pcChannel);
        return DLNA_RET_FAILURE;
    }

    DlnaCommon::getInstance().GetDmrHdl()->bMuteStateVar = bDesiredMute;
    if (memset_s((void *)g_aucDlnaDmrRcsChannel, sizeof(g_aucDlnaDmrRcsChannel), 0, sizeof(g_aucDlnaDmrRcsChannel)) != EOK) {
        DLNA_FREE(pcChannel);
        return DLNA_RET_FAILURE;
    }
    if (strncpy_s((char *)g_aucDlnaDmrRcsChannel, sizeof(g_aucDlnaDmrRcsChannel), pcChannel, strlen(pcChannel)) != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:strncpy_s failed.\r\n", __FUNCTION__, __LINE__);
        DLNA_FREE(pcChannel);
        return DLNA_RET_FAILURE;
    }
    DLNA_FREE(pcChannel);

    DMR_SET_RCS_NOTIFY_MUTE(DlnaCommon::getInstance().GetDmrHdl()->ulRcsEventNotifySet);

    DMR_RENDER_CONTROL_DO_IMMEDIATE_EVENTING();

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: SetMute action is successful\r\n", __FUNCTION__, __LINE__);

    return DLNA_RET_SUCCESS;
}

int DmrRenderingControl::GetVolume(DmrActionParam *pstActionEvent)
{
    int iRetVal;
    char *pcChannel = (char *) DLNA_NULL_PTR;
    UpnpActionRequest *pstActionRequest = NULL;
    int iInstVal;
    char aVolumeVal[UINT_RANGE];
    int mVolume = 0;
    uintptr_t ulRetVal = DLNA_RET_FAILURE;

    if (GetVolumeInit(pstActionEvent, &iInstVal, &pcChannel, &pstActionRequest) != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    if (DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtGetVol) {
        ulRetVal = (uintptr_t)DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtGetVol(&(mVolume), pcChannel);
    }

    DLNA_FREE(pcChannel);

    if (memset_s(aVolumeVal, sizeof(aVolumeVal), 0, sizeof(aVolumeVal)) != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[%{public}s][%{public}d]: memeset_s failed \r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    if (ulRetVal == DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][%{public}d]:GetVolume action result from UI : %u", __FUNCTION__, __LINE__, mVolume);
        if (snprintf_s(aVolumeVal, UINT_RANGE, UINT_RANGE - 1, "%u", mVolume) < EOK) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:snprintf_s failed\r\n", __FUNCTION__, __LINE__);
            return DLNA_RET_FAILURE;
        }
    } else {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][%{public}d]:GetVolume action result from stack: %{public}d", __FUNCTION__, __LINE__,
            DlnaCommon::getInstance().GetDmrHdl()->ulVolumeStateVar);
        if (snprintf_s(aVolumeVal, UINT_RANGE, UINT_RANGE - 1, "%{public}d", DlnaCommon::getInstance().GetDmrHdl()->ulVolumeStateVar) < EOK) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:snprintf_s failed\r\n", __FUNCTION__, __LINE__);
            return DLNA_RET_FAILURE;
        }
    }

    iRetVal = DlnaCommon::getInstance().AddParamToActionResponse(pstActionEvent, DMR_VOLUME_CUR, aVolumeVal, sizeof(aVolumeVal));
    if (iRetVal != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Failed to add the CurrentVolume param in response result"
            " for action request.\r\n",
            __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    return DLNA_RET_SUCCESS;
}

int DmrRenderingControl::SetVolume(DmrActionParam *pstActionEvent)
{
    int iRetVal;
    UpnpActionRequest *pstActionRequest = NULL;
    char *pcChannel = (char *) DLNA_NULL_PTR;
    int iInstVal;
    unsigned int ulDesiredVol;

    if (GetVolumeInit(pstActionEvent, &iInstVal, &pcChannel, &pstActionRequest) != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }
    /*
        Get the DESIREDVOLUME parameter . Its type is of Volume which is
        unsigned integer. Its value ranges from a minimum of 0 to
        some device specific maximum, N (For Us Its 100).
    */
    ulDesiredVol = DlnaCommon::getInstance().GetUInt(pstActionRequest, DESIREDVOLUME);
    if (ulDesiredVol > DLNA_DMR_MAX_VOLUME_SUPPORTED) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][%{public}d]:Invalid Value For DesiredVolume is being"
            " received In the SetVolume action request  DesiredVolume = %u"
            " Max Volume Supported=%{public}d\r\n",
            __FUNCTION__, __LINE__, ulDesiredVol, DLNA_DMR_MAX_VOLUME_SUPPORTED);

        /* Need to check what error code we need to return here */
        pstActionEvent->iErrorCode = DLNA_E_UPNP_601;
        pstActionEvent->pErrorStr = DLNA_E_UPNP_601_STRING;
        DLNA_FREE(pcChannel);
        return DLNA_RET_FAILURE;
    }

    if (DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtSetVol) {
        DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtSetVol(ulDesiredVol, pcChannel);
    }

    iRetVal = DlnaCommon::getInstance().AddParamToActionResponse(pstActionEvent, (char *)DLNA_NULL_PTR, (char *)DLNA_NULL_PTR, 0);
    if (iRetVal != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Failed to add the NULL param in "
            "response result for action request.\r\n",
            __FUNCTION__, __LINE__);
        DLNA_FREE(pcChannel);
        return DLNA_RET_FAILURE;
    }

    DlnaCommon::getInstance().GetDmrHdl()->ulVolumeStateVar = ulDesiredVol;
    memset((void *)g_aucDlnaDmrRcsChannel, 0, sizeof(g_aucDlnaDmrRcsChannel));
    if (strncpy_s((char *)g_aucDlnaDmrRcsChannel, sizeof(g_aucDlnaDmrRcsChannel), pcChannel, strlen(pcChannel)) != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:strncpy_s failed.\r\n", __FUNCTION__, __LINE__);
        DLNA_FREE(pcChannel);
        return DLNA_RET_FAILURE;
    }
    DLNA_FREE(pcChannel);

    DMR_SET_RCS_NOTIFY_VOLUME(DlnaCommon::getInstance().GetDmrHdl()->ulRcsEventNotifySet);

    DMR_RENDER_CONTROL_DO_IMMEDIATE_EVENTING();

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: SetVolume action is successful\r\n", __FUNCTION__, __LINE__);

    return DLNA_RET_SUCCESS;
}


int DmrRenderingControl::ListPresets(DmrActionParam *pstActionEvent)
{
    int iRetVal;
    UpnpActionRequest *pstActionRequest = NULL;
    int iInstVal;

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Peer has invoked the ListPresets"
        " action\r\n",
        __FUNCTION__, __LINE__);

    pstActionRequest = pstActionEvent->pstActionRequest;

    iInstVal = DlnaCommon::getInstance().GetInt(pstActionRequest, INSTANCE_ID);
    if (iInstVal == DLNA_NULL_INT32) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]:Received Invalid Instance ID with incorrect datatype in the "
            "ListPresets action request. \r\n");

        pstActionEvent->iErrorCode = DLNA_E_UPNP_402;
        pstActionEvent->pErrorStr = DLNA_E_UPNP_402_STRING;

        return DLNA_RET_FAILURE;
    }

    if (!IS_DMR_INSTANCE_ID_VALID(iInstVal) || !(IS_DMR_INSTANCE_ID_SUPPORTED(iInstVal))) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]:Received Invalid Instance ID in the ListPresets action "
            "request. Received Instance ID = %{public}d \r\n",
            iInstVal);

        pstActionEvent->iErrorCode = DMR_RENDER_CONTROL_ERROR_702;
        pstActionEvent->pErrorStr = DMR_RENDER_CONTROL_ERROR_702_STRING;

        return DLNA_RET_FAILURE;
    }

    iRetVal = DlnaCommon::getInstance().AddParamToActionResponse(pstActionEvent, CURRENT_PRESET_NAMELIST,
        "FactoryDefaults,InstallationDefaults", strlen("FactoryDefaults,InstallationDefaults"));
    if (iRetVal != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Failed to add the CURRENT_PRESET_NAMELIST param in "
            "response result for action request.\r\n",
            __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: ListPresets action is successful\r\n", __FUNCTION__, __LINE__);

    return DLNA_RET_SUCCESS;
}

int DmrRenderingControl::SelectPreset(DmrActionParam *pstActionEvent)
{
    int iRetVal;
    UpnpActionRequest *pstActionRequest = NULL;
    int iInstVal;
    char *pcPresetName = NULL;

    pstActionRequest = pstActionEvent->pstActionRequest;

    iInstVal = DlnaCommon::getInstance().GetInt(pstActionRequest, INSTANCE_ID);
    if (iInstVal == DLNA_NULL_INT32) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]:Received Invalid Instance ID SelectPreset action request. \r\n");

        pstActionEvent->iErrorCode = DLNA_E_UPNP_402;
        pstActionEvent->pErrorStr = DLNA_E_UPNP_402_STRING;

        return DLNA_RET_FAILURE;
    }

    if (!IS_DMR_INSTANCE_ID_VALID(iInstVal) || !(IS_DMR_INSTANCE_ID_SUPPORTED(iInstVal))) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]:Received Invalid Instance ID Received Instance ID = %{public}d \r\n", iInstVal);

        pstActionEvent->iErrorCode = DMR_RENDER_CONTROL_ERROR_702;
        pstActionEvent->pErrorStr = DMR_RENDER_CONTROL_ERROR_702_STRING;

        return DLNA_RET_FAILURE;
    }

    /* Get The Preset name given by the Peer (DMC) */
    pcPresetName = DlnaCommon::getInstance().GetStr(pstActionRequest, PRESET_NAME);
    if (pcPresetName == DLNA_NULL_PTR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]:Didn't find PresetName in SelectPreset action request\r\n");

        pstActionEvent->iErrorCode = DLNA_E_UPNP_402;
        pstActionEvent->pErrorStr = DLNA_E_UPNP_402_STRING;

        return DLNA_RET_FAILURE;
    }

    if ((DLNA_STRCMP(pcPresetName, "FactoryDefaults")) && (DLNA_STRCMP(pcPresetName, "InstallationDefaults"))) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Invalid Preset name is %{public}s\r\n", __FUNCTION__, __LINE__, pcPresetName);

        /* Set the error response return values */
        pstActionEvent->iErrorCode = DMR_RENDER_CONTROL_ERROR_701;
        pstActionEvent->pErrorStr = DMR_RENDER_CONTROL_ERROR_701_STRING;
        DLNA_FREE(pcPresetName);
        return DLNA_RET_FAILURE;
    }

    iRetVal = DlnaCommon::getInstance().AddParamToActionResponse(pstActionEvent, (char *)DLNA_NULL_PTR, (char *)DLNA_NULL_PTR, 0);
    if (iRetVal != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Failed to add the NULL param\r\n", __FUNCTION__, __LINE__);
        DLNA_FREE(pcPresetName);
        return DLNA_RET_FAILURE;
    }

    DlnaCommon::getInstance().GetDmrHdl()->ulPresetNameStateVar = (DLNA_STRCMP(pcPresetName, "FactoryDefaults") == DLNA_NULL) ?
        DMR_RENDER_CONTROL_PRESETNAME_FACTORY_DEFAULTS :
        DMR_RENDER_CONTROL_PRESETNAME_INSTAL_DEFAULTS;

    DLNA_FREE(pcPresetName);

    DMR_SET_RCS_NOTIFY_PRESETNAME(DlnaCommon::getInstance().GetDmrHdl()->ulRcsEventNotifySet);

    DMR_RENDER_CONTROL_DO_IMMEDIATE_EVENTING();

    return DLNA_RET_SUCCESS;
}

DmrServiceAction* DmrRenderingControl::getActionFuncTable(){
    static DmrServiceAction dmrRcsActionFuncTable[] = {
        { DMR_RENDER_CONTROL_ACTION_GET_MUTE, DmrRenderingControl::GetMute},
        { DMR_RENDER_CONTROL_ACTION_SET_MUTE, DmrRenderingControl::SetMute },
        { DMR_RENDER_CONTROL_ACTION_GET_VOLUME, DmrRenderingControl::GetVolume },
        { DMR_RENDER_CONTROL_ACTION_SET_VOLUME, DmrRenderingControl::SetVolume },
        { DMR_RENDER_CONTROL_ACTION_LIST_PRESETS, DmrRenderingControl::ListPresets },
        { DMR_RENDER_CONTROL_ACTION_SELECT_PRESET, DmrRenderingControl::SelectPreset },
        { NULL, NULL}
    };
	return (DmrServiceAction*)dmrRcsActionFuncTable;
}

#ifdef __cplusplus
}

}
}
}
}
#endif
