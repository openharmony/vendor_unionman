/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: dmr connection manager service
 * Author: 
 * Create: 2023-10-20
 */

#ifndef DMR_CONNECTION_MANAGER_H
#define DMR_CONNECTION_MANAGER_H

#include "dlna_common.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

inline constexpr char *DMR_CONNECTION_MANAGER_LOCATION = "/ConnectionManager.xml";
inline constexpr char *DMR_CONNECTION_MANAGER_SERVICE_ID = "urn:upnp-org:serviceId:ConnectionManager";
inline constexpr char *DMR_CONNECTION_MANAGER_SERVICE_TYPE = "urn:schemas-upnp-org:service:ConnectionManager:1";
inline constexpr char *CONNECTION_MANAGER_SERVICE_ACTION_PROT_INFO = "GetProtocolInfo";
inline constexpr char *CONNECTION_MANAGER_SERVICE_ACTION_CON_ID = "GetCurrentConnectionIDs";
inline constexpr char *CONNECTION_MANAGER_SERVICE_ACTION_CON_INFO = "GetCurrentConnectionInfo";
inline const char *CONNECTION_MANAGER_SERVICE_ARG_SOURCE = "Source";
inline const char *CONNECTION_MANAGER_SERVICE_ARG_SINK = "Sink";
inline const char *CONNECTION_MANAGER_SERVICE_ARG_CONNECTION_IDS = "ConnectionIDs";
inline const char *CONNECTION_MANAGER_SERVICE_ARG_CONNECTION_ID = "ConnectionID";
inline const char *CONNECTION_MANAGER_SERVICE_ARG_RCS_ID = "RcsID";
inline const char *CONNECTION_MANAGER_SERVICE_ARG_TRANSPORT_ID = "AVTransportID";
inline const char *CONNECTION_MANAGER_SERVICE_ARG_PROT_INFO = "ProtocolInfo";
inline const char *CONNECTION_MANAGER_SERVICE_ARG_PEER_CON_MANAGER = "PeerConnectionManager";
inline const char *CONNECTION_MANAGER_SERVICE_ARG_PEER_CON_ID = "PeerConnectionID";
inline const char *CONNECTION_MANAGER_SERVICE_ARG_DIRECTION = "Direction";
inline const char *CONNECTION_MANAGER_SERVICE_ARG_STATUS = "Status";
inline const char *CONNECTION_MANAGER_SERVICE_DEFAULT_CON_ID = "0";
inline const char *CONNECTION_MANAGER_SERVICE_UNKNOW_ID = "-1";
inline const char *CONNECTION_MANAGER_SERVICE_OUTPUT = "Output";
inline const char *CONNECTION_MANAGER_SERVICE_INPUT = "Input";
inline const char *CONNECTION_MANAGER_SERVICE_STATUS_OK = "OK";
inline const char *SERVICE_DEFAULT_ID = "0";

/* CONNECTION MANAGER SERVICE ERROR CODE */
inline constexpr int DMR_CONNECTION_MANAGER_ERROR_701 = 701;
inline constexpr int DMR_CONNECTION_MANAGER_ERROR_702 = 702;
inline constexpr int DMR_CONNECTION_MANAGER_ERROR_703 = 703;
inline constexpr int DMR_CONNECTION_MANAGER_ERROR_704 = 704;
inline constexpr int DMR_CONNECTION_MANAGER_ERROR_705 = 705;
inline constexpr int DMR_CONNECTION_MANAGER_ERROR_706 = 706;
inline constexpr int DMR_CONNECTION_MANAGER_ERROR_707 = 707;
inline constexpr int DMR_CONNECTION_MANAGER_ERROR_708 = 708;
inline constexpr int DMR_CONNECTION_MANAGER_ERROR_709 = 709;
inline constexpr int DMR_CONNECTION_MANAGER_ERROR_710 = 710;
inline constexpr int DMR_CONNECTION_MANAGER_ERROR_711 = 711;
/* CONNECTION MANAGER SERVICE ERROR STRING */
inline constexpr char *DMR_CONNECTION_MANAGER_ERROR_701_STRING = "Incompatible protocol info";
inline constexpr char *DMR_CONNECTION_MANAGER_ERROR_702_STRING = "Incompatible directions";
inline constexpr char *DMR_CONNECTION_MANAGER_ERROR_703_STRING = "Insufficient network resources";
inline constexpr char *DMR_CONNECTION_MANAGER_ERROR_704_STRING = "Local restrictions";
inline constexpr char *DMR_CONNECTION_MANAGER_ERROR_705_STRING = "Access denied";
inline constexpr char *DMR_CONNECTION_MANAGER_ERROR_706_STRING = "Invalid connection reference";
inline constexpr char *DMR_CONNECTION_MANAGER_ERROR_707_STRING = "Not in network";
inline constexpr char *DMR_CONNECTION_MANAGER_ERROR_708_STRING = "Connection Table overflow";
inline constexpr char *DMR_CONNECTION_MANAGER_ERROR_709_STRING = "Internal processing resources exceeded";
inline constexpr char *DMR_CONNECTION_MANAGER_ERROR_710_STRING = "Internal memory resources exceeded";
inline constexpr char *DMR_CONNECTION_MANAGER_ERROR_711_STRING = "Internal storage system capabilities exceeded";

class DmrConnectionManager{
public:
	static DmrConnectionManager &getInstance();
	DmrServiceAction *getActionFuncTable();
		
private:
	DlnaMimeType *DlnaGetDmrMimeTypeList();
	int GetCurrentConnectionInfoInit(DmrActionParam *pstActionEvent);
	int AddAllParamToActionResponse(DmrActionParam *pstActionEvent);
	static int GetProtocolInfo(DmrActionParam *pstActionEvent);
	static int GetCurrentConnectionIDs(DmrActionParam *pstActionEvent);
	static int GetCurrentConnectionInfo(DmrActionParam *pstActionEvent);
};

}
}
}
}
#endif /* DMR_CONNECTION_MANAGER_H */
