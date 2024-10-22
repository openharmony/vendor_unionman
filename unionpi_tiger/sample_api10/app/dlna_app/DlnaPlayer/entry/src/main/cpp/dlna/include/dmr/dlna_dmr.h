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

#ifndef DLNA_DMR_H
#define DLNA_DMR_H

#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include "dlna_common.h"
#include "dmr_rendering_control.h"
#include "dmr_av_transport.h"
#include "dmr_connection_manager.h"


namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

#ifdef __cplusplus
extern "C" {
#endif

#define DMR_DESCRIPTION                                                                                       \
    "<?xml version=\"1.0\"?>\r\n"                                                                             \
        "<root xmlns=\"urn:schemas-upnp-org:device-1-0\" xmlns:dlna=\"urn:schemas-dlna-org:device-1-0\">\r\n" \
        "<specVersion>\r\n"                                                                                   \
        "<major>1</major>\r\n"                                                                                \
        "<minor>0</minor>\r\n"                                                                                \
        "</specVersion>\r\n"                                                                                  \
        "<device>\r\n"                                                                                        \
        "<UDN>uuid:</UDN>\r\n"                                                                                \
        "<friendlyName>%s</friendlyName>\r\n"                                                                 \
        "<deviceType>urn:schemas-upnp-org:device:MediaRenderer:1</deviceType>\r\n"                            \
        "<manufacturer>Technologies Co.,Ltd</manufacturer>\r\n"                                     \
        "<manufacturerURL>http://www.hivendor.com</manufacturerURL>\r\n"                                     \
        "<modelName>MediaRenderer</modelName>\r\n"                                                  \
        "<modelNumber>1.1</modelNumber>\r\n"                                                                  \
        "<modelURL>http://www.hivendor.com</modelURL>\r\n"                                                   \
        "<serialNumber/>\r\n"                                                                                 \
        "<dlna:X_DLNADOC xmlns:dlna=\"urn:schemas-dlna-org:device-1-0\">DMR-1.50</dlna:X_DLNADOC>\r\n"        \
        "<dlna:X_DLNACAP xmlns:dlna=\"urn:schemas-dlna-org:device-1-0\"></dlna:X_DLNACAP> \r\n "              \
        "<serviceList>\r\n"                                                                                   \
        "<service>\r\n"                                                                                       \
        "<serviceType>urn:schemas-upnp-org:service:ConnectionManager:1</serviceType>\r\n"                     \
        "<serviceId>urn:upnp-org:serviceId:ConnectionManager</serviceId>\r\n"                                 \
        "<controlURL>/upnp/service/ConnectionManager/Control</controlURL>\r\n"                                \
        "<eventSubURL>/upnp/service/ConnectionManager/Event</eventSubURL>\r\n"                                \
        "<SCPDURL>/upnp/service/Connection_Manager_Service.xml</SCPDURL>\r\n"                                                        \
        "</service>\r\n"                                                                                      \
        "<service>\r\n"                                                                                       \
        "<serviceType>urn:schemas-upnp-org:service:AVTransport:1</serviceType>\r\n"                           \
        "<serviceId>urn:upnp-org:serviceId:AVTransport</serviceId>\r\n"                                       \
        "<controlURL>/upnp/service/AVTransport/Control</controlURL>\r\n"                                      \
        "<eventSubURL>/upnp/service/AVTransport/Event</eventSubURL>\r\n"                                      \
        "<SCPDURL>/upnp/service/AvTransport.xml</SCPDURL>\r\n"                                                        \
        "</service>\r\n"                                                                                      \
        "<service>\r\n"                                                                                       \
        "<serviceType>urn:schemas-upnp-org:service:RenderingControl:1</serviceType>\r\n"                      \
        "<serviceId>urn:upnp-org:serviceId:RenderingControl</serviceId>\r\n"                                  \
        "<controlURL>/upnp/service/RenderingControl/Control</controlURL>\r\n"                                 \
        "<eventSubURL>/upnp/service/RenderingControl/Event</eventSubURL>\r\n"                                 \
        "<SCPDURL>/upnp/service/Rendering_Control_Service.xml</SCPDURL>\r\n"                                                        \
        "</service>\r\n"                                                                                      \
        "</serviceList>\r\n"                                                                                  \
        "</device>\r\n"                                                                                       \
        "</root>"

#define DMR_AVT_EVENT_ATTRIBUTE "<Event\r\n"                                                          \
        "xmlns=\"urn:schemas-upnp-org:metadata-1-0/AVT/\"\r\n"            \
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\r\n"     \
        "xsi:schemaLocation=\"\r\n"                                       \
        "urn:schemas-upnp-org:metadata-1-0/AVT/\r\n"                      \
        "http://www.upnp.org/schemas/av/avt-event-v1-20060531.xsd\">\r\n" \
        "</Event>\r\n"

#define DMR_RENDER_CONTROL_EVENT_ATTRIBUTE "<Event\r\n"                                                          \
        "xmlns=\"urn:schemas-upnp-org:metadata-1-0/RCS/\"\r\n"            \
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\r\n"     \
        "xsi:schemaLocation=\"\r\n"                                       \
        "urn:schemas-upnp-org:metadata-1-0/RCS/\r\n"                      \
        "http://www.upnp.org/schemas/av/rcs-event-v1-20060531.xsd\">\r\n" \
        "</Event>\r\n"

#define VIRTUAL_DIR "/upnp/service"

inline constexpr int DLNA_MIMETYPELIST_LEN = 22;
inline constexpr int DESCRIPTION_PATH_LENGTH = 1024;
inline constexpr int DLNA_PROPERTY_SET_LEN = 3;
inline constexpr int DMR_SUPPORTED_INSTANCE_ID = 0;

enum class DmrAvState {
    DLNA_DMR_STATE_STOPPED = 0,
    DLNA_DMR_STATE_PLAYING,
    DLNA_DMR_STATE_PAUSED_PLAYBACK,
    DLNA_DMR_STATE_RECORDING,
    DLNA_DMR_STATE_PAUSED_RECORDING,
    DLNA_DMR_STATE_NO_MEDIA_PRESENT,
    DLNA_DMR_STATE_TRANSTIONING,
    DLNA_DMR_STATE_BUTT = DLNA_ENUM_END
};

class DlnaDmr{

public:
    static DlnaDmr &getInstance();
    unsigned int RegisterListener(DlnaDmrListenerType enDmrListenerType, const void *pvListenerFunc);
    void DeInit(void);
    void HandleDataFree(void);
    void SetDescriptionPath(const char *descriptionPath);
    int Init(const char *pcMacAddress);
    unsigned int SetName(const char *pcDmrName);
    void HdInit();
    unsigned int RegisterControlListener(DlnaDmrListenerType enDmrListenerType, const void *pvListenerFunc);
    int SubConnectionManagerServiceServiceId(UpnpSubscriptionRequest *event);
    int SubRenderingControlServiceId(UpnpSubscriptionRequest *event);
    void HandleActionRequestError(UpnpActionRequest *request, char *pServiceType);
    void SetRequestError(UpnpActionRequest *request, DmrActionParam stActionParam);
    int GetDefaultDes(char **description, const char *name);

private:
    friend int HandleCallback(Upnp_EventType eventType, const void *event, void *cookie);
    void HandleActionRequest(UpnpActionRequest *request);
    int HandleSubRequest(UpnpSubscriptionRequest *event);
    DmrServiceTable dmrServiceTable_[4]= {
        {
            DMR_AV_TRANSPORT_SERVICE_ID,
            DMR_AV_TRANSPORT_SERVICE_TYPE,
            DmrAVTransport::getInstance().getActionFuncTable()
        },
        {
            DMR_RENDERING_CONTROL_SERVICE_ID,
            DMR_RENDERING_CONTROL_SERVICE_TYPE,
            DmrRenderingControl::getInstance().getActionFuncTable()
        },
        {
            DMR_CONNECTION_MANAGER_SERVICE_ID,
            DMR_CONNECTION_MANAGER_SERVICE_TYPE,
            DmrConnectionManager::getInstance().getActionFuncTable()
        },
        {NULL, NULL, NULL}
    };
};

#ifdef __cplusplus
}
#endif

}
}
}
}
#endif /* _DLNADMR_H_ */
