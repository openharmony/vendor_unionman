/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: dmr AVTransport service
 * Author: 
 * Create: 2023-10-20
 */

#include <netinet/in.h>
//#include "dlna_log.h"
#include <hilog/log.h>
#include "upnpapi.h"
#include "TimerThread.h"
#include "ThreadPool.h"
#include "ixmlmembuf.h"
#include "dlna_buffer_manager.h"
#include "dlna_dmr.h"
#include "securec.h"
#include "dmr_av_transport.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

//DEFINE_DLNA_LABEL("Cast-dmr-AVTransport");
#define DLNA_LOG_TAG "Dlna_dmr-AVTransport"

SeekModeTarget DmrAVTransport::stDMRSeekModes_[11] = {
    {"TRACK_NR", 0, DlnaParaType::DLNA_PARA_TYPE_UINT32},
    {"ABS_TIME", 1, DlnaParaType::DLNA_PARA_TYPE_STRING},
    {"REL_TIME", 2, DlnaParaType::DLNA_PARA_TYPE_STRING},
    {"ABS_COUNT", 3, DlnaParaType::DLNA_PARA_TYPE_UINT32},
    {"REL_COUNT", 4, DlnaParaType::DLNA_PARA_TYPE_INT32},
    {"CHANNEL_FREQ", 5, DlnaParaType::DLNA_PARA_TYPE_FLOAT},
    {"TAPE-INDEX", 6, DlnaParaType::DLNA_PARA_TYPE_UINT32},
    {"REL_TAPE-INDEX", 7, DlnaParaType::DLNA_PARA_TYPE_INT32},
    {"FRAME", 8, DlnaParaType::DLNA_PARA_TYPE_UINT32},
    {"REL_FRAME", 9, DlnaParaType::DLNA_PARA_TYPE_INT32},
    {NULL, -1, DlnaParaType::DLNA_PARA_TYPE_BUTT}
};

char *DmrAVTransport::szDMRStatus_[7] = {
		"STOPPED", "PLAYING", "PAUSED_PLAYBACK",
		"RECORDING", "PAUSED_RECODING", "NO_MEDIA_PRESENT", "TRANSITIONING"
	};

DmrAVTransport &DmrAVTransport::getInstance()
{
	static DmrAVTransport instance;
	return instance;
}	

/* !
 * \brief Appends a string to a buffer, substituting some characters by escape
 * sequences.
 */
void DmrAVTransport::CopyWithEscape(void *buf, const char *pBuf)
{
    size_t i;
    size_t plen;
    ixml_membuf *buffer = static_cast<ixml_membuf *>(buf);
	
    if (pBuf == NULL) {
        return;
    }
    plen = strlen(pBuf);
    for (i = (size_t)0; i < plen; ++i) {
        switch (pBuf[i]) {
            case '<':
                ixml_membuf_append_str(buffer, "&lt;");
                break;
            case '>':
                ixml_membuf_append_str(buffer, "&gt;");
                break;
            case '&':
                ixml_membuf_append_str(buffer, "&amp;");
                break;
            case '\'':
                ixml_membuf_append_str(buffer, "&apos;");
                break;
            case '\"':
                ixml_membuf_append_str(buffer, "&quot;");
                break;
            default:
                ixml_membuf_append(buffer, &pBuf[i]);
                break;
        }
    }
}

DOMString DmrAVTransport::IxmlDoXmlEscaping(const char *value)
{
    ixml_membuf memBuf;
    ixml_membuf *buf = &memBuf;
    (void)ixml_membuf_init(buf);
    CopyWithEscape(static_cast<void *>(buf), value);
    return buf->buf;
}

void DmrAVTransport::StartNotifyTimer()
{
    ThreadPoolJob jobAvt = { 0 };

    (void)TPJobInit(&jobAvt, EventNotifyTimeout, DLNA_NULL_PTR);
    (void)TPJobSetFreeFunction(&jobAvt, NULL);
    if (TimerThreadSchedule(&gTimerThread, DLNA_DMR_AVTRANSPORT_AUTO_NOTIFY_TIME, REL_SEC, &jobAvt, SHORT_TERM,
                            &(DlnaCommon::getInstance().GetDmrHdl()->iAvtTmThdId)) != 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d] TimerThreadSchedule failure", __FUNCTION__, __LINE__);
        DlnaCommon::getInstance().GetDmrHdl()->bSendQuickNotify = DLNA_TRUE;
    }
}

void DmrAVTransport::EventNotifyTimeout(void *pvArgument)
{
    int iretVal;
    void *pstEventBuff = NULL;

    DLNA_UNUSED_VAR(pvArgument);

    DLNA_LOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
    if (DlnaCommon::getInstance().GetState() != DlnaStateType::DLNA_STATE_ACTIVE) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Eventing will be stopped \r\n", __FUNCTION__, __LINE__);
        DlnaCommon::getInstance().GetDmrHdl()->bSendQuickNotify = DLNA_TRUE;
        DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
        return;
    }

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: AvTransport eventing timer has expired.\r\n", __FUNCTION__, __LINE__);

    /* Check Do We Need To Send Notification */
    if (DMR_AVTRANSPORT_IS_ANY_STATE_VAR_TOBE_NOTIFIED(DlnaCommon::getInstance().GetDmrHdl()->ulAvtEventNotifySet)) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Need to send Notify.\r\n", __FUNCTION__, __LINE__);

        pstEventBuff = DlnaBufferManager::AllocStrBuff();
        if (pstEventBuff == DLNA_NULL_PTR) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: AllocStrBuff Failed\r\n ", __FUNCTION__, __LINE__);
            goto DMR_RESTART_TIMER;
        }

        DmrAVTransport::getInstance().CreateAvtNotifyResponse(DLNA_FALSE, pstEventBuff);

        iretVal = UpnpNotifyExt(DlnaCommon::getInstance().GetDmrHdl()->dev, DlnaCommon::getInstance().GetDmrHdl()->udn, DMR_AV_TRANSPORT_SERVICE_ID,
                                (IXML_Document *)(DlnaBufferManager::StrBuffGetBuff(pstEventBuff)));
        DlnaBufferManager::FreeStrBuff(pstEventBuff);
        if (iretVal != UPNP_E_SUCCESS) {
            OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]: Return value = %{public}d\r\n", __FUNCTION__, __LINE__, iretVal);
            if (iretVal == -1) {
                goto DMR_EXIT_TIMER;
            } else {
                goto DMR_RESTART_TIMER;
            }
        }

        /* Reset the state variables bits */
        DMR_RESET_STATEVAR_BITSET(&(DlnaCommon::getInstance().GetDmrHdl()->ulAvtEventNotifySet));
    } else {
        goto DMR_EXIT_TIMER;
    }

    /* Reschedule The Timer Thread */
DMR_RESTART_TIMER:
    StartNotifyTimer();
    DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
    return;

DMR_EXIT_TIMER:
    DlnaCommon::getInstance().GetDmrHdl()->bSendQuickNotify = DLNA_TRUE;
    DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
    return;
}

void DmrAVTransport::AddStateVarForInitialNotify(void *pstEventRespBuf)
{
    DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "TransportStatus", "val", "OK")

    DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "CurrentPlayMode", "val", "NORMAL")

    DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "TransportPlaySpeed", "val", "1")

    DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "CurrentMediaDuration", "val", "NOT_IMPLEMENTED")

    /* NextAVTransportURI */
    DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "NextAVTransportURI", "val", "NOT_IMPLEMENTED")

    /* NextAVTransportURIMetaData */
    DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "NextAVTransportURIMetaData", "val", "NOT_IMPLEMENTED")

    if (DlnaCommon::getInstance().GetDmrHdl()->dmrState == static_cast<int>(DmrAvState::DLNA_DMR_STATE_NO_MEDIA_PRESENT)) {
        DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "PlaybackStorageMedium", "val", "NONE")
    } else {
        DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "PlaybackStorageMedium", "val", "NETWORK")
    }

    /* RecordStorageMedium */
    DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "RecordStorageMedium", "val", "NOT_IMPLEMENTED")

    /* RecordMediumWriteStatus */
    DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "RecordMediumWriteStatus", "val", "NOT_IMPLEMENTED")

    /* CurrentRecordQualityMode */
    DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "CurrentRecordQualityMode", "val", "NOT_IMPLEMENTED")

    DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "PossiblePlaybackStorageMedia", "val", "NOT_IMPLEMENTED")

    DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "PossibleRecordStorageMedia", "val", "NOT_IMPLEMENTED")

    DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "PossibleRecordQualityModes", "val", "NOT_IMPLEMENTED")

    DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "CurrentRecordQualityMode", "val", "NOT_IMPLEMENTED")
}

void DmrAVTransport::CreateAvtNotifyResponse(int isInitialNotify, void *pstEventRespBuf)
{
    DLNADMR_AVTRANSPORT_START_TAG(pstEventRespBuf, "e:propertyset xmlns:e=\"urn:schemas-upnp-org:event-1-0\"")

    DLNADMR_AVTRANSPORT_START_TAG(pstEventRespBuf, "e:property")
    DLNADMR_AVTRANSPORT_START_TAG(pstEventRespBuf, "LastChange")

    DLNADMR_AVTRANSPORT_ESCAPED_OPEN_STARTTAG(pstEventRespBuf, "Event")
    DLNADMR_AVTRANSPORT_WRITE_ATTRIBUTE_NAME(pstEventRespBuf, "xmlns")
    DLNADMR_AVTRANSPORT_WRITE_ATTR_STANDARD_VAL(pstEventRespBuf, "urn:schemas-upnp-org:metadata-1-0/AVT/")
    DLNADMR_AVTRANSPORT_WRITE_ATTRIBUTE_NAME(pstEventRespBuf, "xmlns:xsi")
    DLNADMR_AVTRANSPORT_WRITE_ATTR_STANDARD_VAL(pstEventRespBuf, "http://www.w3.org/2001/XMLSchema-instance")
    DLNADMR_AVTRANSPORT_WRITE_ATTRIBUTE_NAME(pstEventRespBuf, "xsi:schemaLocation")
    DLNADMR_AVTRANSPORT_WRITE_LASTATTR_STANDARD_VAL(pstEventRespBuf,
                                            "urn:schemas-upnp-org:metadata-1-0/AVT/http://www.upnp.org/schemas/"
                                            "av/avt-event-v1-20060531.xsd")
    DLNADMR_AVTRANSPORT_ESCAPED_CLOSE_STARTTAG(pstEventRespBuf)

    DLNADMR_AVTRANSPORT_ESCAPED_OPEN_STARTTAG(pstEventRespBuf, "InstanceID")
    DLNADMR_AVTRANSPORT_WRITE_ATTRIBUTE_NAME(pstEventRespBuf, "val")
    DLNADMR_AVTRANSPORT_WRITE_LASTATTR_STANDARD_VAL(pstEventRespBuf, "0")
    DLNADMR_AVTRANSPORT_ESCAPED_CLOSE_STARTTAG(pstEventRespBuf)

    CreateAvtOptNotifyContents(isInitialNotify, pstEventRespBuf);

    DLNADMR_AVTRANSPORT_ESCAPED_END_TAG(pstEventRespBuf, "InstanceID")
    DLNADMR_AVTRANSPORT_ESCAPED_END_TAG(pstEventRespBuf, "Event")
    DLNADMR_AVTRANSPORT_END_TAG(pstEventRespBuf, "LastChange")
    DLNADMR_AVTRANSPORT_END_TAG(pstEventRespBuf, "e:property")
    DLNADMR_AVTRANSPORT_END_TAG(pstEventRespBuf, "e:propertyset")

    return;
}

void DmrAVTransport::CreateTransportStateToNotify(void *pstEventRespBuf)
{
    DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "TransportState", "val",
        szDMRStatus_[DlnaCommon::getInstance().GetDmrHdl()->dmrState]);
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d] dmrState in NOTIFY:%{public}s", __FUNCTION__, __LINE__,
        szDMRStatus_[DlnaCommon::getInstance().GetDmrHdl()->dmrState]);

    if (DlnaCommon::getInstance().GetDmrHdl()->pszCurrentTransportActions &&
        (strlen(DlnaCommon::getInstance().GetDmrHdl()->pszCurrentTransportActions) > 0)) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d] CurrentTransportActions in lastchange:%{public}s\r\n", __FUNCTION__, __LINE__,
            DlnaCommon::getInstance().GetDmrHdl()->pszCurrentTransportActions);
        DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "CurrentTransportActions", "val",
            DlnaCommon::getInstance().GetDmrHdl()->pszCurrentTransportActions)
    } else {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d] CurrentTransportActions in lastchange: null", __FUNCTION__, __LINE__);
        DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "CurrentTransportActions", "val", "")
    }
}

void DmrAVTransport::CreateCURTRACKURIToNotify(void *pstEventRespBuf)
{
    if (DlnaCommon::getInstance().GetDmrHdl()->dmrState != static_cast<int>(DmrAvState::DLNA_DMR_STATE_NO_MEDIA_PRESENT)) {
        DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "NumberOfTracks", "val", "1")

        DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "CurrentTrack", "val", "1")
    } else {
        DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "NumberOfTracks", "val", "0")

        DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "CurrentTrack", "val", "0")
    }

    if (DlnaCommon::getInstance().GetDmrHdl()->pszPlayURL && strlen(DlnaCommon::getInstance().GetDmrHdl()->pszPlayURL)) {
        DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "AVTransportURI", "val",
            DlnaCommon::getInstance().GetDmrHdl()->pszPlayURL)

        DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "CurrentTrackURI", "val",
            DlnaCommon::getInstance().GetDmrHdl()->pszPlayURL)
    } else {
        DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "AVTransportURI", "val", "")

        DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "CurrentTrackURI", "val", "")
    }
}

void DmrAVTransport::CreateAvtOptNotifyContentsData(int isInitialNotify, void *pstEventRespBuf)
{
    if ((isInitialNotify != DLNA_TRUE) &&
        (!DMR_AVTRANSPORT_IS_CURTRACKMETADATA_TO_NOTIFY(DlnaCommon::getInstance().GetDmrHdl()->ulAvtEventNotifySet))) {
            return;
    }
    char *pcAttrVal = NULL;
    char *pcAttrVal1 = NULL;

    if (DlnaCommon::getInstance().GetDmrHdl()->pszTrackMetaData && strlen(DlnaCommon::getInstance().GetDmrHdl()->pszTrackMetaData)) {
        /* AVTransportURIMetaData */
        pcAttrVal = (char *)IxmlDoXmlEscaping(DlnaCommon::getInstance().GetDmrHdl()->pszTrackMetaData);
        if (pcAttrVal != NULL) {
            pcAttrVal1 = (char *)IxmlDoXmlEscaping(pcAttrVal);
        }
        if (pcAttrVal1 != NULL) {
            DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "AVTransportURIMetaData", "val", pcAttrVal1)
        }

        DLNA_FREE(pcAttrVal);
        DLNA_FREE(pcAttrVal1);

        /* CurrentTrackMetaData */
        pcAttrVal = (char *)IxmlDoXmlEscaping(DlnaCommon::getInstance().GetDmrHdl()->pszTrackMetaData);
        if (pcAttrVal != NULL) {
            pcAttrVal1 = (char *)IxmlDoXmlEscaping(pcAttrVal);
        }
        if (pcAttrVal1 != NULL) {
            DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "CurrentTrackMetaData", "val", pcAttrVal1)
        }

        DLNA_FREE(pcAttrVal);
        DLNA_FREE(pcAttrVal1);
        return;
    }
    DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "AVTransportURIMetaData", "val", "")

    DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "CurrentTrackMetaData", "val", "")
}

void DmrAVTransport::CreateAvtOptNotifyContents(int isInitialNotify, void *pstEventRespBuf)
{
    /* DMR state element  */
    if ((isInitialNotify == DLNA_TRUE) ||
        (DMR_AVTRANSPORT_IS_TRANSPORT_STATE_TO_NOTIFY(DlnaCommon::getInstance().GetDmrHdl()->ulAvtEventNotifySet))) {
        CreateTransportStateToNotify(pstEventRespBuf);
    }

    if ((isInitialNotify == DLNA_TRUE) || (DMR_AVTRANSPORT_IS_CURTRACKURI_TO_NOTIFY(DlnaCommon::getInstance().GetDmrHdl()->ulAvtEventNotifySet))) {
        CreateCURTRACKURIToNotify(pstEventRespBuf);
    }
    CreateAvtOptNotifyContentsData(isInitialNotify, pstEventRespBuf);

    if ((isInitialNotify == DLNA_TRUE) || (DMR_AVTRANSPORT_IS_CURTRACKDUR_TO_NOTIFY(DlnaCommon::getInstance().GetDmrHdl()->ulAvtEventNotifySet))) {
        /* CurrentTrackDuration */
        DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pstEventRespBuf, "CurrentTrackDuration", "val",
            DlnaCommon::getInstance().GetDmrHdl()->currentduration)
    }

    if (isInitialNotify != 0) {
        AddStateVarForInitialNotify(pstEventRespBuf);
    }
}

void DmrAVTransport::DoImmediateEventing()
{
    int iretVal;
    void *pstEventBuff;
    ThreadPoolJob jobAvt = { 0 };

    pstEventBuff = DlnaBufferManager::AllocStrBuff();
    if (pstEventBuff == DLNA_NULL_PTR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: AllocStrBuff Failed\r\n ", __FUNCTION__, __LINE__);
        return;
    }
    if (DlnaCommon::getInstance().GetDmrHdl()->ulDeviceInit == DLNA_FALSE) {
        DlnaBufferManager::FreeStrBuff(pstEventBuff);
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "%{public}s L%d: dlna stack is deinit, not handle", __FUNCTION__, __LINE__);
        return;
    }
    DLNA_LOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
    /* Need To Send The Notify Here */
    DlnaCommon::getInstance().GetDmrHdl()->bSendQuickNotify = DLNA_FALSE;
    DmrAVTransport::getInstance().CreateAvtNotifyResponse(DLNA_FALSE, pstEventBuff);

    /* to avoid race conditions stop the timer */
    (void)TimerThreadRemove(&gTimerThread, DlnaCommon::getInstance().GetDmrHdl()->iAvtTmThdId, &jobAvt);

    iretVal = UpnpNotifyExt(DlnaCommon::getInstance().GetDmrHdl()->dev, DlnaCommon::getInstance().GetDmrHdl()->udn, DMR_AV_TRANSPORT_SERVICE_ID,
                            (IXML_Document *)(DlnaBufferManager::StrBuffGetBuff(pstEventBuff)));

    DlnaBufferManager::FreeStrBuff(pstEventBuff);
    StartNotifyTimer();
    if (iretVal != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Failed to Send the Notify Msg."
            " Return value = %{public}d\r\n", __FUNCTION__, __LINE__, iretVal);
        goto DlnaDmrAvtDoImmediateEventingExit;
    }

    /* Reset the state variables bits */
    DMR_RESET_STATEVAR_BITSET(&(DlnaCommon::getInstance().GetDmrHdl()->ulAvtEventNotifySet));

DlnaDmrAvtDoImmediateEventingExit:
    DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
    return;
}

void DmrAVTransport::SetCurrentActionWithStopped()
{
    char *dlnaCurrentAction = NULL;
    dlnaCurrentAction = DlnaCommon::getInstance().GetDmrHdl()->isImage ? (char *)DLNA_DMR_CURRENTACTION_STOP_IMAGE
            : (char *)DLNA_DMR_CURRENTACTION_STOP;
    if (snprintf_s(DlnaCommon::getInstance().GetDmrHdl()->pszCurrentTransportActions, DLNA_BUFF_SIZE, DLNA_BUFF_SIZE - 1,
        "%{public}s", dlnaCurrentAction) < EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:snprintf_s failed\r\n", __FUNCTION__, __LINE__);
    }
}

void DmrAVTransport::SetCurrentActionWithPlaying()
{
    char *dlnaCurrentAction = NULL;
    dlnaCurrentAction = DlnaCommon::getInstance().GetDmrHdl()->isImage ? (char *)DLNA_DMR_CURRENTACTION_PLAY_IMAGE
            : (char *)DLNA_DMR_CURRENTACTION_PLAY;
    if (snprintf_s(DlnaCommon::getInstance().GetDmrHdl()->pszCurrentTransportActions, DLNA_BUFF_SIZE, DLNA_BUFF_SIZE - 1,
        "%{public}s", dlnaCurrentAction) < EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:snprintf_s failed\r\n", __FUNCTION__, __LINE__);
    }
}

void DmrAVTransport::SetCurrentActionWithPresent()
{
    char *dlnaCurrentAction = NULL;
    dlnaCurrentAction = DlnaCommon::getInstance().GetDmrHdl()->isImage ? (char *)DLNA_DMR_CURRENTACTION_PAUSE_IMAGE
            : (char *)DLNA_DMR_CURRENTACTION_PAUSE;
    if (snprintf_s(DlnaCommon::getInstance().GetDmrHdl()->pszCurrentTransportActions, DLNA_BUFF_SIZE, DLNA_BUFF_SIZE - 1,
        "%{public}s", dlnaCurrentAction) < EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:snprintf_s failed\r\n", __FUNCTION__, __LINE__);
    }
}

void DmrAVTransport::NoMediaWithPresent()
{
    if (strncpy_s(DlnaCommon::getInstance().GetDmrHdl()->pszCurrentTransportActions,
        DLNA_BUFF_SIZE, "", (strlen("") + 1)) != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:strncpy_s failed\r\n", __FUNCTION__, __LINE__);
    }
}

void DmrAVTransport::SetCurrentAction(int state)
{
    if (memset_s(DlnaCommon::getInstance().GetDmrHdl()->pszCurrentTransportActions, DLNA_BUFF_SIZE, 0, DLNA_BUFF_SIZE) != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[%{public}s][%{public}d]:SetCurrentAction memeset_s failed \r\n", __FUNCTION__, __LINE__);
        return;
    }
    switch (state) {
        case static_cast<int>(DmrAvState::DLNA_DMR_STATE_STOPPED):
            SetCurrentActionWithStopped();
            break;
        case static_cast<int>(DmrAvState::DLNA_DMR_STATE_PLAYING):
            SetCurrentActionWithPlaying();
            break;
        case static_cast<int>(DmrAvState::DLNA_DMR_STATE_PAUSED_PLAYBACK):
            SetCurrentActionWithPresent();
            break;
        case static_cast<int>(DmrAvState::DLNA_DMR_STATE_NO_MEDIA_PRESENT):
            NoMediaWithPresent();
            break;
        default:
            break;
    }
}

int DmrAVTransport::HttpReceiveMsg(int s32Socket, char *pchMsgBuf, int *u32MsgLen)
{
    fd_set tReadFd;
    struct timeval tTimeOut;
    int u32ActReadLen;
    int iRet;

    FD_ZERO(&tReadFd);
    FD_SET((unsigned int)s32Socket, &tReadFd);
    tTimeOut.tv_sec = 0;
    tTimeOut.tv_usec = 500 * 1000; // 500ms
    iRet = select(s32Socket + 1, &tReadFd, 0, 0, &tTimeOut);
    if (iRet < 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "Rtsp Client Read Message Failed!Socket Not Ready.[Sock: %{public}d]\n", s32Socket);
        return -1;
    } else if (iRet == 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "OHOS_RTSP_ReceiveMsg timeout after 5s \n");
        return -1;
    }
    u32ActReadLen = recv((unsigned int)s32Socket, pchMsgBuf, *u32MsgLen, MSG_NOSIGNAL);
    if (u32ActReadLen <= 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "RTSP can't read from Server u32ActReadLen : %{public}d\n", u32ActReadLen);
        return -1;
    }

    // set terminated string
    if (u32ActReadLen >= HTTPREQUESTLEN) {
        if (memset_s(pchMsgBuf + HTTPREQUESTLEN - 1, HTTPMSGTAILLEN, '\0', HTTPMSGTAILLEN) != EOK) {
            return -1;
        }
    } else {
        if (memset_s(pchMsgBuf + u32ActReadLen - 1, HTTPMSGTAILLEN, '\0', HTTPMSGTAILLEN) != EOK) {
            return -1;
        }
    }

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "recv len : %{public}d\r\n", u32ActReadLen);
    *u32MsgLen = u32ActReadLen;
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "pchMsgBuf : %{public}s,len : %{public}d\r\n", pchMsgBuf, u32ActReadLen);
    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::HttpSendMsg(int s32Socket, char *pchMsgBuf, int s32MsgLen)
{
    fd_set tWriteFd;
    struct timeval tTimeOut;
    int u32ActSend;

    if (pchMsgBuf == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "HTTP Client Send Message Failed!Message Buffer is Null.\n");
        return -1;
    }
    FD_ZERO(&tWriteFd);
    FD_SET((unsigned int)s32Socket, &tWriteFd);
    tTimeOut.tv_sec = 0;
    tTimeOut.tv_usec = 500 * 1000; // 500 ms
    if (select(s32Socket + 1, 0, &tWriteFd, 0, &tTimeOut) <= 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "HTTP Client Send Message Failed!Socket Not Ready.[Sock: %{public}d]\n", s32Socket);
        return -1;
    }
    u32ActSend = send((unsigned int)s32Socket, pchMsgBuf, s32MsgLen, 0);
    if (u32ActSend < 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "send error\n");
        return -1;
    }
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "http Client Send Message :\n%{public}s\n,u32ActSend  = %{public}d\n", pchMsgBuf, u32ActSend);
    return DLNA_RET_SUCCESS;
}

bool DmrAVTransport::AppendString(char *dest, size_t destMax, const char *src, size_t count)
{
    if (src == NULL) {
        return false;
    }
    int ret = strncat_s(dest, destMax, src, count);
    if (ret != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]:strncat_s failed! ret = %{public}d \r\n", __FUNCTION__, __LINE__, ret);
        return false;
    }
    return true;
}

bool DmrAVTransport::CopyString(char *dest, size_t destMax, const char *src, size_t count)
{
    if (src == NULL) {
        return false;
    }
    int ret = memset_s(dest, destMax, 0, destMax);
    if (ret != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]:memset_s failed! ret = %{public}d \r\n", __FUNCTION__, __LINE__, ret);
        return false;
        }
    ret = strncpy_s(dest, destMax, src, count);
    if (ret != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]:strncpy_s failed! ret = %{public}d \r\n", __FUNCTION__, __LINE__, ret);
        return false;
    }
    return true;
}

int DmrAVTransport::SeparateUrl(const char *pUrl, UrlPart *urlPart)
{
    const char pHttpHead[] = "http://";
    char pHttpPort[HTTPPORTLEN];

    char *pFlagPtr = (char *)strstr(pUrl, pHttpHead);
    if (pFlagPtr == DLNA_NULL_PTR) {
        return DLNA_RET_FAILURE;
    }
    pFlagPtr += strlen(pHttpHead);
    char *pEndPtr = (char *)strchr(pFlagPtr, ':');
    if (pEndPtr == NULL) {
        pEndPtr = strchr(pFlagPtr, '/');
        if (pEndPtr == NULL) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]:SetCurrentURI seg.\r\n", __FUNCTION__, __LINE__);
            return DLNA_RET_FAILURE;
        }
        if (!CopyString(urlPart->host, HTTPIPADDRLEN, pFlagPtr, pEndPtr - pFlagPtr)) {
            return DLNA_RET_FAILURE;
        }
    } else {
        if (!CopyString(urlPart->host, HTTPIPADDRLEN, pFlagPtr, pEndPtr - pFlagPtr)) {
            return DLNA_RET_FAILURE;
        }
        pFlagPtr = pEndPtr + 1;
        pEndPtr = strchr(pFlagPtr, '/');
        if (pEndPtr == NULL) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]:SetCurrentURI seg.\r\n", __FUNCTION__, __LINE__);
            return DLNA_RET_FAILURE;
        }
        if (!CopyString(pHttpPort, sizeof(pHttpPort), pFlagPtr, pEndPtr - pFlagPtr)) {
            return DLNA_RET_FAILURE;
        }
        urlPart->u32Port = (unsigned int)atoi(pHttpPort);
    }
    // path
    if (strlen(pEndPtr) >= URILEN) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "define url len < data len!!\r\n");
        return DLNA_RET_FAILURE;
    }
    if (!CopyString(urlPart->path, URILEN, pEndPtr, (strlen(pEndPtr) + 1))) {
        return DLNA_RET_FAILURE;
    }
    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::InitHttpRequest(char *httpRequest, int httpRequestLen, UrlPart urlPart)
{
    char httpMsgBuffer[HTTPMSGBUFFERLEN];
    if (memset_s(httpRequest, httpRequestLen, 0, httpRequestLen) != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]:memset_s failed\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }
    if (memset_s(httpMsgBuffer, HTTPMSGBUFFERLEN, 0, HTTPMSGBUFFERLEN) != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]:memset_s failed\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }
    if (snprintf_s(httpRequest, httpRequestLen, httpRequestLen - 1, "HEAD %{public}s HTTP/1.1\r\n", urlPart.path) < EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]:snprintf_s failed\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }
    if (snprintf_s(httpMsgBuffer, HTTPMSGBUFFERLEN, HTTPMSGBUFFERLEN - 1,
        "Host:%{public}s:%u\r\n", urlPart.host, urlPart.u32Port) < EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]:snprintf_s failed\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }
    if (!AppendString(httpRequest, httpRequestLen, httpMsgBuffer, strlen(httpMsgBuffer))) {
        return DLNA_RET_FAILURE;
    }
    if (snprintf_s(httpMsgBuffer, HTTPMSGBUFFERLEN, HTTPMSGBUFFERLEN - 1, "Connection: Keep-Alive\r\n") < EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]:snprintf_s failed\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }
    if (!AppendString(httpRequest, httpRequestLen, httpMsgBuffer, strlen(httpMsgBuffer))) {
        return DLNA_RET_FAILURE;
    }
    if (snprintf_s(httpMsgBuffer, HTTPMSGBUFFERLEN, HTTPMSGBUFFERLEN - 1, "Accept-Encoding: gzip\r\n\r\n") < EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]:snprintf_s failed\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }
    if (!AppendString(httpRequest, httpRequestLen, httpMsgBuffer, strlen(httpMsgBuffer))) {
        return DLNA_RET_FAILURE;
    }
    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::GetMediaTypeFromContentString(char *pMediaType, size_t mediaTypeLen,
    char *pContentString, size_t contentStringLen)
{
    if (contentStringLen <= 0) {
        return DLNA_RET_FAILURE;
    }

    // decide media type by http header: Content-Type
    if (strstr(pContentString, "image") != NULL) {
        if (!CopyString(pMediaType, mediaTypeLen, "image", (strlen("image") + 1))) {
            return DLNA_RET_FAILURE;
        }
    }
    if (strstr(pContentString, "audio") != NULL) {
        if (!CopyString(pMediaType, mediaTypeLen, "audio", (strlen("audio") + 1))) {
            return DLNA_RET_FAILURE;
        }
    }
    if (strstr(pContentString, "video") != NULL) {
        if (!CopyString(pMediaType, mediaTypeLen, "video", (strlen("video") + 1))) {
            return DLNA_RET_FAILURE;
        }
    }
    if (!CopyString(pMediaType, mediaTypeLen, "unknow", (strlen("unknow") + 1))) {
        return DLNA_RET_FAILURE;
    }

    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::GetMediaTypeFromHttpClient(char *httpRequest, char *pMediaType, size_t mediaTypeLen,
    char *pContentString, int contentStringLen)
{
    char *pFlagPtr = (char *)strcasestr(httpRequest, "CONTENT-TYPE");
    if (pFlagPtr == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "not found content type in head response\r\n");
        return DLNA_RET_FAILURE;
    }
    char *pEndPtr = (char *)strcasestr(pFlagPtr, "\r\n");
    if (pEndPtr == NULL) {
        return DLNA_RET_FAILURE;
    }

    int u32Len = (pEndPtr - pFlagPtr) <= (contentStringLen - 1) ? 0 : (contentStringLen - 1);
    // append Content-Type to pContentString
    if (!CopyString(pContentString, contentStringLen, pFlagPtr, u32Len) ||
        AppendString(pContentString, contentStringLen, ";", strlen(";"))) {
        return DLNA_RET_FAILURE;
    }

    // decide media type by http header: Content-Type
    int ret = GetMediaTypeFromContentString(pMediaType, mediaTypeLen, pContentString, contentStringLen);
    if (ret != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }
    // can seek
    if ((char *)strcasestr(httpRequest, "Accept-Ranges: bytes") != NULL) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "can_seek \n");
        if (!AppendString(pMediaType, mediaTypeLen, "/can_seek", strlen("/can_seek"))) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:strncat_s failed\r\n", __FUNCTION__, __LINE__);
            return DLNA_RET_FAILURE;
        }
    }
    // append content length to pContentString;
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "pMediaType : %{public}s!!!\r\n", pMediaType);
    pFlagPtr = (char *)strcasestr(httpRequest, "CONTENT-LENGTH");
    if (pFlagPtr != NULL) {
        pEndPtr = (char *)strcasestr(pFlagPtr, "\r\n");
        if (pEndPtr == NULL) {
            return DLNA_RET_FAILURE;
        }

        u32Len = pEndPtr - pFlagPtr;
        if (!AppendString(pContentString, contentStringLen, pFlagPtr, u32Len) ||
            !AppendString(pContentString, contentStringLen, ";", strlen(";"))) {
            return DLNA_RET_FAILURE;
        }
    }
    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::CheckSocketAvaliable(const char *host, size_t hostLen, unsigned int u32Port, int *s32Socket)
{
    struct sockaddr_in addr;
    if (hostLen <= 0) {
        return DLNA_RET_FAILURE;
    }

    if (memset_s(&addr, sizeof(struct sockaddr_in), 0, sizeof(struct sockaddr_in)) != EOK) {
        return DLNA_RET_FAILURE;
    }
    inet_aton(host, (struct in_addr *)&addr.sin_addr.s_addr);
    addr.sin_port = htons(u32Port);
    addr.sin_family = AF_INET;
    /* parse compelete */
    *s32Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (*s32Socket < 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "http socket error!!\r\n");
        return DLNA_RET_FAILURE;
    }
    if (connect(*s32Socket, (struct sockaddr *)(&addr), sizeof(struct sockaddr_in)) < 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "http connect error!!\r\n");
        close(*s32Socket);
        return DLNA_RET_FAILURE;
    }
    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::CheckHttpClientSuccess(char *httpRequest, size_t httpRequestLen, int s32Socket)
{
    if (memset_s(httpRequest, httpRequestLen, 0, httpRequestLen) != EOK) {
        close(s32Socket);
        return DLNA_RET_FAILURE;
    }
    int s32ClearTotalLen = httpRequestLen;
    if (HttpReceiveMsg(s32Socket, httpRequest, &s32ClearTotalLen) < 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "HttpReceiveMsg error!!\r\n");
        close(s32Socket);
        return DLNA_RET_FAILURE;
    }

    httpRequest[httpRequestLen - 1] = '\0'; // add '\0' dispose
    if (strstr(httpRequest, "HTTP/1.1 200 OK") == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "get response error!!!\r\n");
        close(s32Socket);
        return DLNA_RET_FAILURE;
    }
    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::GetMediaTypeInHttpHead(const char *pURL, char *pMediaType, size_t mediaTypeLen,
    char *pContentString, size_t contentStringLen)
{
    char httpRequest[HTTPREQUESTLEN];
    UrlPart urlPart;
    int s32Socket;

    if (pURL == NULL || strlen(pURL) == 0) {
        return DLNA_RET_FAILURE;
    }

    /* *parse currentURI */
    char *pFlagPtr = strstr((char *)pURL, "http://");
    if (pFlagPtr == DLNA_NULL_PTR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Failed to parse http head\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }
    // special handle Tencent video"vlive.qqvideo.tc.qq.com";"vkp.tc.qq.com"
    if (strstr(pURL, ".tc.qq.com") != NULL || strstr(pURL, "youku.com") != NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:handle tencent/youku video\r\n", __FUNCTION__, __LINE__);
        if (!CopyString(pMediaType, mediaTypeLen, "unknow/can_seek", strlen("unknow/can_seek"))) {
            return DLNA_RET_FAILURE;
        }
        return DLNA_RET_SUCCESS;
    }
    // for common url: split protocal://host:port/path
    if (SeparateUrl(pURL, &urlPart) == DLNA_RET_FAILURE) {
        return DLNA_RET_FAILURE;
        }
    if (CheckSocketAvaliable(urlPart.host, HTTPIPADDRLEN, urlPart.u32Port, &s32Socket) == DLNA_RET_FAILURE) {
        return DLNA_RET_FAILURE;
        }
    if (InitHttpRequest(httpRequest, HTTPREQUESTLEN, urlPart) == DLNA_RET_FAILURE) {
            goto close_socket;
        }
    if (HttpSendMsg(s32Socket, httpRequest, strlen(httpRequest) + 1) < 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "HttpSendMsg error!!\r\n");
            goto close_socket;
        }
    if (CheckHttpClientSuccess(httpRequest, HTTPREQUESTLEN, s32Socket)  == DLNA_RET_FAILURE) {
        return DLNA_RET_FAILURE;
    }
    if (GetMediaTypeFromHttpClient(httpRequest, pMediaType, mediaTypeLen, pContentString, contentStringLen)
        == DLNA_RET_FAILURE) {
        goto close_socket;
    }
    close(s32Socket);
    return DLNA_RET_SUCCESS;
close_socket:
    close(s32Socket);
    return DLNA_RET_FAILURE;
}

void DmrAVTransport::SleepUntilConditionMet(const int *condition, size_t interval)
{
    size_t time = LOOP_TOTAL_TILES;
    while (*condition == DLNA_FALSE) {
        DLNA_SLEEP(interval);
        time--;
        if (time % LOOP_TOTAL_TILES == 0) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:time:%{public}zu", __FUNCTION__, __LINE__, time);
        }
        if (time == 0) {
            break;
        }
    }
}

void DmrAVTransport::SetMediaData(DmrAvTransportSetMediaURIData dmrAvtSetMediaUriData)
{
    DLNA_LOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
    DLNA_FREE(DlnaCommon::getInstance().GetDmrHdl()->pszPlayURL);
    DlnaCommon::getInstance().GetDmrHdl()->pszPlayURL = dmrAvtSetMediaUriData.pcCurrentURI;
    DLNA_FREE(DlnaCommon::getInstance().GetDmrHdl()->pszTrackMetaData);
    DlnaCommon::getInstance().GetDmrHdl()->pszTrackMetaData = dmrAvtSetMediaUriData.pcCurrentURIMetaData;
    DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));

    if (((strlen(dmrAvtSetMediaUriData.pcCurrentURI)) == 0) &&
        ((strlen(dmrAvtSetMediaUriData.pcCurrentURIMetaData)) == 0)) {
        DLNA_LOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
        DlnaCommon::getInstance().GetDmrHdl()->dmrState = static_cast<int>(DmrAvState::DLNA_DMR_STATE_NO_MEDIA_PRESENT);
        SetCurrentAction(static_cast<int>(DmrAvState::DLNA_DMR_STATE_NO_MEDIA_PRESENT));
        DlnaCommon::getInstance().GetDmrHdl()->setURIactionDone = DLNA_TRUE;
        DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
    } else {
        SetCurrentAction(static_cast<int>(DmrAvState::DLNA_DMR_STATE_STOPPED));
    }

    SleepUntilConditionMet(&(DlnaCommon::getInstance().GetDmrHdl()->setURIactionDone), INTERVAL_TIME);

    DlnaCommon::getInstance().GetDmrHdl()->setURIactionDone = DLNA_FALSE;
}

int DmrAVTransport::SetMediaURI(DmrAvTransportSetMediaURIData dmrAvtSetMediaUriData, DmrActionParam *pstActionEvent)
{
    const struct sockaddr_storage *addrStorage = UpnpActionRequest_get_CtrlPtIPAddr(pstActionEvent->pstActionRequest);
    char tmpIp[INET_ADDRSTRLEN + 1] = {'\0'};
    const char *tmpIpRet = DlnaCommon::getInstance().GetIpBySockaddrStorage(addrStorage, tmpIp, INET_ADDRSTRLEN);
    if (tmpIpRet == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]: get tmpIpt is NULL. \r\n");
    }

    if (DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtSetMediaUri) {
        MediaUrlData mediaUrlData = {(unsigned int)dmrAvtSetMediaUriData.iInstVal,
            (unsigned char *)dmrAvtSetMediaUriData.pcCurrentURI, (unsigned int)strlen(dmrAvtSetMediaUriData.pcCurrentURI), 0,
            (unsigned char *)dmrAvtSetMediaUriData.pcCurrentURIMetaData,
            (unsigned int)strlen(dmrAvtSetMediaUriData.pcCurrentURIMetaData), dmrAvtSetMediaUriData.pMediaType,
            dmrAvtSetMediaUriData.pContentString, dmrAvtSetMediaUriData.pContentStringLen, tmpIp};

        unsigned int ulRetVal = DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtSetMediaUri(mediaUrlData);
        if (ulRetVal != DLNA_RET_SUCCESS) {
            if (ulRetVal == HTTP_RET_OK) {
                DlnaCommon::getInstance().GetDmrHdl()->setURIactionDone = DLNA_TRUE;
            } else if (ulRetVal == HTTP_RET_FILE_TYPE) {
                DlnaCommon::getInstance().GetDmrHdl()->isImage = DLNA_TRUE;
            } else if (ulRetVal == HTTP_RET_UNAUTHORIZED) {
                pstActionEvent->iErrorCode = DLNA_E_UPNP_401;
                pstActionEvent->pErrorStr = (char *)DLNA_E_UPNP_401_STRING;
                return DLNA_RET_FAILURE;
            } else {
                pstActionEvent->iErrorCode = (ulRetVal == DMR_AV_TRANSPORT_ERROR_716) ? DMR_AV_TRANSPORT_ERROR_716 : DLNA_E_UPNP_500;
                pstActionEvent->pErrorStr = (ulRetVal == DMR_AV_TRANSPORT_ERROR_716) ? (char *)DMR_AV_TRANSPORT_ERROR_716_STRING
                    : (char *)DLNA_E_UPNP_500_STRING;
                return DLNA_RET_FAILURE;
            }
        }
    }
    SetMediaData(dmrAvtSetMediaUriData);

    if (DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtGetAlltime) {
        char *pAllTime = DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtGetAlltime();
        pAllTime = pAllTime ? pAllTime : (char *)DMR_DEFAULT_TIME;
        if (!CopyString(DlnaCommon::getInstance().GetDmrHdl()->currentduration, DLNA_BUFF_SIZE, pAllTime, (strlen(pAllTime) + 1))) {
            return DLNA_RET_FAILURE;
        }
    }

    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, (const char *)DLNA_NULL_PTR, (char *)DLNA_NULL_PTR, 0);

    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::CheckInstanceIdValid(int *iInstVal, UpnpActionRequest *pstActionRequest,
    DmrActionParam *pstActionEvent)
{
    *iInstVal = DlnaCommon::getInstance().GetInt(pstActionRequest, INSTANCE_ID);
    if (*iInstVal == DLNA_NULL_INT32) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]: Received Invalid Instance ID with incorrect datatype. \r\n");

        pstActionEvent->iErrorCode = DLNA_E_UPNP_402;
        pstActionEvent->pErrorStr = DLNA_E_UPNP_402_STRING;

        return DLNA_RET_FAILURE;
    }
    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::CheckAvtAvaliable(int iInstVal, DmrActionParam *pstActionEvent)
{
    if (!(IS_DMR_INSTANCE_ID_VALID(iInstVal)) ||
            !(IS_DMR_INSTANCE_ID_SUPPORTED(iInstVal))) { // Only ZERO is valid instance id
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Received Invalid Instance ID = %{public}d.\r\n", __FUNCTION__, __LINE__, iInstVal);

        /* Set the error response return values */
        pstActionEvent->iErrorCode = DMR_AV_TRANSPORT_ERROR_718;
        pstActionEvent->pErrorStr = DMR_AV_TRANSPORT_ERROR_718_STRING;
        return DLNA_RET_FAILURE;
    }
    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::SetTransportURIInit(DmrActionParam *pstActionEvent, int *iInstVal, char **pcCurrentURI,
    char **pcCurrentURIMetaData)
{
    UpnpActionRequest *pstActionRequest = pstActionEvent->pstActionRequest;

    int iRetVal = CheckInstanceIdValid(iInstVal, pstActionRequest, pstActionEvent);
    if (iRetVal != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    const char *iptvFlag = "pushtype=";
    *pcCurrentURI = DlnaCommon::getInstance().GetStr(pstActionRequest, CURRENT_URI);

    iRetVal = CheckAvtAvaliable(*iInstVal, pstActionEvent);
    if (iRetVal != DLNA_RET_SUCCESS) {
        DLNA_FREE(*pcCurrentURI);
        return DLNA_RET_FAILURE;
    }

    if (*pcCurrentURI == DLNA_NULL_PTR) {
        *pcCurrentURI = strdup("");
        DLNA_CHK_RETURN_PRINT((*pcCurrentURI == DLNA_NULL_PTR), DLNA_RET_FAILURE,
                              "[CastEngineDlna][%{public}s][:%{public}d]: strdup failed for CurrentURI.", __FUNCTION__, __LINE__);
    }

    *pcCurrentURIMetaData = DlnaCommon::getInstance().GetStr(pstActionRequest, CURRENT_URI_METADATA);
    if (*pcCurrentURIMetaData == DLNA_NULL_PTR) {
        *pcCurrentURIMetaData = strdup("");
        if (*pcCurrentURIMetaData == DLNA_NULL_PTR) {
            DLNA_FREE(*pcCurrentURI);
            DLNA_CHK_RETURN_PRINT((*pcCurrentURIMetaData == DLNA_NULL_PTR), DLNA_RET_FAILURE,
                "[CastEngineDlna][%{public}s][:%{public}d]: strdup failed for pcCurrentURIMetaData.", __FUNCTION__, __LINE__);
        }
    }

    // the source type is image with the address
    DlnaCommon::getInstance().GetDmrHdl()->isImage = strstr(*pcCurrentURIMetaData, UPNP_IMAGE) ? DLNA_TRUE : DLNA_FALSE;

    /* Giving Indication To the Application
     */
    DlnaCommon::getInstance().GetDmrHdl()->setURIactionDone = DLNA_FALSE;
    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::SetTransportURI(DmrActionParam *pstActionEvent)
{
    char *pcCurrentURI = (char *)DLNA_NULL_PTR;
    char *pcCurrentURIMetaData = (char *)DLNA_NULL_PTR;
    char pMediaType[MEDIA_TYPE_LEN];
    char pContentString[URILEN];
    int iInstVal;
    int iRetVal;

    if (SetTransportURIInit(pstActionEvent, &iInstVal,
        &pcCurrentURI, &pcCurrentURIMetaData) != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    iRetVal = GetMediaTypeInHttpHead(pcCurrentURI, pMediaType, sizeof(pMediaType),
        pContentString, sizeof(pContentString));
    if (iRetVal != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]: get media type Failed iRetVal=%{public}d \r\n", __FUNCTION__, __LINE__, iRetVal);
    }
    DmrAvTransportSetMediaURIData dmrAvtSetMediaUriData = {pcCurrentURI, pcCurrentURIMetaData, iInstVal,
        pMediaType, MEDIA_TYPE_LEN, pContentString, URILEN};
    iRetVal = SetMediaURI(dmrAvtSetMediaUriData, pstActionEvent);
    if (iRetVal != DLNA_RET_SUCCESS) {
		OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "SetMediaURI fail");
        DLNA_FREE(pcCurrentURI);
        DLNA_FREE(pcCurrentURIMetaData);
        return DLNA_RET_FAILURE;
    }

    /* free the previous uri, if any and store the new value */
    DLNA_LOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
    if (((strlen(pcCurrentURI)) != 0) || ((strlen(pcCurrentURIMetaData)) != 0)) {
        /* Set to Stopped In all the cases , Since the New Uri has been set
        No point in continuing with the Old Uri */
        if (DlnaCommon::getInstance().GetDmrHdl()->dmrState == static_cast<int>(DmrAvState::DLNA_DMR_STATE_NO_MEDIA_PRESENT)) {
            DlnaCommon::getInstance().GetDmrHdl()->dmrState = static_cast<int>(DmrAvState::DLNA_DMR_STATE_STOPPED); // can not send here
            SetCurrentAction(static_cast<int>(DmrAvState::DLNA_DMR_STATE_STOPPED));
        } else if (DlnaCommon::getInstance().GetDmrHdl()->dmrState == static_cast<int>(DmrAvState::DLNA_DMR_STATE_PLAYING) ||
                   DlnaCommon::getInstance().GetDmrHdl()->dmrState == static_cast<int>(DmrAvState::DLNA_DMR_STATE_TRANSTIONING)) {
            DlnaCommon::getInstance().GetDmrHdl()->dmrState = static_cast<int>(DmrAvState::DLNA_DMR_STATE_STOPPED); // can not send here
            SetCurrentAction(static_cast<int>(DmrAvState::DLNA_DMR_STATE_STOPPED));
        }
    }

    DMR_SET_AVTRANSPORT_NOTIFY_ALLDATA(DlnaCommon::getInstance().GetDmrHdl()->ulAvtEventNotifySet);
    DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
    DMR_AVTRANSPORT_DO_IMMEDIATE_EVENTING();

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: SetAVTransportURI action is successful\r\n", __FUNCTION__, __LINE__);

    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::PlayForPlaySpeed(DmrActionParam *pstActionEvent,
                               UpnpActionRequest *pstActionRequest)
{
    /* Get The playing Speed given by the Peer (DMC) */
    char *pcPlaySpeed = DlnaCommon::getInstance().GetStr(pstActionRequest, PLAY_SPEED);
    if (pcPlaySpeed == DLNA_NULL_PTR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Failed to get the play speed from the"
                  " received %{public}s action request.\r\n", __FUNCTION__, __LINE__,
                  UpnpString_get_String(UpnpActionRequest_get_ActionName(pstActionRequest)));

        pstActionEvent->iErrorCode = DLNA_E_UPNP_402;
        pstActionEvent->pErrorStr = DLNA_E_UPNP_402_STRING;

        return DLNA_RET_FAILURE;
    }

    int iPlaySpeed = DlnaCommon::getInstance().ConvAscToSInt((unsigned char *)pcPlaySpeed);
    if (iPlaySpeed != 1) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Invalid playspeed = %{public}s received."
                  " Allowed value is 1\r\n", __FUNCTION__, __LINE__, pcPlaySpeed);

        /* Set the error response return values */
        pstActionEvent->iErrorCode = DMR_AV_TRANSPORT_ERROR_717;
        pstActionEvent->pErrorStr = DMR_AV_TRANSPORT_ERROR_717_STRING;

        DLNA_FREE(pcPlaySpeed);
        return DLNA_RET_FAILURE;
    }

    if (DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtPlay) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Entering Callback -  pFnDlnaDmrPlay"
                  " Params - PlaySpeed : %{public}s\r\n", __FUNCTION__, __LINE__, pcPlaySpeed);
        const struct sockaddr_storage *addrStorage
            = UpnpActionRequest_get_CtrlPtIPAddr(pstActionEvent->pstActionRequest);
        char tmpIp[INET_ADDRSTRLEN + 1] = {'\0'};
        const char *tmpIpRet = DlnaCommon::getInstance().GetIpBySockaddrStorage(addrStorage, tmpIp, INET_ADDRSTRLEN);
        if (tmpIpRet == NULL) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]: get tmpIp is NULL. \r\n");
        }
        PlayData data = {pcPlaySpeed, tmpIp};
        unsigned int ulRetVal = DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtPlay(&data);
        if (ulRetVal != DLNA_RET_SUCCESS) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Play Indication to the application failed."
                      " With an error Code--:%{public}d\r\n", __FUNCTION__, __LINE__, ulRetVal);

            pstActionEvent->iErrorCode = DLNA_E_UPNP_500;
            pstActionEvent->pErrorStr = DLNA_E_UPNP_500_STRING;

            /* free the playSpeed Value */
            DLNA_FREE(pcPlaySpeed);
            return DLNA_RET_FAILURE;
        }

        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Exiting from Callback - pFnDlnaDmrPlay \r\n", __FUNCTION__, __LINE__);
    }

    DLNA_FREE(pcPlaySpeed);
    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::Play(DmrActionParam *pstActionEvent)
{
    int iInstVal;
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Peer has invoked the Play action\r\n", __FUNCTION__, __LINE__);

    if (pstActionEvent == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: pstActionEvent is NULL\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }
    UpnpActionRequest *pstActionRequest = pstActionEvent->pstActionRequest;

    if (CheckInstanceIdValid(&iInstVal, pstActionRequest, pstActionEvent) != DLNA_RET_SUCCESS
        || CheckAvtAvaliable(iInstVal, pstActionEvent) != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    if (DlnaCommon::getInstance().GetDmrHdl()->dmrState == static_cast<int>(DmrAvState::DLNA_DMR_STATE_PLAYING)) {
        /* Need To Check Whether We need to return 500 or 701 */
        DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, (const char *)DLNA_NULL_PTR, (char *)DLNA_NULL_PTR, 0);

        return DLNA_RET_SUCCESS;
    }

    int iRetVal = PlayForPlaySpeed(pstActionEvent, pstActionRequest);
    if (iRetVal != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    /* Set The action result that will be sent to the peer */
    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, (const char *)DLNA_NULL_PTR, (char *)DLNA_NULL_PTR, 0);
    /* Set The State to Playing */
    if (DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtGetAlltime) {
        // EV_PLAY_ALLTIME=13,
        char *pAllTime = DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtGetAlltime();
        pAllTime = pAllTime ? pAllTime : (char *)DMR_DEFAULT_TIME;
        if (!CopyString(DlnaCommon::getInstance().GetDmrHdl()->currentduration, DLNA_BUFF_SIZE, pAllTime, strlen(pAllTime) + 1)) {
            return DLNA_RET_FAILURE;
        }
    }

    if (DlnaCommon::getInstance().GetDmrHdl()->dmrState == static_cast<int>(DmrAvState::DLNA_DMR_STATE_STOPPED)) {
        DLNA_LOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
        DlnaCommon::getInstance().GetDmrHdl()->dmrState = static_cast<int>(DmrAvState::DLNA_DMR_STATE_PLAYING);//DLNA_DMR_STATE_TRANSTIONING); // playerActivity need to send playNotify
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d] DMR_SET_AVT_NOTIFY", __FUNCTION__, __LINE__);
        DMR_SET_AVTRANSPORT_NOTIFY_TRANSPORT_STATE(DlnaCommon::getInstance().GetDmrHdl()->ulAvtEventNotifySet);
        DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
        DMR_AVTRANSPORT_DO_IMMEDIATE_EVENTING();
    } else if (DlnaCommon::getInstance().GetDmrHdl()->dmrState == static_cast<int>(DmrAvState::DLNA_DMR_STATE_PAUSED_PLAYBACK)) {
        DLNA_LOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
        DlnaCommon::getInstance().GetDmrHdl()->dmrState = static_cast<int>(DmrAvState::DLNA_DMR_STATE_PLAYING);
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d] DMR_SET_AVT_NOTIFY", __FUNCTION__, __LINE__);
        DMR_SET_AVTRANSPORT_NOTIFY_TRANSPORT_STATE(DlnaCommon::getInstance().GetDmrHdl()->ulAvtEventNotifySet);
        DMR_SET_AVTRANSPORT_NOTIFY_CURTRACKDUR(DlnaCommon::getInstance().GetDmrHdl()->ulAvtEventNotifySet);
        DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
        DMR_AVTRANSPORT_DO_IMMEDIATE_EVENTING();
    }

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Play action is successful\r\n", __FUNCTION__, __LINE__);

    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::CheckActionEvent(int *iInstVal, DmrActionParam *pstActionEvent)
{
    int iRetVal;
    UpnpActionRequest *pstActionRequest = NULL;
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Peer has invoked the Stop action\r\n", __FUNCTION__, __LINE__);

    if (pstActionEvent == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: pstActionEvent is NULL\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }
    pstActionRequest = pstActionEvent->pstActionRequest;

    iRetVal = CheckInstanceIdValid(iInstVal, pstActionRequest, pstActionEvent);
    if (iRetVal != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    iRetVal = CheckAvtAvaliable((*iInstVal), pstActionEvent);
    if (iRetVal != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }
    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::Stop(DmrActionParam *pstActionEvent)
{
    int iInstVal;
    const struct sockaddr_storage *addrStorage = UpnpActionRequest_get_CtrlPtIPAddr(pstActionEvent->pstActionRequest);
    char tmpIp[INET_ADDRSTRLEN + 1] = {'\0'};
    const char *tmpIpRet = DlnaCommon::getInstance().GetIpBySockaddrStorage(addrStorage, tmpIp, INET_ADDRSTRLEN);
    if (tmpIpRet == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]: get tmpIp is NULL. \r\n");
    }

    if (CheckActionEvent(&iInstVal, pstActionEvent) != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    DlnaCommon::getInstance().GetDmrHdl()->stopActionDone = DLNA_FALSE;
    /* Indicate To Application */
    if (DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtStop) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Entering Callback -  pFnDlnaDmrStop Stop"
                  " Indication - iInstVal :%{public}u\r\n", __FUNCTION__, __LINE__, iInstVal);

        StopData data = {(uint32_t)iInstVal, tmpIp};
        unsigned int ulRetVal = DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtStop(&data);
        if (ulRetVal != DLNA_RET_SUCCESS) {
            if (ulRetVal == HTTP_RET_OK) {
                DlnaCommon::getInstance().GetDmrHdl()->stopActionDone = DLNA_TRUE;
            } else {
                OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Stop Indication to the application failed"
                          " With an error Code--:%{public}d\r\n", __FUNCTION__, __LINE__, ulRetVal);

                pstActionEvent->iErrorCode = DLNA_E_UPNP_500;
                pstActionEvent->pErrorStr = DLNA_E_UPNP_500_STRING;

                return DLNA_RET_FAILURE;
            }
        }
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Exiting from Callback -  pFnDlnaDmrStop \r\n", __FUNCTION__, __LINE__);
    }

    SleepUntilConditionMet(&(DlnaCommon::getInstance().GetDmrHdl()->stopActionDone), INTERVAL_TIME);

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "after block DlnaCommon::getInstance().GetDmrHdl()->stopActionDone");
    DlnaCommon::getInstance().GetDmrHdl()->stopActionDone = DLNA_FALSE;
    /* Now Set the result of the Stop action to be sent as response
         to the peer (DMC)
    */
    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, (char *)DLNA_NULL_PTR, (char *)DLNA_NULL_PTR, 0);

    /* Set The State to Stop */
    // state machine
    if (DlnaCommon::getInstance().GetDmrHdl()->ulDeviceInit == DLNA_FALSE) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "%{public}s L%d: dlna stack is deinit, not handle", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }
    DLNA_LOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
    DlnaCommon::getInstance().GetDmrHdl()->dmrState = static_cast<int>(DmrAvState::DLNA_DMR_STATE_STOPPED);
    // here
    SetCurrentAction(static_cast<int>(DmrAvState::DLNA_DMR_STATE_STOPPED));

    DMR_SET_AVTRANSPORT_NOTIFY_TRANSPORT_STATE(DlnaCommon::getInstance().GetDmrHdl()->ulAvtEventNotifySet);
    DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
    DMR_AVTRANSPORT_DO_IMMEDIATE_EVENTING();

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Stop action is successful\r\n", __FUNCTION__, __LINE__);
    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::CheckInstVal(int instVal, DmrActionParam *pstActionEvent)
{
    if (!(IS_DMR_INSTANCE_ID_VALID(instVal)) ||
            !(IS_DMR_INSTANCE_ID_SUPPORTED(instVal))) {
        pstActionEvent->iErrorCode = DMR_AV_TRANSPORT_ERROR_718;
        pstActionEvent->pErrorStr = DMR_AV_TRANSPORT_ERROR_718_STRING;

        return DLNA_RET_FAILURE;
    }
    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::Next(DmrActionParam *pstActionEvent)
{
    int iInstVal;
    UpnpActionRequest *pstActionRequest = NULL;

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Peer has invoked the Next action\r\n", __FUNCTION__, __LINE__);

    if (pstActionEvent == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: pstActionEvent is NULL\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }
    pstActionRequest = pstActionEvent->pstActionRequest;

    iInstVal = DlnaCommon::getInstance().GetInt(pstActionRequest, INSTANCE_ID);
    if (iInstVal == DLNA_NULL_INT32) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]:Received Invalid Instance ID with incorrect datatype in the "
                  "Next action request. \r\n");

        pstActionEvent->iErrorCode = DLNA_E_UPNP_402;
        pstActionEvent->pErrorStr = DLNA_E_UPNP_402_STRING;

        return DLNA_RET_FAILURE;
    }

    if (CheckInstVal(iInstVal, pstActionEvent) != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    /* Now Set the result of the Next action to be sent as response
         to the peer (DMC)
    */
    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, (char *)DLNA_NULL_PTR, (char *)DLNA_NULL_PTR, 0);

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Next action is successful\r\n", __FUNCTION__, __LINE__);

    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::Previous(DmrActionParam *pstActionEvent)
{
    int iInstVal;
    int iRetVal;
    UpnpActionRequest *pstActionRequest = NULL;

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Peer has invoked the Previous action\r\n", __FUNCTION__, __LINE__);

    if (pstActionEvent == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: pstActionEvent is NULL\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }
    pstActionRequest = pstActionEvent->pstActionRequest;

    iRetVal = CheckInstanceIdValid(&iInstVal, pstActionRequest, pstActionEvent);
    if (iRetVal != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    iRetVal = CheckAvtAvaliable(iInstVal, pstActionEvent);
    if (iRetVal != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    /* Now Set the result of the Previous action to be sent as response
         to the peer (DMC)
    */
    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, (char *)DLNA_NULL_PTR, (char *)DLNA_NULL_PTR, 0);

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Previous action is successful\r\n", __FUNCTION__, __LINE__);

    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::PauseStateCheck(DmrActionParam *pstActionEvent)
{
    /* The State of the Media will be handled by the Player ,
        it need not be done in the Stack */
    if ((DlnaCommon::getInstance().GetDmrHdl()->dmrState != static_cast<int>(DmrAvState::DLNA_DMR_STATE_PLAYING)) &&
        (DlnaCommon::getInstance().GetDmrHdl()->dmrState != static_cast<int>(DmrAvState::DLNA_DMR_STATE_RECORDING))) {
        /* Need To Check Whether We need to return 500 or 701 */
        if (DlnaCommon::getInstance().GetDmrHdl()->dmrState == static_cast<int>(DmrAvState::DLNA_DMR_STATE_PAUSED_PLAYBACK)) {
            // slove souhu video push send pause action when in PAUSE state
            OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]:Received Pause action request when DMR state is PAUSED_PLAYBACK");
        } else {
            // DLNA_DMR_STATE_STOP
            // DLNA_DMR_STATE_TRANSTIONING
            // DLNA_DMR_STATE_NO_MEDIA_PRESENT
            pstActionEvent->iErrorCode = DMR_AV_TRANSPORT_ERROR_701;
            pstActionEvent->pErrorStr = DMR_AV_TRANSPORT_ERROR_701_STRING;

            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]:Received Pause action request when DMR state is"
                " not either \"PLAYING\" or \"RECORDING\".Current DMR state = %{public}d\n", DlnaCommon::getInstance().GetDmrHdl()->dmrState);
            return DLNA_RET_FAILURE;
        }
    }

    /* Indicate To Application */
    if (DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtPause) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Entering Callback -  pFnDlnaDmrPause"
                  " Params\r\n", __FUNCTION__, __LINE__);
        const struct sockaddr_storage *addrStorage
            = UpnpActionRequest_get_CtrlPtIPAddr(pstActionEvent->pstActionRequest);
        char tmpIp[INET_ADDRSTRLEN + 1] = {'\0'};
        const char *tmpIpRet = DlnaCommon::getInstance().GetIpBySockaddrStorage(addrStorage, tmpIp, INET_ADDRSTRLEN);
        if (tmpIpRet == NULL) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]: get tmpIp is NULL. \r\n");
        }
        PauseData data = {0, tmpIp};
        unsigned int ulRetVal = DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtPause(&data);
        if (ulRetVal != DLNA_RET_SUCCESS) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Pause Indication to the application failed "
                      " With an error Code--:%{public}d\r\n", __FUNCTION__, __LINE__, ulRetVal);

            /* Set error return values for the response */
            pstActionEvent->iErrorCode = DLNA_E_UPNP_500;
            pstActionEvent->pErrorStr = DLNA_E_UPNP_500_STRING;

            return DLNA_RET_FAILURE;
        }

        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Exiting from Callback-pFnDlnaDmrPause\r\n", __FUNCTION__, __LINE__);
        }
    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::Pause(DmrActionParam *pstActionEvent)
{
    int iInstVal;
    int iRetVal;

    if (CheckActionEvent(&iInstVal, pstActionEvent) != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    iRetVal = PauseStateCheck(pstActionEvent);
    if (iRetVal != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    /* Now Set the result of the Stop action to be sent as response
    to the peer (DMC)     */
    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, (char *)DLNA_NULL_PTR, (char *)DLNA_NULL_PTR, 0);

    DLNA_LOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
    if (DlnaCommon::getInstance().GetDmrHdl()->dmrState == static_cast<int>(DmrAvState::DLNA_DMR_STATE_PLAYING)) {
        DlnaCommon::getInstance().GetDmrHdl()->dmrState = static_cast<int>(DmrAvState::DLNA_DMR_STATE_PAUSED_PLAYBACK);
    } else {
        DlnaCommon::getInstance().GetDmrHdl()->dmrState = static_cast<int>(DmrAvState::DLNA_DMR_STATE_PAUSED_RECORDING);
    }

    // state machine
    DlnaCommon::getInstance().GetDmrHdl()->dmrState =
        static_cast<int>(DmrAvState::DLNA_DMR_STATE_PAUSED_PLAYBACK); // playerActivity need to send pauseNotify? have send pauseNotify here
    SetCurrentAction(static_cast<int>(DmrAvState::DLNA_DMR_STATE_PAUSED_PLAYBACK));

    DMR_SET_AVTRANSPORT_NOTIFY_TRANSPORT_STATE(DlnaCommon::getInstance().GetDmrHdl()->ulAvtEventNotifySet);
    DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
    DMR_AVTRANSPORT_DO_IMMEDIATE_EVENTING();

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Pause action is successful\r\n", __FUNCTION__, __LINE__);

    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::SetCurrPlayState(DlnaDmrAction dmrState)
{
    if (DlnaCommon::getInstance().GetState() != DlnaStateType::DLNA_STATE_ACTIVE) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Stack is not initialized.\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    DLNA_LOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
    switch (dmrState) {
        case DlnaDmrAction::DLNA_DMR_ACTION_PAUSE:

            DlnaCommon::getInstance().GetDmrHdl()->dmrState = static_cast<int>(DmrAvState::DLNA_DMR_STATE_PAUSED_PLAYBACK);
            SetCurrentAction(static_cast<int>(DmrAvState::DLNA_DMR_STATE_PAUSED_PLAYBACK));

            DMR_SET_AVTRANSPORT_NOTIFY_TRANSPORT_STATE(DlnaCommon::getInstance().GetDmrHdl()->ulAvtEventNotifySet);
            DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
            DMR_AVTRANSPORT_DO_IMMEDIATE_EVENTING();
            break;
        case DlnaDmrAction::DLNA_DMR_ACTION_PLAY:
            /* Set The State to Playing */
            DlnaCommon::getInstance().GetDmrHdl()->dmrState = static_cast<int>(DmrAvState::DLNA_DMR_STATE_PLAYING);
            SetCurrentAction(static_cast<int>(DmrAvState::DLNA_DMR_STATE_PLAYING));

            DMR_SET_AVTRANSPORT_NOTIFY_TRANSPORT_STATE(DlnaCommon::getInstance().GetDmrHdl()->ulAvtEventNotifySet);
            DMR_SET_AVTRANSPORT_NOTIFY_CURTRACKDUR(DlnaCommon::getInstance().GetDmrHdl()->ulAvtEventNotifySet);
            DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
            DMR_AVTRANSPORT_DO_IMMEDIATE_EVENTING();
            break;
        case DlnaDmrAction::DLNA_DMR_ACTION_STOP:
            /* Set The State to Stop */
            DlnaCommon::getInstance().GetDmrHdl()->dmrState = static_cast<int>(DmrAvState::DLNA_DMR_STATE_STOPPED);
            SetCurrentAction(static_cast<int>(DmrAvState::DLNA_DMR_STATE_STOPPED));
            DlnaCommon::getInstance().GetDmrHdl()->stopActionDone = DLNA_TRUE;

            DMR_SET_AVTRANSPORT_NOTIFY_TRANSPORT_STATE(DlnaCommon::getInstance().GetDmrHdl()->ulAvtEventNotifySet);
            DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
            DMR_AVTRANSPORT_DO_IMMEDIATE_EVENTING();
            break;
        case DlnaDmrAction::DLNA_DMR_ACTION_OPEN:
            DlnaCommon::getInstance().GetDmrHdl()->setURIactionDone = DLNA_TRUE;

            DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
            break;
		default:
			DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
			break;
    }

    return DLNA_RET_SUCCESS;
}

SeekModeTarget* DmrAVTransport::GetSeekMode(const char *pcMode)
{
    unsigned int ulIndex = DLNA_NULL;

    while (stDMRSeekModes_[ulIndex].pcModeName != DLNA_NULL_PTR) {
        if (!DLNA_STRCMP(stDMRSeekModes_[ulIndex].pcModeName, pcMode)) {
            return &(stDMRSeekModes_[ulIndex]);
        }

        ulIndex++;
    }
    return (SeekModeTarget*)DLNA_NULL_PTR;
}

unsigned int DmrAVTransport::ConvSeekTarget(const char *pcSeekTgt, DlnaParaType enModeDataType,
    DmrSeekTargetVal *puSeekTargetVal)
{
    if (puSeekTargetVal == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: puSeekTargetVal is NULL\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }
    switch (enModeDataType) {
        case DlnaParaType::DLNA_PARA_TYPE_STRING:
            puSeekTargetVal->stStrVal.pucBuf = (unsigned char *)strdup(pcSeekTgt);
            if (!puSeekTargetVal->stStrVal.pucBuf) {
                return DLNA_RET_FAILURE;
            }
            puSeekTargetVal->stStrVal.ulLen = strlen(pcSeekTgt);
            break;

        case DlnaParaType::DLNA_PARA_TYPE_INT32:
            if (sscanf_s(pcSeekTgt, "%9d", &puSeekTargetVal->iVal) != 1) {
                OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:sscanf set iVal error\r\n", __FUNCTION__, __LINE__);
                return DLNA_RET_FAILURE;
            }
            break;

        case DlnaParaType::DLNA_PARA_TYPE_UINT32:
            if (sscanf_s(pcSeekTgt, "%9u", &puSeekTargetVal->ulVal) != 1) {
                OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:sscanf set uVal error\r\n", __FUNCTION__, __LINE__);
                return DLNA_RET_FAILURE;
            }
            break;

        case DlnaParaType::DLNA_PARA_TYPE_FLOAT:
            if (sscanf_s(pcSeekTgt, "%f", &puSeekTargetVal->fVal) != 1) {
                OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:sscanf set fVal error\r\n", __FUNCTION__, __LINE__);
                return DLNA_RET_FAILURE;
            }
            break;
		default:
			break;
    }

    return DLNA_RET_SUCCESS;
}

void DmrAVTransport::FreeSeekTarget(DmrSeekTargetVal *puSeekTargetVal, DlnaParaType enModeDataType)
{
    if (puSeekTargetVal == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: puSeekTargetVal is NULL\r\n", __FUNCTION__, __LINE__);
        return;
    }

    if (enModeDataType == DlnaParaType::DLNA_PARA_TYPE_STRING) {
        DLNA_FREE(puSeekTargetVal->stStrVal.pucBuf);
    }

    return;
}

int DmrAVTransport::CheckSeekMode(DmrActionParam *pstActionEvent,
    UpnpActionRequest *pstActionRequest, SeekModeTarget **pstModeDtls)
{
    /* Get The Seek Duration  Given by the Peer (DMC) */
    char *pcSeekMd = DlnaCommon::getInstance().GetStr(pstActionRequest, SEEK_MODE);
    if (pcSeekMd == DLNA_NULL_PTR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Failed to get the Seek Length from the received %{public}s action request\r\n",
            __FUNCTION__, __LINE__, UpnpString_get_String(UpnpActionRequest_get_ActionName(pstActionRequest)));
        pstActionEvent->iErrorCode = DLNA_E_UPNP_402;
        pstActionEvent->pErrorStr = DLNA_E_UPNP_402_STRING;

        return DLNA_RET_FAILURE;
    }

    *pstModeDtls = GetSeekMode(pcSeekMd);
    if (*pstModeDtls == DLNA_NULL_PTR || (strcmp(pcSeekMd, "ABS_COUNT")) == 0 ||
            (strcmp(pcSeekMd, "REL_COUNT")) == 0 || (strcmp(pcSeekMd, "CHANNEL_FREQ")) == 0 ||
            (strcmp(pcSeekMd, "TAPE-INDEX")) == 0 || (strcmp(pcSeekMd, "FRAME")) == 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Invalid Seek Mode is given in the %{public}s action "
            " request. Received seek mode = %{public}s\r\n", __FUNCTION__, __LINE__,
            UpnpString_get_String(UpnpActionRequest_get_ActionName(pstActionRequest)), pcSeekMd);

        /* Set the error response return values */
        pstActionEvent->iErrorCode = DMR_AV_TRANSPORT_ERROR_710;
        pstActionEvent->pErrorStr = DMR_AV_TRANSPORT_ERROR_710_STRING;

        DLNA_FREE(pcSeekMd);
        return DLNA_RET_FAILURE;
    }

    // so available SEEK_MODE includes REL_TIME and ABS_TIME
    if ((strcmp(pcSeekMd, "ABS_TIME")) == 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Seek Mode in the %{public}s action request. Received seek mode = %{public}s"
            "is also allowable \r\n", __FUNCTION__, __LINE__,
            UpnpString_get_String(UpnpActionRequest_get_ActionName(pstActionRequest)), pcSeekMd);
    }

    DLNA_FREE(pcSeekMd);
    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::CheckSeekTargetInit(const char *pcSeekTgt, const DurationTimeData *timeData,
    const DurationTimeData *durationTimeData, DmrActionParam *pstActionEvent)
{
    if (sscanf_s(pcSeekTgt, "%2d:%2d:%2d", &(timeData->hour), &(timeData->min), &(timeData->sec)) != SCAN_TIME_LEN) {
        pstActionEvent->iErrorCode = DLNA_E_UPNP_402;
        pstActionEvent->pErrorStr = DLNA_E_UPNP_402_STRING;
        return DLNA_RET_FAILURE;
    }

    /* re-get duration */
    if (DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtGetAlltime) {
        char *pAllTime = DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtGetAlltime();
        pAllTime = pAllTime ? pAllTime : (char *)DMR_DEFAULT_TIME;
        if (!CopyString(DlnaCommon::getInstance().GetDmrHdl()->currentduration, DLNA_BUFF_SIZE, pAllTime, strlen(pAllTime) + 1)) {
            pstActionEvent->iErrorCode = DLNA_E_UPNP_402;
            pstActionEvent->pErrorStr = DLNA_E_UPNP_402_STRING;
            return DLNA_RET_FAILURE;
        }
    }

    if (sscanf_s(DlnaCommon::getInstance().GetDmrHdl()->currentduration, "%02d:%02d:%02d", &(durationTimeData->hour),
        &(durationTimeData->min), &(durationTimeData->sec)) != SCAN_TIME_LEN) {
        pstActionEvent->iErrorCode = DLNA_E_UPNP_500;
        pstActionEvent->pErrorStr = DLNA_E_UPNP_500_STRING;
        return DLNA_RET_FAILURE;
    }
    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::CheckSeekTarget(DmrActionParam *pstActionEvent,
    const char *pcSeekTgt, const SeekModeTarget *pstModeDtls)
{
    DmrSeekTargetVal uSeekTargetVal;
    DurationTimeData timeData = {0, 0, 0};
    DurationTimeData durationTimeData = {0, 0, 0};

    if (CheckSeekTargetInit(pcSeekTgt, &timeData, &durationTimeData, pstActionEvent) != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    int totaltime = timeData.hour * DMR_HOUR_TO_SEC + timeData.min * DMR_MIN_TO_SEC + timeData.sec;
    int durationtotaltime = durationTimeData.hour * DMR_HOUR_TO_SEC +
        durationTimeData.min * DMR_MIN_TO_SEC + durationTimeData.sec;

    if (totaltime > durationtotaltime && (DlnaCommon::getInstance().GetDmrHdl()->isImage != DLNA_TRUE) &&
            DlnaCommon::getInstance().GetDmrHdl()->dmrState == static_cast<int>(DmrAvState::DLNA_DMR_STATE_PLAYING)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:seek target larger than duration", __FUNCTION__, __LINE__);

        pstActionEvent->iErrorCode = DLNA_E_UPNP_711;
        pstActionEvent->pErrorStr = DLNA_E_UPNP_711_STRING;
        return DLNA_RET_FAILURE;
    }

    if (ConvSeekTarget(pcSeekTgt, pstModeDtls->eModeDataType, &uSeekTargetVal) != 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Failed To the get the Target File from the received %{public}s action request\r\n",
            __FUNCTION__, __LINE__,
            UpnpString_get_String(UpnpActionRequest_get_ActionName(pstActionEvent->pstActionRequest)));

        pstActionEvent->iErrorCode = DLNA_E_UPNP_711;
        pstActionEvent->pErrorStr = DLNA_E_UPNP_711_STRING;
        return DLNA_RET_FAILURE;
    }

    if ((DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtSeek) != NULL) {
        const struct sockaddr_storage *addrStorage
            = UpnpActionRequest_get_CtrlPtIPAddr(pstActionEvent->pstActionRequest);
        char tmpIp[INET_ADDRSTRLEN + 1] = {'\0'};
        if (DlnaCommon::getInstance().GetIpBySockaddrStorage(addrStorage, tmpIp, INET_ADDRSTRLEN) == NULL) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]: get tmpIp is NULL. \r\n");
        }
        SeekData data = {(AvTransportSeekModeType)(pstModeDtls->iModeTYpe),
            (DlnaParaType)pstModeDtls->eModeDataType, &uSeekTargetVal, tmpIp};
        if (DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtSeek(&data) != DLNA_RET_SUCCESS) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]:Seek Indication to the application failed With an error;\n");
            pstActionEvent->iErrorCode = DLNA_E_UPNP_500;
            pstActionEvent->pErrorStr = DLNA_E_UPNP_500_STRING;
            /* Free the playSpeed Value */
            FreeSeekTarget(&uSeekTargetVal, pstModeDtls->eModeDataType);
            return DLNA_RET_FAILURE;
        }

        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Exiting Callback - pFnDlnaDmrSeek \r\n", __FUNCTION__, __LINE__);
    }

    FreeSeekTarget(&uSeekTargetVal, pstModeDtls->eModeDataType);
    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::Seek(DmrActionParam *pstActionEvent)
{
    int iInstVal;
    SeekModeTarget *pstModeDtls = (SeekModeTarget *)DLNA_NULL_PTR;

    if (pstActionEvent == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: pstActionEvent is NULL\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }
    UpnpActionRequest *pstActionRequest = pstActionEvent->pstActionRequest;

    if (CheckInstanceIdValid(&iInstVal, pstActionRequest, pstActionEvent) != DLNA_RET_SUCCESS
        || CheckAvtAvaliable(iInstVal, pstActionEvent) != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    int iRetVal = CheckSeekMode(pstActionEvent, pstActionRequest, &pstModeDtls);
    if (iRetVal != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    /* Get The Seek Target  Given by the Peer (DMC) */
    char *pcSeekTgt = DlnaCommon::getInstance().GetStr(pstActionRequest, SEEK_TARGET);
    if (pcSeekTgt == DLNA_NULL_PTR) {
        /* Set the error response return values */
        pstActionEvent->iErrorCode = DMR_AV_TRANSPORT_ERROR_711;
        pstActionEvent->pErrorStr = DMR_AV_TRANSPORT_ERROR_711_STRING;

        return DLNA_RET_FAILURE;
    }
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: seek time :%{public}s \r\n ", __FUNCTION__, __LINE__, pcSeekTgt);

    iRetVal = CheckSeekTarget(pstActionEvent, pcSeekTgt, pstModeDtls);
    DLNA_FREE(pcSeekTgt);
    if (iRetVal != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    /* Set The action result that will be sent to the peer */
    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, (const char *)DLNA_NULL_PTR, (const char *)DLNA_NULL_PTR, 0);

    // state machine
    if (DlnaCommon::getInstance().GetDmrHdl()->dmrState == static_cast<int>(DmrAvState::DLNA_DMR_STATE_PLAYING) && (DlnaCommon::getInstance().GetDmrHdl()->isImage != DLNA_TRUE)) {
        if (DlnaCommon::getInstance().GetDmrHdl()->isImage != DLNA_TRUE) {
            DLNA_LOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
            DlnaCommon::getInstance().GetDmrHdl()->dmrState = static_cast<int>(DmrAvState::DLNA_DMR_STATE_TRANSTIONING); // playerActivity need to send playNotify
            // or stopNotify based on last state
            DMR_SET_AVTRANSPORT_NOTIFY_TRANSPORT_STATE(DlnaCommon::getInstance().GetDmrHdl()->ulAvtEventNotifySet);
            DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
        } else {
            DLNA_LOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
            DlnaCommon::getInstance().GetDmrHdl()->dmrState = static_cast<int>(DmrAvState::DLNA_DMR_STATE_PLAYING); // playerActivity need to send playNotify
            // or stopNotify based on last state
            DMR_SET_AVTRANSPORT_NOTIFY_TRANSPORT_STATE(DlnaCommon::getInstance().GetDmrHdl()->ulAvtEventNotifySet);
            DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
        }
        DMR_AVTRANSPORT_DO_IMMEDIATE_EVENTING();
    }

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Seek action is successful! \r\n", __FUNCTION__, __LINE__);
    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::SendTimesByUpnp(DmrActionParam *pstActionEvent, const char *pAllTime, const char *pCurrTime)
{
    if (DlnaCommon::getInstance().GetDmrHdl()->dmrState == static_cast<int>(DmrAvState::DLNA_DMR_STATE_NO_MEDIA_PRESENT) ||
            DlnaCommon::getInstance().GetDmrHdl()->dmrState == static_cast<int>(DmrAvState::DLNA_DMR_STATE_STOPPED)) {
        DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, ABS_TIME, "NOT_IMPLEMENTED", strlen("NOT_IMPLEMENTED"));
    } else {
        if (pCurrTime != NULL && pAllTime != NULL) {
            if (strcmp(pAllTime, DMR_DEFAULT_TIME) == 0) {
                DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, ABS_TIME, DMR_DEFAULT_TIME, strlen(DMR_DEFAULT_TIME));
            } else {
                DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, ABS_TIME, pCurrTime, strlen(pCurrTime));
            }
        } else {
            DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, ABS_TIME, DMR_DEFAULT_TIME, strlen(DMR_DEFAULT_TIME));
        }
    }

    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, REL_COUNT, "2147483647", strlen("2147483647"));
    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, ABS_COUNT, "2147483647", strlen("2147483647"));

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: GetPositionInfo action is successful\r\n", __FUNCTION__, __LINE__);
    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::GetPositionInfoSendTimes(DmrActionParam *pstActionEvent, const char *pAllTime)
{
    char *pCurrTime = NULL;
    if (DlnaCommon::getInstance().GetDmrHdl()->dmrState == static_cast<int>(DmrAvState::DLNA_DMR_STATE_NO_MEDIA_PRESENT)) {
        DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, REL_TIME, DMR_DEFAULT_TIME, strlen(DMR_DEFAULT_TIME));
        return SendTimesByUpnp(pstActionEvent, pAllTime, pCurrTime);
    }
    if (DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtGetCurrtime) {
        pCurrTime = DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtGetCurrtime();
    }

    if (pCurrTime != NULL && pAllTime != NULL) {
        if (strcmp(pAllTime, DMR_DEFAULT_TIME) == 0) {
            DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, REL_TIME, DMR_DEFAULT_TIME, strlen(DMR_DEFAULT_TIME));
        } else {
            DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, REL_TIME, pCurrTime, strlen(pCurrTime));
        }
        return SendTimesByUpnp(pstActionEvent, pAllTime, pCurrTime);
    }
    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, REL_TIME, DMR_DEFAULT_TIME, strlen(DMR_DEFAULT_TIME));
    return SendTimesByUpnp(pstActionEvent, pAllTime, pCurrTime);
}

int DmrAVTransport::GetPositionInfo(DmrActionParam *pstActionEvent)
{
    UpnpActionRequest *pstActionRequest = NULL;
    char *pAllTime = NULL;
    int iInstVal;

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Peer has invoked the GetPositionInfo action\r\n", __FUNCTION__, __LINE__);

    if (pstActionEvent == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: pstActionEvent is NULL\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    pstActionRequest = pstActionEvent->pstActionRequest;

    if (CheckInstanceIdValid(&iInstVal, pstActionRequest, pstActionEvent) != DLNA_RET_SUCCESS
        || CheckAvtAvaliable(iInstVal, pstActionEvent) != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    bool noMediaPresent = DlnaCommon::getInstance().GetDmrHdl()->dmrState == static_cast<int>(DmrAvState::DLNA_DMR_STATE_NO_MEDIA_PRESENT);
    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, TRACK, noMediaPresent ? "0" : "1", 1);

    if (noMediaPresent) {
        DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, TRACK_DURATION, DMR_DEFAULT_TIME, strlen(DMR_DEFAULT_TIME));
    } else {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "after block DlnaCommon::getInstance().GetDmrHdl()->dmrstate");
        if ((DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtGetAlltime) != NULL) {
            pAllTime = DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtGetAlltime();
        }
        if (pAllTime != NULL) {
            DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, TRACK_DURATION, pAllTime, strlen(pAllTime));

            DLNA_LOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
            DMR_SET_AVTRANSPORT_NOTIFY_CURTRACKDUR(DlnaCommon::getInstance().GetDmrHdl()->ulAvtEventNotifySet);
            DLNA_UNLOCK(&(DlnaCommon::getInstance().GetDmrHdl()->dmrAVTVariableMutex));
            DMR_AVTRANSPORT_DO_IMMEDIATE_EVENTING();
        } else {
            DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, TRACK_DURATION, DMR_DEFAULT_TIME, strlen(DMR_DEFAULT_TIME));
        }
    }

    if (DlnaCommon::getInstance().GetDmrHdl()->pszTrackMetaData == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: DlnaCommon::getInstance().GetDmrHdl()->pszTrackMetaData == null", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }
        DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, TRACK_METADATA, DlnaCommon::getInstance().GetDmrHdl()->pszTrackMetaData,
            strlen(DlnaCommon::getInstance().GetDmrHdl()->pszTrackMetaData));

    if (DlnaCommon::getInstance().GetDmrHdl()->pszPlayURL == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: DlnaCommon::getInstance().GetDmrHdl()->pszPlayURL == null", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }
    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, TRACK_URI, DlnaCommon::getInstance().GetDmrHdl()->pszPlayURL,
        strlen(DlnaCommon::getInstance().GetDmrHdl()->pszPlayURL));
    return GetPositionInfoSendTimes(pstActionEvent, (const char *)pAllTime);
}


int DmrAVTransport::GetTransportInfo(DmrActionParam *pstActionEvent)
{
    UpnpActionRequest *pstActionRequest = NULL;
    int iInstVal;
    int iRetVal;

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Peer has invoked the GetTransportInfo"
        " action\r\n", __FUNCTION__, __LINE__);

    if (pstActionEvent == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: pstActionEvent is NULL\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    pstActionRequest = pstActionEvent->pstActionRequest;

    iRetVal = CheckInstanceIdValid(&iInstVal, pstActionRequest, pstActionEvent);
    if (iRetVal != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    iRetVal = CheckAvtAvaliable(iInstVal, pstActionEvent);
    if (iRetVal != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, CURRENT_TRANSPORT_STATE,
        szDMRStatus_[DlnaCommon::getInstance().GetDmrHdl()->dmrState], strlen(szDMRStatus_[DlnaCommon::getInstance().GetDmrHdl()->dmrState]));
    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, CURRENT_TRANSPORT_STATUS, "OK", strlen("OK"));
    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, CURRENT_SPEED, "1", strlen("1"));

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: GetTransportInfo action is successful\r\n", __FUNCTION__, __LINE__);

    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::GetTransportSettings(DmrActionParam *pstActionEvent)
{
    UpnpActionRequest *pstActionRequest = NULL;
    int iInstVal;

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Peer has invoked the GetTransportSettings action\r\n", __FUNCTION__, __LINE__);

    if (pstActionEvent == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: pstActionEvent is NULL\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    pstActionRequest = pstActionEvent->pstActionRequest;

    iInstVal = DlnaCommon::getInstance().GetInt(pstActionRequest, INSTANCE_ID);
    if (iInstVal == DLNA_NULL_INT32) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]:Received Invalid Instance ID with incorrect datatype in the "
                  "GetTransportSettings action request. \r\n");

        pstActionEvent->iErrorCode = DLNA_E_UPNP_402;
        pstActionEvent->pErrorStr = DLNA_E_UPNP_402_STRING;

        return DLNA_RET_FAILURE;
    }

    if (CheckInstVal(iInstVal, pstActionEvent) != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, PLAY_MODE, "NORMAL", strlen("NORMAL"));
    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, REC_QUALITY_MODE, "NOT_IMPLEMENTED", strlen("NOT_IMPLEMENTED"));

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: GetTransportSettings action is successful\r\n", __FUNCTION__, __LINE__);

    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::FillMediaInfoIsPresent(DmrActionParam *pstActionEvent, bool noMediaPresent)
{
    if (DlnaCommon::getInstance().GetDmrHdl()->pszPlayURL == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: DlnaCommon::getInstance().GetDmrHdl()->pszPlayURL == null", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    } else {
        DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, CURRENT_URI, DlnaCommon::getInstance().GetDmrHdl()->pszPlayURL,
            strlen(DlnaCommon::getInstance().GetDmrHdl()->pszPlayURL));
    }

    if (DlnaCommon::getInstance().GetDmrHdl()->pszTrackMetaData == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: DlnaCommon::getInstance().GetDmrHdl()->pszTrackMetaData == null", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    } else {
        DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, CURRENT_URI_METADATA, DlnaCommon::getInstance().GetDmrHdl()->pszTrackMetaData,
            strlen(DlnaCommon::getInstance().GetDmrHdl()->pszTrackMetaData));
    }

    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, NEXT_URI, "NOT_IMPLEMENTED", strlen("NOT_IMPLEMENTED"));
    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, NEXT_URI_METADATA, "NOT_IMPLEMENTED", strlen("NOT_IMPLEMENTED"));
    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, PLAY_MEDIUM,
        noMediaPresent ? "NONE" : "NETWORK",
        noMediaPresent ? strlen("NONE") : strlen("NETWORK"));

    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, RECORD_MEDIUM, "NOT_IMPLEMENTED", strlen("NOT_IMPLEMENTED"));
    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, WRITE_STATUS, "NOT_IMPLEMENTED", strlen("NOT_IMPLEMENTED"));
    return DLNA_RET_SUCCESS;
}

int DmrAVTransport::FillMediaInfo(DmrActionParam *pstActionEvent)
{
    char *pAllTime = NULL;

    if (pstActionEvent == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: pstActionEvent is NULL\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    bool noMediaPresent = DlnaCommon::getInstance().GetDmrHdl()->dmrState == static_cast<int>(DmrAvState::DLNA_DMR_STATE_NO_MEDIA_PRESENT);
    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, NR_TRACKS, noMediaPresent ? "0" : "1", 1);

    if (noMediaPresent || DlnaCommon::getInstance().GetDmrHdl()->dmrState == static_cast<int>(DmrAvState::DLNA_DMR_STATE_STOPPED)) {
        DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, MEDIA_DURATION, "NOT_IMPLEMENTED", strlen("NOT_IMPLEMENTED"));
        return FillMediaInfoIsPresent(pstActionEvent, noMediaPresent);
    }
    if (DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtGetAlltime) {
        pAllTime = DlnaCommon::getInstance().GetDmrHdl()->pFnDmrAvtGetAlltime();
    }
    if (pAllTime == NULL) {
        DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, MEDIA_DURATION, DMR_DEFAULT_TIME, strlen(DMR_DEFAULT_TIME));
        return FillMediaInfoIsPresent(pstActionEvent, noMediaPresent);
    }
    if (strcmp(pAllTime, DMR_DEFAULT_TIME2) == 0 || strcmp(pAllTime, DMR_DEFAULT_TIME) == 0) {
        DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, MEDIA_DURATION, DMR_DEFAULT_TIME,
            strlen(DMR_DEFAULT_TIME));
        return FillMediaInfoIsPresent(pstActionEvent, noMediaPresent);
    }
    DLNA_ADD_PARAM_RETURN_FAILED(pstActionEvent, MEDIA_DURATION, pAllTime, strlen(pAllTime));

    return FillMediaInfoIsPresent(pstActionEvent, noMediaPresent);
}
}
}
}
}