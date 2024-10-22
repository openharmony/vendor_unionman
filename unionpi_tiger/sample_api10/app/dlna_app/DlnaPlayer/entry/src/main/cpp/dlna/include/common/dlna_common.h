/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: dlna common
 * Author: 
 * Create: 2023-10-20
 */

#ifndef DLNA_COMMON_H
#define DLNA_COMMON_H

#include "ithread.h"
#include "ixml.h"
#include "upnp.h"
#include "dlna_types.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

#define NOTLIMIT_NETWORK_ADAPTER_NAME "default"

#define DLNA_P2P0_NAME "p2p"
#define UDN_LEN 250
#define DECIMAL 10
#define MAC_ITEM_LEN 5
#define DLNA_ENABLE_IPV6 0

#define DLNA_DMR_PORT 55826
/* dlna over p2p port */
#define DLNA_DMR_OVER_P2P_PORT 55836
constexpr  int DLNA_DEFAULT_PORT = 55866;
constexpr  int MAC_LEN = 20;

struct DmrActionParam{
    UpnpActionRequest *pstActionRequest;
    char *pcServiceType;
    int iErrorCode;
    char *pErrorStr;
};


struct DmrServiceAction{
    char *pszActionName;
    int (*DlnaActionFn)(DmrActionParam *);
};


struct DmrServiceTable{
    char *pszServiceID;
    char *pszServiceType;
    DmrServiceAction *pServerActions;
};

class DlnaCommon{
private:
	DlnaStateType initMode_ = DlnaStateType::DLNA_STATE_UNINTIALISED;
	
	//DlnaStateType dlnaCommonState_ = DlnaStateType::DLNA_STATE_UNINTIALISED;

	char commonHttpIpAddr_[HTTPIPADDRLEN];

    char commonURI_[URILEN];
    
    /* DMR */
    struct DlnaDmrDevice{
        int ulDeviceInit;
        UpnpDevice_Handle dev;
        char *udn;
        int dmrState;
        int setURIactionDone;
        int stopActionDone;
        char *pszPlayURL;
        char *pszTrackMetaData;
        char *pszCurrentTransportActions;
        int isImage;
    
        /* RENDERINGCONTROLSERVICE State Variables */
        int bMuteStateVar;
        unsigned int ulVolumeStateVar;
        unsigned int ulPresetNameStateVar;
    
        /* AVTRANSPORT Eventing Thread ID */
        int iAvtTmThdId;
    
        char *currentduration;
    
        /* RENDERINGCONTROLSERVICE Eventing Thread ID */
        int iRcsTmThdId;
    
        unsigned int ulAvtEventNotifySet;
        unsigned int ulRcsEventNotifySet;
    
        int bSendQuickNotify;
        int bRCSSendQuickNotify;
    
        ithread_mutex_t dmrAVTVariableMutex;
    
        pFnaDmrAvTransportSetMediaUri pFnDmrAvtSetMediaUri;
        pFnDmrAvTransportPlay pFnDmrAvtPlay;
        pFnDmrAvTransportPause pFnDmrAvtPause;
        pFnDmrAvTransportStop pFnDmrAvtStop;
        pFnDmrAvTransportSeek pFnDmrAvtSeek;
        pFnDmrAvTransportGetCurrtime pFnDmrAvtGetCurrtime;
        pFnDmrAvTransportGetAlltime pFnDmrAvtGetAlltime;
        pFnDmrAvTransportSetVol pFnDmrAvtSetVol;
        pFnDmrAvTransportGetVol pFnDmrAvtGetVol;
        pFnDmrAvTransportSetMute pFnDmrAvtGetSetMute;
    };

	DlnaDmrDevice *dlnaDmrHdl_;
    
    struct DlnaMetaDataList{
        unsigned int ulEntryCount;
        DlnaEntryNode **pstEntryNode;
    };
    
    DlnaMetaDataList metaDataList_;
public:
	DlnaStateType dlnaCommonState_ = DlnaStateType::DLNA_STATE_UNINTIALISED;
	static DlnaCommon &getInstance();
	DlnaCommon();
	~DlnaCommon();
	
	unsigned int ChangeDeviceDescription(ChangeDeviceDes changeDeviceDes);

	int ChangeDescriptionFriendlyName(IXML_Document *pstDescDoc, const char *pszDeviceName);
	int CreateConnect(int *s32Socket, struct sockaddr_in *addr);

	char *GetUDN(char *pucUdnPrefix, const char *pcMacAddress);
	int HTTPSendMsgCommom(int s32Socket, char *pchMsgBuf, unsigned int u32MsgLen);
	int HttpReceiveMsg(int s32Socket, char *pchMsgBuf, unsigned int *u32MsgLen);

	int AddParamToActionResponse(DmrActionParam *event, const char *key, const char *value,
	    int len);
	int SafetyInitHttpData(char *httpRequest, int httpRequestLen, char *httpMsgBuffer, int httpMsgBufferLen);
	int GetIpaddrAndPort(const char *pURL);
	int InitURI(char **pFlagPtr);

	char *GetStr(UpnpActionRequest *request, const char *key);
	int GetUniString(char *uniUND, unsigned int uniUNDLen);

	int ConvAscToSInt(unsigned char *pucString);

	int GetInt(UpnpActionRequest *request, const char *key);
	int AddrInit(int *s32Socket, struct sockaddr_in *addr);

	unsigned int GetUInt(UpnpActionRequest *request, const char *pchObject);

	unsigned int GetIntValue(UpnpActionRequest *request, const char *key, int *piValue);

	int FindServiceAndAction(UpnpActionRequest *request, DmrServiceTable *pstServiceTable,
	    char **ppcServiceType, DmrServiceAction **ppcAction);

	const char *DocumentGetChildElementValue(IXML_Document *pstParent, const char *pcTagName);

	int UpnpInit(const char *pcAdapterName, DlnaStateType ulInitMode);
    int UpnpInitExt(const char *pcAdapterName, const char *ipAddr, DlnaStateType ulInitMode);

    int VirtualDirInit(void);

	char *ConnectionManagerServiceCollectProtoInfo(DlnaLocalDeviceType enDeviceType);

	static void DlnaMtPrintAllGlobal(void);

	const char* GetIpBySockaddrStorage(const struct sockaddr_storage *sockAddr, char *tmpIp, int tmpIpSize);

	unsigned int Run(DlnaStateType ulInitMode);

	void DeInit(void);

	unsigned int MapInitEnum(DlnaInitModeType enInitMode, DlnaStateType *pulInitMode);

	void *Realloc(void *src, unsigned int oldSize, unsigned int newSize);

	DlnaDmrDevice *GetDmrHdl();
	DlnaStateType GetInitMode();
	char *GetPcDeviceName();
	void SetPcDeviceName(char *pcDeviceName);
	void FreePcDeviceName();
	DlnaMetaDataList *GetStMetaDataList();
	DlnaStateType GetState();
	char *GetPchUdn(const char *pucUdnPrefix, const char *pchUniString, int pchUniStringLen);
	bool GetTextNode(IXML_NodeList *pstNodeList, IXML_Node **pstTextNode);
	bool CheckPcMacAddress(const char *pcMacAddress, int index);
	unsigned int SetDeviceName(const char *pcNewName);
	const char *GetVersion();
	unsigned int ConvAscToUInt(unsigned char *pucString);
	int ConvAscToSIntInit(unsigned char **pucBufPtr, int *bIsNeg);
	bool ConvValNormal(int bIsNeg, int convVal);
	bool IsInt(unsigned char ucChar);
	
	unsigned int ServceInit(const char *pcAdapterName, const char *pcMacAddress, const DlnaStateType ulInitMode);
    unsigned int ServceInitExt(const char *pcAdapterName, const char *ipAddr, const char *pcMacAddress, const DlnaStateType ulInitMode);
    unsigned int Create(const char *pcAdapterName, const char *pcMacAddress, DlnaInitModeType enInitMode);
    unsigned int CreateExt(const char *pcAdapterName, const char *ipAddr, const char *pcMacAddress, DlnaInitModeType enInitMode);
    void Destroy(void);
};

}
}
}
}
#endif /* DLNA_COMMON_H */
