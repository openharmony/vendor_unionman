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

#ifndef DMR_AV_TRANSPORT_H
#define DMR_AV_TRANSPORT_H

#include "dlna_common.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

/* AVTRANSPORT ERROR CODE */
inline constexpr int DMR_AV_TRANSPORT_ERROR_701 = 701;
inline constexpr int DMR_AV_TRANSPORT_ERROR_702 = 702;
inline constexpr int DMR_AV_TRANSPORT_ERROR_703 = 703;
inline constexpr int DMR_AV_TRANSPORT_ERROR_704 = 704;
inline constexpr int DMR_AV_TRANSPORT_ERROR_705 = 705;
inline constexpr int DMR_AV_TRANSPORT_ERROR_706 = 706;
inline constexpr int DMR_AV_TRANSPORT_ERROR_707 = 707;
inline constexpr int DMR_AV_TRANSPORT_ERROR_708 = 708;
inline constexpr int DMR_AV_TRANSPORT_ERROR_709 = 709;
inline constexpr int DMR_AV_TRANSPORT_ERROR_710 = 710;
inline constexpr int DMR_AV_TRANSPORT_ERROR_711 = 711;
inline constexpr int DMR_AV_TRANSPORT_ERROR_712 = 712;
inline constexpr int DMR_AV_TRANSPORT_ERROR_713 = 713;
inline constexpr int DMR_AV_TRANSPORT_ERROR_714 = 714;
inline constexpr int DMR_AV_TRANSPORT_ERROR_715 = 715;
inline constexpr int DMR_AV_TRANSPORT_ERROR_716 = 716;
inline constexpr int DMR_AV_TRANSPORT_ERROR_717 = 717;
inline constexpr int DMR_AV_TRANSPORT_ERROR_718 = 718;
inline constexpr int DMR_AV_TRANSPORT_ERROR_719 = 719;
inline constexpr int DMR_AV_TRANSPORT_ERROR_720 = 720;
inline constexpr int DMR_AV_TRANSPORT_ERROR_721 = 721;
inline constexpr int DMR_AV_TRANSPORT_ERROR_722 = 722;
inline constexpr int DMR_AV_TRANSPORT_ERROR_723 = 723;
inline constexpr int DMR_AV_TRANSPORT_ERROR_724 = 724;
inline constexpr int DMR_AV_TRANSPORT_ERROR_725 = 725;
inline constexpr int DMR_AV_TRANSPORT_ERROR_726 = 726;
inline constexpr int DMR_AV_TRANSPORT_ERROR_727 = 727;
inline constexpr int DMR_AV_TRANSPORT_ERROR_728 = 728;
inline constexpr int DMR_AV_TRANSPORT_ERROR_729 = 729;
inline constexpr int DMR_AV_TRANSPORT_ERROR_730 = 730;
/* AVTRANSPORT ERROR STRING */
inline constexpr char *DMR_AV_TRANSPORT_ERROR_701_STRING = "Transition not available";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_702_STRING = "No contents";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_703_STRING = "Read error";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_704_STRING = "Format not supported for playback";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_705_STRING = "Transport is locked";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_706_STRING = "Write error";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_707_STRING = "Media is protected or not writable";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_708_STRING = "Format not supported for recording";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_709_STRING = "Media is full";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_710_STRING = "Seek mode not supported";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_711_STRING = "Illegal seek target";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_712_STRING = "Play mode not supported";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_713_STRING = "Record quality not supported";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_714_STRING = "Illegal MIME-type";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_715_STRING = "Content BUSY";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_716_STRING = "Resource not found";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_717_STRING = "Play speed not supported";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_718_STRING = "Invalid InstanceID";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_719_STRING = "DRM error";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_720_STRING = "Expired content";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_721_STRING = "Non-allowed use";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_722_STRING = "Can??t determine allowed uses";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_723_STRING = "Exhausted allowed use";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_724_STRING = "Device authentication failure";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_725_STRING = "Device revocation";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_726_STRING = "Invalid StateVariableList";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_727_STRING = "Ill-formed CSV List";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_728_STRING = "Invalid State Variable Value";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_729_STRING = "Invalid Service Type";
inline constexpr char *DMR_AV_TRANSPORT_ERROR_730_STRING = "Invalid Service Id";
inline constexpr char *DMR_AV_TRANSPORT_SERVICE_ID = "urn:upnp-org:serviceId:AVTransport";
inline constexpr char *DMR_AV_TRANSPORT_SERVICE_TYPE = "urn:schemas-upnp-org:service:AVTransport:1";
inline constexpr char *DMR_AV_TRANSPORT_ACTION_SET_AVTRANSPORT_URI = "SetAVTransportURI";
inline constexpr char *DMR_AV_TRANSPORT_ACTION_PLAY = "Play";
inline constexpr char *DMR_AV_TRANSPORT_ACTION_STOP = "Stop";
inline constexpr char *DMR_AV_TRANSPORT_ACTION_PAUSE = "Pause";
inline constexpr char *DMR_AV_TRANSPORT_ACTION_GET_POSITION_INFO = "GetPositionInfo";
inline constexpr char *DMR_AV_TRANSPORT_ACTION_SEEK = "Seek";
inline constexpr char *DMR_AV_TRANSPORT_ACTION_GET_TRANSPORT_SETTINGS = "GetTransportSettings";
inline constexpr char *DMR_AV_TRANSPORT_ACTION_GET_MEDIA_INFO = "GetMediaInfo";
inline constexpr char *DMR_AV_TRANSPORT_ACTION_GET_TRANSPORT_INFO = "GetTransportInfo";
inline constexpr char *DMR_AV_TRANSPORT_ACTION_GET_DEVICE_CAPABILITIES = "GetDeviceCapabilities";
inline constexpr char *DMR_AV_TRANSPORT_ACTION_NEXT = "Next";
inline constexpr char *DMR_AV_TRANSPORT_ACTION_PREVIOUS = "Previous";
inline constexpr char *DMR_AV_TRANSPORT_ACTION_GET_TRANSPORT_ACTION = "GetCurrentTransportActions";
inline constexpr char *DLNA_DMR_CURRENTACTION_STOP_IMAGE = "Play,Stop,Pause,Seek,Next,Previous";
inline constexpr char *DLNA_DMR_CURRENTACTION_PLAY_IMAGE = "Play,Stop,Pause,Pause,Seek,Next,Previous";
inline constexpr char *DLNA_DMR_CURRENTACTION_PAUSE_IMAGE = "Play,Stop,Seek,Next,Previous";
inline constexpr char *DLNA_DMR_CURRENTACTION_STOP = "Play,Stop,Seek,X_DLNA_SeekTime,Next,Previous";
inline constexpr char *DLNA_DMR_CURRENTACTION_PLAY = "Play,Stop,Pause,Seek,X_DLNA_SeekTime,Next,Previous";
inline constexpr char *DLNA_DMR_CURRENTACTION_PAUSE = "Play,Stop,Seek,X_DLNA_SeekTime,Next,Previous";
inline constexpr char *DMR_DEFAULT_TIME = "00:00:00";
inline constexpr char *DMR_DEFAULT_TIME2 = "0:00:00";
inline constexpr char *CURRENT_URI = "CurrentURI";
inline constexpr char *CURRENT_URI_METADATA = "CurrentURIMetaData";
inline constexpr char *INSTANCE_ID = "InstanceID";
inline constexpr char *CHANNEL = "Channel";
inline constexpr char *DESIREDVOLUME = "DesiredVolume";
inline constexpr char *DESIREDMUTE = "DesiredMute";
inline constexpr char *TRACK = "Track";
inline constexpr char *TRACK_DURATION = "TrackDuration";
inline constexpr char *TRACK_METADATA = "TrackMetaData";
inline constexpr char *TRACK_URI = "TrackURI";
inline constexpr char *REL_TIME = "RelTime";
inline constexpr char *ABS_TIME = "AbsTime";
inline constexpr char *REL_COUNT = "RelCount";
inline constexpr char *ABS_COUNT = "AbsCount";
inline constexpr char *PLAY_SPEED = "Speed";
inline constexpr char *SEEK_MODE = "Unit";
inline constexpr char *SEEK_TARGET = "Target";
inline constexpr char *NR_TRACKS = "NrTracks";
inline constexpr char *CURRENT_TYPE = "CurrentType";
inline constexpr char *MEDIA_DURATION = "MediaDuration";
inline constexpr char *META_DATA = "MetaData";
inline constexpr char *NEXT_URI = "NextURI";
inline constexpr char *NEXT_URI_METADATA = "NextURIMetaData";
inline constexpr char *PLAY_MEDIUM = "PlayMedium";
inline constexpr char *RECORD_MEDIUM = "RecordMedium";
inline constexpr char *WRITE_STATUS = "WriteStatus";
inline constexpr char *CURRENT_TRANSPORT_STATE = "CurrentTransportState";
inline constexpr char *CURRENT_TRANSPORT_STATUS = "CurrentTransportStatus";
inline constexpr char *CURRENT_SPEED = "CurrentSpeed";
inline constexpr char *PLAY_MODE = "PlayMode";
inline constexpr char *REC_QUALITY_MODE = "RecQualityMode";
inline constexpr char *PLAY_MEDIA = "PlayMedia";
inline constexpr char *RECORD_MEDIA = "RecMedia";
inline constexpr char *RECORD_QUALITY_MODES = "RecQualityModes";

inline constexpr int DMR_HOUR_TO_SEC = 3600;;
inline constexpr int DMR_MIN_TO_SEC = 60;
inline constexpr int DMR_AV_TRANSPORT_ACTION_SEEK_MSG_LEN = 64;
inline constexpr int LOOP_TOTAL_TILES = 200;
inline constexpr int LOOP_ITEM_TILES = 100;
inline constexpr int HTTP_RET_OK = 200;
inline constexpr int HTTP_RET_FILE_TYPE = 206;
inline constexpr int HTTP_RET_UNAUTHORIZED = 401;
inline constexpr int MEDIA_TYPE_LEN = 16;
inline constexpr int SCAN_TIME_LEN = 3;
inline constexpr int EV_PLAY_CURTIME = 12;
inline constexpr int EV_PLAY_ALLTIME = 13;
inline constexpr int INTERVAL_TIME = 5;

#define DMR_SET_AVTRANSPORT_NOTIFY_TRANSPORT_STATE(NotifyBitSet) ((NotifyBitSet) = (NotifyBitSet) | 0x01)

#define DMR_AVTRANSPORT_IS_TRANSPORT_STATE_TO_NOTIFY(NotifyBitSet) ((NotifyBitSet) & 0x01)

#define DMR_UNSET_AVTRANSPORT_NOTIFY_TRANSPORT_STATE(NotifyBitSet) ((NotifyBitSet) = (NotifyBitSet) | ~(0x01))

#define DMR_SET_AVTRANSPORT_NOTIFY_CURTRACKURI(NotifyBitSet) ((NotifyBitSet) = (NotifyBitSet) | 0x02)

#define DMR_AVTRANSPORT_IS_CURTRACKURI_TO_NOTIFY(NotifyBitSet) ((NotifyBitSet) & 0x02)

#define DMR_UNSET_AVTRANSPORT_NOTIFY_CURTRACKURI(NotifyBitSet) ((NotifyBitSet) = (NotifyBitSet) | ~(0x02))

#define DMR_SET_AVTRANSPORT_NOTIFY_CURTRACKMETADATA(NotifyBitSet) ((NotifyBitSet) = (NotifyBitSet) | (0x04))

#define DMR_AVTRANSPORT_IS_CURTRACKMETADATA_TO_NOTIFY(NotifyBitSet) ((NotifyBitSet) & 0x04)

#define DMR_UNSET_AVTRANSPORT_NOTIFY_CURTRACKMETADATA(NotifyBitSet) ((NotifyBitSet) = (NotifyBitSet) | ~(0x04))

#define DMR_SET_AVTRANSPORT_NOTIFY_CURTRACKDUR(NotifyBitSet) ((NotifyBitSet) = (NotifyBitSet) | (0x08))

#define DMR_AVTRANSPORT_IS_CURTRACKDUR_TO_NOTIFY(NotifyBitSet) ((NotifyBitSet) & 0x08)

#define DMR_SET_AVTRANSPORT_NOTIFY_ALLDATA(NotifyBitSet) ((NotifyBitSet) = (NotifyBitSet) | (0xFF))


#define DMR_AVTRANSPORT_IS_ANY_STATE_VAR_TOBE_NOTIFIED(NotifyBitSet)                                                 \
    (DMR_AVTRANSPORT_IS_TRANSPORT_STATE_TO_NOTIFY(NotifyBitSet) || DMR_AVTRANSPORT_IS_CURTRACKURI_TO_NOTIFY(NotifyBitSet) || \
        DMR_AVTRANSPORT_IS_CURTRACKMETADATA_TO_NOTIFY(NotifyBitSet) || DMR_AVTRANSPORT_IS_CURTRACKDUR_TO_NOTIFY(NotifyBitSet))

#define DLNADMR_AVTRANSPORT_START_TAG(pEventBuff, startTag) DlnaBufferManager::StrBuffAppendFormatStr(pEventBuff, "<%s>\r\n", startTag);

#define DLNADMR_AVTRANSPORT_END_TAG(pEventBuff, EndTags) DlnaBufferManager::StrBuffAppendFormatStr(pEventBuff, "</%s>\r\n", EndTags);

#define DLNADMR_AVTRANSPORT_ESCAPED_OPEN_STARTTAG(pEventBuff, tagName) \
    DlnaBufferManager::StrBuffAppendFormatStr(pEventBuff, "&lt;%s ", tagName);

#define DLNADMR_AVTRANSPORT_ESCAPED_END_TAG(pEventBuff, tagName) \
    DlnaBufferManager::StrBuffAppendFormatStr(pEventBuff, "&lt;/%s&gt;\r\n", tagName);

#define DLNADMR_AVTRANSPORT_WRITE_ATTRIBUTE_NAME(pEventBuff, attrName) DlnaBufferManager::StrBuffAppendFormatStr(pEventBuff, "%s=", attrName);

#define DLNADMR_AVTRANSPORT_WRITE_ATTR_STANDARD_VAL(pEventBuff, attrVal) \
    DlnaBufferManager::StrBuffAppendFormatStr(pEventBuff, "\"%s\" ", attrVal);

#define DLNADMR_AVTRANSPORT_WRITE_LASTATTR_STANDARD_VAL(pEventBuff, attrVal) \
    DlnaBufferManager::StrBuffAppendFormatStr(pEventBuff, "\"%s\"", attrVal);

#define DLNADMR_AVTRANSPORT_ESCAPED_CLOSE_STARTTAG(pEventBuff) DlnaBufferManager::StrBuffAppendFormatStr(pEventBuff, "&gt;\r\n");

#define DLNADMR_AVTRANSPORT_EMPTY_ELEMENT_WITH_ATTRIBUTE(pEventBuff, elmName, attrName, attrValue) \
    DlnaBufferManager::StrBuffAppendFormatStr(pEventBuff, "&lt;%s %s=\"%s\"/&gt;\r\n", elmName, attrName, attrValue);

#define DLNA_DMR_AVTRANSPORT_AUTO_NOTIFY_TIME 210

#define DMR_AVTRANSPORT_DO_IMMEDIATE_EVENTING() do {   \
    if (DlnaCommon::getInstance().GetDmrHdl()->bSendQuickNotify) { \
        DmrAVTransport::DoImmediateEventing();       \
    }                                          \
} while (0)

#define DLNA_ADD_PARAM_RETURN_FAILED(event, key, value, len) do {  \
    int retVal = DlnaCommon::getInstance().AddParamToActionResponse(event, key, value, len);  \
    if (retVal != 0) {                                                      \
        return (retVal);                                                \
    }                                                                   \
} while (0)

struct SeekModeTarget{
    char *pcModeName;
    int iModeTYpe;
    DlnaParaType eModeDataType;
};

struct DurationTimeData{
    int hour;
    int min;
    int sec;
};

struct UrlPart{
    char host[HTTPIPADDRLEN];
    char path[URILEN];
    unsigned int u32Port;
};

struct DmrAvTransportSetMediaURIData{
    char *pcCurrentURI;
    char *pcCurrentURIMetaData;
    int iInstVal;
    char *pMediaType;
    int pMediaTypeLen;
    char *pContentString;
    int pContentStringLen;
};

class DmrAVTransport{

public:
	static DmrAVTransport &getInstance();
	DmrServiceAction* getActionFuncTable();
	int SetCurrPlayState(DlnaDmrAction dmrState);
	void CreateAvtNotifyResponse(int isInitialNotify, void *pstEventRespBuf);

private:
	static void FreeSeekTarget(DmrSeekTargetVal *puSeekTargetVal, DlnaParaType enModeDataType);
	static unsigned int ConvSeekTarget(const char *pcSeekTgt, DlnaParaType enModeDataType,
	    DmrSeekTargetVal *puSeekTargetVal);
	static void CreateAvtOptNotifyContents(int isInitialNotify, void *pstEventRespBuf);
	static void EventNotifyTimeout(void *pvArgument);
	static void CopyWithEscape(void *buf, const char *pBuf);
	
	static DOMString IxmlDoXmlEscaping(const char *value);
	
	static void StartNotifyTimer();
	
	static void AddStateVarForInitialNotify(void *pstEventRespBuf);
	
	static void CreateTransportStateToNotify(void *pstEventRespBuf);
	
	static void CreateCURTRACKURIToNotify(void *pstEventRespBuf);
	
	static void CreateAvtOptNotifyContentsData(int isInitialNotify, void *pstEventRespBuf);
	
	static void SetCurrentActionWithStopped();
	
	static void SetCurrentActionWithPlaying();
	
	static void SetCurrentActionWithPresent();
	
	static void NoMediaWithPresent();

	static void SetCurrentAction(int state);

	static int HttpReceiveMsg(int s32Socket, char *pchMsgBuf, int *u32MsgLen);
	static int HttpSendMsg(int s32Socket, char *pchMsgBuf, int s32MsgLen);
	static bool AppendString(char *dest, size_t destMax, const char *src, size_t count);
	static bool CopyString(char *dest, size_t destMax, const char *src, size_t count);
	static int SeparateUrl(const char *pUrl, UrlPart *urlPart);
	static int InitHttpRequest(char *httpRequest, int httpRequestLen, UrlPart urlPart);
	static int GetMediaTypeFromContentString(char *pMediaType, size_t mediaTypeLen,char *pContentString, size_t contentStringLen);
	static int GetMediaTypeFromHttpClient(char *httpRequest, char *pMediaType, size_t mediaTypeLen,char *pContentString, int contentStringLen);
	static int CheckSocketAvaliable(const char *host, size_t hostLen, unsigned int u32Port, int *s32Socket);
	static int CheckHttpClientSuccess(char *httpRequest, size_t httpRequestLen, int s32Socket);
	static int GetMediaTypeInHttpHead(const char *pURL, char *pMediaType, size_t mediaTypeLen,char *pContentString, size_t contentStringLen);

	static void SleepUntilConditionMet(const int *condition, size_t interval);
	static void SetMediaData(DmrAvTransportSetMediaURIData dmrAvtSetMediaUriData);
	static int SetMediaURI(DmrAvTransportSetMediaURIData dmrAvtSetMediaUriData, DmrActionParam *pstActionEvent);
	static int CheckInstanceIdValid(int *iInstVal, UpnpActionRequest *pstActionRequest,DmrActionParam *pstActionEvent);
	static int CheckAvtAvaliable(int iInstVal, DmrActionParam *pstActionEvent);
	static int SetTransportURIInit(DmrActionParam *pstActionEvent, int *iInstVal, char **pcCurrentURI,char **pcCurrentURIMetaData);
	static int SetTransportURI(DmrActionParam *pstActionEvent);
	static int PlayForPlaySpeed(DmrActionParam *pstActionEvent,UpnpActionRequest *pstActionRequest);	
	static int Play(DmrActionParam *pstActionEvent);
	
	static int CheckActionEvent(int *iInstVal, DmrActionParam *pstActionEvent);
	static int Stop(DmrActionParam *pstActionEvent);
	static int CheckInstVal(int instVal, DmrActionParam *pstActionEvent);
	static int Next(DmrActionParam *pstActionEvent);
	static int Previous(DmrActionParam *pstActionEvent);
	static int PauseStateCheck(DmrActionParam *pstActionEvent);
	static int Pause(DmrActionParam *pstActionEvent);
	static SeekModeTarget* GetSeekMode(const char *pcMode);
	static int CheckSeekMode(DmrActionParam *pstActionEvent,UpnpActionRequest *pstActionRequest, SeekModeTarget **pstModeDtls);

	static int CheckSeekTargetInit(const char *pcSeekTgt, const DurationTimeData *timeData,const DurationTimeData *durationTimeData, DmrActionParam *pstActionEvent);
	static int CheckSeekTarget(DmrActionParam *pstActionEvent,const char *pcSeekTgt, const SeekModeTarget *pstModeDtls);
	static int Seek(DmrActionParam *pstActionEvent);
	static int SendTimesByUpnp(DmrActionParam *pstActionEvent, const char *pAllTime, const char *pCurrTime);
	static int GetPositionInfoSendTimes(DmrActionParam *pstActionEvent, const char *pAllTime);
	static int GetPositionInfo(DmrActionParam *pstActionEvent);
	static int GetTransportInfo(DmrActionParam *pstActionEvent);
	static int GetTransportSettings(DmrActionParam *pstActionEvent);
	static int FillMediaInfoIsPresent(DmrActionParam *pstActionEvent, bool noMediaPresent);
	static int FillMediaInfo(DmrActionParam *pstActionEvent);
	static int GetMediaInfo(DmrActionParam *pstActionEvent);
	static int GetDeviceCapabilities(DmrActionParam *pstActionEvent);
	static int GetCurrentTransportActions(DmrActionParam *pstActionEvent);
	static void DoImmediateEventing();
	
	static char *szDMRStatus_[7];
	static SeekModeTarget stDMRSeekModes_[11];
};

}
}
}
}
#endif /* DMR_AV_TRANSPORT_H */
