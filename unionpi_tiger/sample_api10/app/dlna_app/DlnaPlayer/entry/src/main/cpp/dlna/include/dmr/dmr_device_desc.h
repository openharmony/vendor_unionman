/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: dmr device description
 * Author: 
 * Create: 2023-10-20
 */

#ifndef DMR_DEVICE_DESC_H
#define DMR_DEVICE_DESC_H

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

#define DEV_DESC "<?xml version=\"1.0\"?>\r\n"                                                                                \
                "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\r\n"                                                   \
                    "<specVersion>\r\n"                                                                                     \
                        "<major>1</major>\r\n"                                                                              \
                        "<minor>0</minor>\r\n"                                                                              \
                    "</specVersion>\r\n"                                                                                    \
                    "<device>\r\n"                                                                                          \
                        "<UDN>uuid:</UDN>\r\n"                                                                                  \
                        "<friendlyName>MediaRenderer</friendlyName>\r\n"                                                        \
                        "<deviceType>urn:schemas-upnp-org:device:MediaRenderer:1</deviceType>\r\n"                              \
                        "<manufacturer>FHM</manufacturer>\r\n"                                   \
                        "<manufacturerURL>FHM</manufacturerURL>\r\n"                                                        \
                        "<modelName>FHM MediaRenderer</modelName>\r\n"                                                \
                        "<modelNumber>1.1</modelNumber>\r\n"                                                                \
                        "<modelURL>FHM</modelURL>\r\n"                                                                      \
                        "<serialNumber>123456789001</serialNumber>\r\n"                                    					\
                        "<serviceList>\r\n"                                                                                 \
                            "<service>\r\n"                                                                                 \
                                "<serviceType>urn:schemas-upnp-org:service:ConnectionManager:1</serviceType>\r\n"           \
                                "<serviceId>urn:upnp-org:serviceId:ConnectionManager</serviceId>\r\n"                       \
                                "<controlURL>/upnp/service/ConnectionManager/Control</controlURL>\r\n"                      \
                                "<eventSubURL>/upnp/service/ConnectionManager/Event</eventSubURL>\r\n"                      \
                                "<SCPDURL>/upnp/service/Connection_Manager_Service.xml</SCPDURL>\r\n"                                              \
                            "</service>\r\n"                                                                                \
                            "<service>\r\n"                                                                                 \
                                "<serviceType>urn:schemas-upnp-org:service:AVTransport:1</serviceType>\r\n"                 \
                                "<serviceId>urn:upnp-org:serviceId:AVTransport</serviceId>\r\n"                             \
                                "<controlURL>/upnp/service/AVTransport/Control</controlURL>\r\n"                            \
                                "<eventSubURL>/upnp/service/AVTransport/Event</eventSubURL>\r\n"                            \
                                "<SCPDURL>/upnp/service/AvTransport.xml</SCPDURL>\r\n"                                              \
                            "</service>\r\n"                                                                                \
                            "<service>\r\n"                                                                                 \
                                "<serviceType>urn:schemas-upnp-org:service:RenderingControl:1</serviceType>\r\n"            \
                                "<serviceId>urn:upnp-org:serviceId:RenderingControl</serviceId>\r\n"                        \
                                "<controlURL>/upnp/service/RenderingControl/Control</controlURL>\r\n"                       \
                                "<eventSubURL>/upnp/service/RenderingControl/Event</eventSubURL>\r\n"                       \
                                "<SCPDURL>/upnp/service/Rendering_Control_Service.xml</SCPDURL>\r\n"                                              \
                            "</service>\r\n"                                                                                \
                        "</serviceList>\r\n"                                                                                \
                    "</device>\r\n"                                                                                         \
                "</root>\r\n"                                                                                             

#define CONTROL_DEVICE_SERVICE_DESCRIPTION "<?xml version=\"1.0\"?>\r\n"                                                  \
						"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\r\n" 				   \
						"<specVersion>\r\n" 													   \
						"<major>1</major>\r\n"													   \
						"<minor>0</minor>\r\n"													   \
						"</specVersion>\r\n"													   \
						"<actionList>\r\n"														   \
						"<action>\r\n"															   \
						"<name>GetSearchCapabilities</name>\r\n"								   \
						"<argumentList>\r\n"													   \
						"<argument>\r\n"														   \
						"<name>SearchCaps</name>\r\n"											   \
						"<direction>out</direction>\r\n"										   \
						"<relatedStateVariable>SearchCapabilities</relatedStateVariable>\r\n"	   \
						"</argument>\r\n"														   \
						"</argumentList>\r\n"													   \
						"</action>\r\n" 														   \
						"<action>\r\n"															   \
						"<name>GetSortCapabilities</name>\r\n"									   \
						"<argumentList>\r\n"													   \
						"<argument>\r\n"														   \
						"<name>SortCaps</name>\r\n" 											   \
						"<direction>out</direction>\r\n"										   \
						"<relatedStateVariable>SortCapabilities</relatedStateVariable>\r\n" 	   \
						"</argument>\r\n"														   \
						"</argumentList>\r\n"													   \
						"</action>\r\n" 														   \
						"<action>\r\n"															   \
						"<name>GetSystemUpdateID</name>\r\n"									   \
						"<argumentList>\r\n"													   \
						"<argument>\r\n"														   \
						"<name>Id</name>\r\n"													   \
						"<direction>out</direction>\r\n"										   \
						"<relatedStateVariable>SystemUpdateID</relatedStateVariable>\r\n"		   \
						"</argument>\r\n"														   \
						"</argumentList>\r\n"													   \
						"</action>\r\n" 														   \
						"<action>\r\n"															   \
						"<name>Browse</name>\r\n"												   \
						"<argumentList>\r\n"													   \
						"<argument>\r\n"														   \
						"<name>ObjectID</name>\r\n" 											   \
						"<direction>in</direction>\r\n" 										   \
						"<relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>\r\n"	   \
						"</argument>\r\n"														   \
						"<argument>\r\n"														   \
						"<name>BrowseFlag</name>\r\n"											   \
						"<direction>in</direction>\r\n" 										   \
						"<relatedStateVariable>A_ARG_TYPE_BrowseFlag</relatedStateVariable>\r\n"   \
						"</argument>\r\n"														   \
						"<argument>\r\n"														   \
						"<name>Filter</name>\r\n"												   \
						"<direction>in</direction>\r\n" 										   \
						"<relatedStateVariable>A_ARG_TYPE_Filter</relatedStateVariable>\r\n"	   \
						"</argument>\r\n"														   \
						"<argument>\r\n"														   \
						"<name>StartingIndex</name>\r\n"										   \
						"<direction>in</direction>\r\n" 										   \
						"<relatedStateVariable>A_ARG_TYPE_Index</relatedStateVariable>\r\n" 	   \
						"</argument>\r\n"														   \
						"<argument>\r\n"														   \
						"<name>RequestedCount</name>\r\n"										   \
						"<direction>in</direction>\r\n" 										   \
						"<relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>\r\n" 	   \
						"</argument>\r\n"														   \
						"<argument>\r\n"														   \
						"<name>SortCriteria</name>\r\n" 										   \
						"<direction>in</direction>\r\n" 										   \
						"<relatedStateVariable>A_ARG_TYPE_SortCriteria</relatedStateVariable>\r\n" \
						"</argument>\r\n"														   \
						"<argument>\r\n"														   \
						"<name>Result</name>\r\n"												   \
						"<direction>out</direction>\r\n"										   \
						"<relatedStateVariable>A_ARG_TYPE_Result</relatedStateVariable>\r\n"	   \
						"</argument>\r\n"														   \
						"<argument>\r\n"														   \
						"<name>NumberReturned</name>\r\n"										   \
						"<direction>out</direction>\r\n"										   \
						"<relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>\r\n" 	   \
						"</argument>\r\n"														   \
						"<argument>\r\n"														   \
						"<name>TotalMatches</name>\r\n" 										   \
						"<direction>out</direction>\r\n"										   \
						"<relatedStateVariable>A_ARG_TYPE_Count</relatedStateVariable>\r\n" 	   \
						"</argument>\r\n"														   \
						"<argument>\r\n"														   \
						"<name>UpdateID</name>\r\n" 											   \
						"<direction>out</direction>\r\n"										   \
						"<relatedStateVariable>A_ARG_TYPE_UpdateID</relatedStateVariable>\r\n"	   \
						"</argument>\r\n"														   \
						"</argumentList>\r\n"													   \
						"</action>\r\n" 														   \
						"<action>\r\n"															   \
						"<name>CreateObject</name>\r\n" 										   \
						"<argumentList>\r\n"													   \
						"<argument>\r\n"														   \
						"<name>ContainerID</name>\r\n"											   \
						"<direction>in</direction>\r\n" 										   \
						"<relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>\r\n"	   \
						"</argument>\r\n"														   \
						"<argument>\r\n"														   \
						"<name>Elements</name>\r\n" 											   \
						"<direction>in</direction>\r\n" 										   \
						"<relatedStateVariable>A_ARG_TYPE_Result</relatedStateVariable>\r\n"	   \
						"</argument>\r\n"														   \
						"<argument>\r\n"														   \
						"<name>ObjectID</name>\r\n" 											   \
						"<direction>out</direction>\r\n"										   \
						"<relatedStateVariable>A_ARG_TYPE_ObjectID</relatedStateVariable>\r\n"	   \
						"</argument>\r\n"														   \
						"<argument>\r\n"														   \
						"<name>Result</name>\r\n"												   \
						"<direction>out</direction>\r\n"										   \
						"<relatedStateVariable>A_ARG_TYPE_Result</relatedStateVariable>\r\n"	   \
						"</argument>\r\n"														   \
						"</argumentList>\r\n"													   \
						"</action>\r\n" 														   \
						"</actionList>\r\n" 													   \
						"<serviceStateTable>\r\n"												   \
						"<stateVariable sendEvents=\"no\">\r\n" 								   \
						"<name>SearchCapabilities</name>\r\n"									   \
						"<dataType>string</dataType>\r\n"										   \
						"</stateVariable>\r\n"													   \
						"<stateVariable sendEvents=\"no\">\r\n" 								   \
						"<name>SortCapabilities</name>\r\n" 									   \
						"<dataType>string</dataType>\r\n"										   \
						"</stateVariable>\r\n"													   \
						"<stateVariable sendEvents=\"yes\">\r\n"								   \
						"<name>SystemUpdateID</name>\r\n"										   \
						"<dataType>ui4</dataType>\r\n"											   \
						"</stateVariable>\r\n"													   \
						"<stateVariable sendEvents=\"no\">\r\n" 								   \
						"<name>A_ARG_TYPE_ObjectID</name>\r\n"									   \
						"<dataType>string</dataType>\r\n"										   \
						"</stateVariable>\r\n"													   \
						"<stateVariable sendEvents=\"no\">\r\n" 								   \
						"<name>A_ARG_TYPE_Result</name>\r\n"									   \
						"<dataType>string</dataType>\r\n"										   \
						"</stateVariable>\r\n"													   \
						"<stateVariable sendEvents=\"no\">\r\n" 								   \
						"<name>A_ARG_TYPE_BrowseFlag</name>\r\n"								   \
						"<dataType>string</dataType>\r\n"										   \
						"<allowedValueList>\r\n"												   \
						"<allowedValue>BrowseMetadata</allowedValue>\r\n"						   \
						"<allowedValue>BrowseDirectChildren</allowedValue>\r\n" 				   \
						"</allowedValueList>\r\n"												   \
						"</stateVariable>\r\n"													   \
						"<stateVariable sendEvents=\"no\">\r\n" 								   \
						"<name>A_ARG_TYPE_Filter</name>\r\n"									   \
						"<dataType>string</dataType>\r\n"										   \
						"</stateVariable>\r\n"													   \
						"<stateVariable sendEvents=\"no\">\r\n" 								   \
						"<name>A_ARG_TYPE_SortCriteria</name>\r\n"								   \
						"<dataType>string</dataType>\r\n"										   \
						"</stateVariable>\r\n"													   \
						"<stateVariable sendEvents=\"no\">\r\n" 								   \
						"<name>A_ARG_TYPE_Index</name>\r\n" 									   \
						"<dataType>ui4</dataType>\r\n"											   \
						"</stateVariable>\r\n"													   \
						"<stateVariable sendEvents=\"no\">\r\n" 								   \
						"<name>A_ARG_TYPE_Count</name>\r\n" 									   \
						"<dataType>ui4</dataType>\r\n"											   \
						"</stateVariable>\r\n"													   \
						"<stateVariable sendEvents=\"no\">\r\n" 								   \
						"<name>A_ARG_TYPE_UpdateID</name>\r\n"									   \
						"<dataType>ui4</dataType>\r\n"											   \
						"</stateVariable>\r\n"													   \
						"</serviceStateTable>\r\n"												   \
						"</scpd>\r\n"
				
#define CONTROL_DEVICE_SERVICE_DESCRIPTION_LEN strlen(CONTROL_DEVICE_SERVICE_DESCRIPTION)
				
#define CONTROL_DEVICE_SERVICE_LOCATION "/upnp/service/Control_Device_Service.xml"
				
#define CONNECTION_MANAGER_SERVICE_DESCRIPTION "<?xml version=\"1.0\"?>\r\n"                                                       \
						"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\r\n" 						\
						"<specVersion>\r\n" 															\
						"<major>1</major>\r\n"															\
						"<minor>0</minor>\r\n"															\
						"</specVersion>\r\n"															\
						"<actionList>\r\n"																\
						"<action>\r\n"																	\
						"<name>GetProtocolInfo</name>\r\n"												\
						"<argumentList>\r\n"															\
						"<argument>\r\n"																\
						"<name>Source</name>\r\n"														\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>SourceProtocolInfo</relatedStateVariable>\r\n"			\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>Sink</name>\r\n" 														\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>SinkProtocolInfo</relatedStateVariable>\r\n" 			\
						"</argument>\r\n"																\
						"</argumentList>\r\n"															\
						"</action>\r\n" 																\
						"<action>\r\n"																	\
						"<name>GetCurrentConnectionIDs</name>\r\n"										\
						"<argumentList>\r\n"															\
						"<argument>\r\n"																\
						"<name>ConnectionIDs</name>\r\n"												\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>CurrentConnectionIDs</relatedStateVariable>\r\n" 		\
						"</argument>\r\n"																\
						"</argumentList>\r\n"															\
						"</action>\r\n" 																\
						"<action>\r\n"																	\
						"<name>GetCurrentConnectionInfo</name>\r\n" 									\
						"<argumentList>\r\n"															\
						"<argument>\r\n"																\
						"<name>ConnectionID</name>\r\n" 												\
						"<direction>in</direction>\r\n" 												\
						"<relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>RcsID</name>\r\n"														\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>A_ARG_TYPE_RcsID</relatedStateVariable>\r\n" 			\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>AVTransportID</name>\r\n"												\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>A_ARG_TYPE_AVTransportID</relatedStateVariable>\r\n" 	\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>ProtocolInfo</name>\r\n" 												\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>A_ARG_TYPE_ProtocolInfo</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>PeerConnectionManager</name>\r\n"										\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>A_ARG_TYPE_ConnectionManager</relatedStateVariable>\r\n" \
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>PeerConnectionID</name>\r\n" 											\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>A_ARG_TYPE_ConnectionID</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>Direction</name>\r\n"													\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>A_ARG_TYPE_Direction</relatedStateVariable>\r\n" 		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>Status</name>\r\n"														\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>A_ARG_TYPE_ConnectionStatus</relatedStateVariable>\r\n"	\
						"</argument>\r\n"																\
						"</argumentList>\r\n"															\
						"</action>\r\n" 																\
						"</actionList>\r\n" 															\
						"<serviceStateTable>\r\n"														\
						"<stateVariable sendEvents=\"yes\">\r\n"										\
						"<name>SourceProtocolInfo</name>\r\n"											\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"yes\">\r\n"										\
						"<name>SinkProtocolInfo</name>\r\n" 											\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"yes\">\r\n"										\
						"<name>CurrentConnectionIDs</name>\r\n" 										\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>A_ARG_TYPE_ConnectionStatus</name>\r\n"									\
						"<dataType>string</dataType>\r\n"												\
						"<allowedValueList>\r\n"														\
						"<allowedValue>OK</allowedValue>\r\n"											\
						"<allowedValue>ContentFormatMismatch</allowedValue>\r\n"						\
						"<allowedValue>InsufficientBandwidth</allowedValue>\r\n"						\
						"<allowedValue>UnreliableChannel</allowedValue>\r\n"							\
						"<allowedValue>Unknown</allowedValue>\r\n"										\
						"</allowedValueList>\r\n"														\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>A_ARG_TYPE_ConnectionManager</name>\r\n" 								\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>A_ARG_TYPE_Direction</name>\r\n" 										\
						"<dataType>string</dataType>\r\n"												\
						"<allowedValueList>\r\n"														\
						"<allowedValue>Input</allowedValue>\r\n"										\
						"<allowedValue>Output</allowedValue>\r\n"										\
						"</allowedValueList>\r\n"														\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>A_ARG_TYPE_ProtocolInfo</name>\r\n"										\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>A_ARG_TYPE_ConnectionID</name>\r\n"										\
						"<dataType>i4</dataType>\r\n"													\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>A_ARG_TYPE_AVTransportID</name>\r\n" 									\
						"<dataType>i4</dataType>\r\n"													\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>A_ARG_TYPE_RcsID</name>\r\n" 											\
						"<dataType>i4</dataType>\r\n"													\
						"</stateVariable>\r\n"															\
						"</serviceStateTable>\r\n"														\
						"</scpd>\r\n"
				
				
#define CONNECTION_MANAGER_SERVICE_DESCRIPTION_LEN strlen(CONNECTION_MANAGER_SERVICE_DESCRIPTION)
				
#define CONNECTION_MANAGER_SERVICE_LOCATION "/upnp/service/Connection_Manager_Service.xml"
				
				
#define AVTRANSPORT_LOCATION "/upnp/service/AvTransport.xml"
				
#define AVTRANSPORT_DESCRIPTION "<?xml version=\"1.0\"?>\r\n"                                                       \
						"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\r\n" 						\
						"<specVersion>\r\n" 															\
						"<major>1</major>\r\n"															\
						"<minor>0</minor>\r\n"															\
						"</specVersion>\r\n"															\
						"<actionList>\r\n"																\
						"<action>\r\n"																	\
						"<name>SetAVTransportURI</name>\r\n"											\
						"<argumentList>\r\n"															\
						"<argument>\r\n"																\
						"<name>InstanceID</name>\r\n"													\
						"<direction>in</direction>\r\n" 												\
						"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>CurrentURI</name>\r\n"													\
						"<direction>in</direction>\r\n" 												\
						"<relatedStateVariable>AVTransportURI</relatedStateVariable>\r\n"				\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>CurrentURIMetaData</name>\r\n"											\
						"<direction>in</direction>\r\n" 												\
						"<relatedStateVariable>AVTransportURIMetaData</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"</argumentList>\r\n"															\
						"</action>\r\n" 																\
						"<action>\r\n"																	\
						"<name>GetMediaInfo</name>\r\n" 												\
						"<argumentList>\r\n"															\
						"<argument>\r\n"																\
						"<name>InstanceID</name>\r\n"													\
						"<direction>in</direction>\r\n" 												\
						"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>NrTracks</name>\r\n" 													\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>NumberOfTracks</relatedStateVariable>\r\n"				\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>MediaDuration</name>\r\n"												\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>CurrentMediaDuration</relatedStateVariable>\r\n" 		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>CurrentURI</name>\r\n"													\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>AVTransportURI</relatedStateVariable>\r\n"				\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>CurrentURIMetaData</name>\r\n"											\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>AVTransportURIMetaData</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>NextURI</name>\r\n"														\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>NextAVTransportURI</relatedStateVariable>\r\n"			\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>NextURIMetaData</name>\r\n"												\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>NextAVTransportURIMetaData</relatedStateVariable>\r\n"	\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>PlayMedium</name>\r\n"													\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>PlaybackStorageMedium</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>RecordMedium</name>\r\n" 												\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>RecordStorageMedium</relatedStateVariable>\r\n"			\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>WriteStatus</name>\r\n"													\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>RecordMediumWriteStatus</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"</argumentList>\r\n"															\
						"</action>\r\n" 																\
						"<action>\r\n"																	\
						"<name>GetDeviceCapabilities</name>\r\n"										\
						"<argumentList>\r\n"															\
						"<argument>\r\n"																\
						"<name>InstanceID</name>\r\n"													\
						"<direction>in</direction>\r\n" 												\
						"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>PlayMedia</name>\r\n"													\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>PossiblePlaybackStorageMedia</relatedStateVariable>\r\n" \
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>RecMedia</name>\r\n" 													\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>PossibleRecordStorageMedia</relatedStateVariable>\r\n"	\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>RecQualityModes</name>\r\n"												\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>PossibleRecordQualityModes</relatedStateVariable>\r\n"	\
						"</argument>\r\n"																\
						"</argumentList>\r\n"															\
						"</action>\r\n" 																\
						"<action>\r\n"																	\
						"<name>GetTransportInfo</name>\r\n" 											\
						"<argumentList>\r\n"															\
						"<argument>\r\n"																\
						"<name>InstanceID</name>\r\n"													\
						"<direction>in</direction>\r\n" 												\
						"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>CurrentTransportState</name>\r\n"										\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>TransportState</relatedStateVariable>\r\n"				\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>CurrentTransportStatus</name>\r\n"										\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>TransportStatus</relatedStateVariable>\r\n"				\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>CurrentSpeed</name>\r\n" 												\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>TransportPlaySpeed</relatedStateVariable>\r\n"			\
						"</argument>\r\n"																\
						"</argumentList>\r\n"															\
						"</action>\r\n" 																\
						"<action>\r\n"																	\
						"<name>GetPositionInfo</name>\r\n"												\
						"<argumentList>\r\n"															\
						"<argument>\r\n"																\
						"<name>InstanceID</name>\r\n"													\
						"<direction>in</direction>\r\n" 												\
						"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>Track</name>\r\n"														\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>CurrentTrack</relatedStateVariable>\r\n" 				\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>TrackDuration</name>\r\n"												\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>CurrentTrackDuration</relatedStateVariable>\r\n" 		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>TrackMetaData</name>\r\n"												\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>CurrentTrackMetaData</relatedStateVariable>\r\n" 		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>TrackURI</name>\r\n" 													\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>CurrentTrackURI</relatedStateVariable>\r\n"				\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>RelTime</name>\r\n"														\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>RelativeTimePosition</relatedStateVariable>\r\n" 		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>AbsTime</name>\r\n"														\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>AbsoluteTimePosition</relatedStateVariable>\r\n" 		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>RelCount</name>\r\n" 													\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>RelativeCounterPosition</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>AbsCount</name>\r\n" 													\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>AbsoluteCounterPosition</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"</argumentList>\r\n"															\
						"</action>\r\n" 																\
						"<action>\r\n"																	\
						"<name>GetTransportSettings</name>\r\n" 										\
						"<argumentList>\r\n"															\
						"<argument>\r\n"																\
						"<name>InstanceID</name>\r\n"													\
						"<direction>in</direction>\r\n" 												\
						"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>PlayMode</name>\r\n" 													\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>CurrentPlayMode</relatedStateVariable>\r\n"				\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>RecQualityMode</name>\r\n"												\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>CurrentRecordQualityMode</relatedStateVariable>\r\n" 	\
						"</argument>\r\n"																\
						"</argumentList>\r\n"															\
						"</action>\r\n" 																\
						"<action>\r\n"																	\
						"<name>Stop</name>\r\n" 														\
						"<argumentList>\r\n"															\
						"<argument>\r\n"																\
						"<name>InstanceID</name>\r\n"													\
						"<direction>in</direction>\r\n" 												\
						"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"</argumentList>\r\n"															\
						"</action>\r\n" 																\
						"<action>\r\n"																	\
						"<name>Play</name>\r\n" 														\
						"<argumentList>\r\n"															\
						"<argument>\r\n"																\
						"<name>InstanceID</name>\r\n"													\
						"<direction>in</direction>\r\n" 												\
						"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>Speed</name>\r\n"														\
						"<direction>in</direction>\r\n" 												\
						"<relatedStateVariable>TransportPlaySpeed</relatedStateVariable>\r\n"			\
						"</argument>\r\n"																\
						"</argumentList>\r\n"															\
						"</action>\r\n" 																\
						"<action>\r\n"																	\
						"<name>Pause</name>\r\n"														\
						"<argumentList>\r\n"															\
						"<argument>\r\n"																\
						"<name>InstanceID</name>\r\n"													\
						"<direction>in</direction>\r\n" 												\
						"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"</argumentList>\r\n"															\
						"</action>\r\n" 																\
						"<action>\r\n"																	\
						"<name>Seek</name>\r\n" 														\
						"<argumentList>\r\n"															\
						"<argument>\r\n"																\
						"<name>InstanceID</name>\r\n"													\
						"<direction>in</direction>\r\n" 												\
						"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>Unit</name>\r\n" 														\
						"<direction>in</direction>\r\n" 												\
						"<relatedStateVariable>A_ARG_TYPE_SeekMode</relatedStateVariable>\r\n"			\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>Target</name>\r\n"														\
						"<direction>in</direction>\r\n" 												\
						"<relatedStateVariable>A_ARG_TYPE_SeekTarget</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"</argumentList>\r\n"															\
						"</action>\r\n" 																\
						"<action>\r\n"																	\
						"<name>Next</name>\r\n" 														\
						"<argumentList>\r\n"															\
						"<argument>\r\n"																\
						"<name>InstanceID</name>\r\n"													\
						"<direction>in</direction>\r\n" 												\
						"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"</argumentList>\r\n"															\
						"</action>\r\n" 																\
						"<action>\r\n"																	\
						"<name>Previous</name>\r\n" 													\
						"<argumentList>\r\n"															\
						"<argument>\r\n"																\
						"<name>InstanceID</name>\r\n"													\
						"<direction>in</direction>\r\n" 												\
						"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"</argumentList>\r\n"															\
						"</action>\r\n" 																\
						"<action>\r\n"																	\
						"<name>GetCurrentTransportActions</name>\r\n"									\
						"<argumentList>\r\n"															\
						"<argument>\r\n"																\
						"<name>InstanceID</name>\r\n"													\
						"<direction>in</direction>\r\n" 												\
						"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"<argument>\r\n"																\
						"<name>Actions</name>\r\n"														\
						"<direction>out</direction>\r\n"												\
						"<relatedStateVariable>CurrentTransportActions</relatedStateVariable>\r\n"		\
						"</argument>\r\n"																\
						"</argumentList>\r\n"															\
						"</action>\r\n" 																\
						"</actionList>\r\n" 															\
						"<serviceStateTable>\r\n"														\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>TransportState</name>\r\n"												\
						"<dataType>string</dataType>\r\n"												\
						"<allowedValueList>\r\n"														\
						"<allowedValue>STOPPED</allowedValue>\r\n"										\
						"<allowedValue>PAUSED_PLAYBACK</allowedValue>\r\n"								\
						"<allowedValue>PAUSED_RECORDING</allowedValue>\r\n" 							\
						"<allowedValue>PLAYING</allowedValue>\r\n"										\
						"<allowedValue>RECORDING</allowedValue>\r\n"									\
						"<allowedValue>TRANSITIONING</allowedValue>\r\n"								\
						"<allowedValue>NO_MEDIA_PRESENT</allowedValue>\r\n" 							\
						"</allowedValueList>\r\n"														\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>TransportStatus</name>\r\n"												\
						"<dataType>string</dataType>\r\n"												\
						"<allowedValueList>\r\n"														\
						"<allowedValue>OK</allowedValue>\r\n"											\
						"<allowedValue>ERROR_OCCURRED</allowedValue>\r\n"								\
						"</allowedValueList>\r\n"														\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>PlaybackStorageMedium</name>\r\n"										\
						"<dataType>string</dataType>\r\n"												\
						"<allowedValueList>\r\n"														\
						"<allowedValue>UNKNOWN</allowedValue>\r\n"										\
						"<allowedValue>DV</allowedValue>\r\n"											\
						"<allowedValue>MINI-DV</allowedValue>\r\n"										\
						"<allowedValue>VHS</allowedValue>\r\n"											\
						"<allowedValue>W-VHS</allowedValue>\r\n"										\
						"<allowedValue>S-VHS</allowedValue>\r\n"										\
						"<allowedValue>D-VHS</allowedValue>\r\n"										\
						"<allowedValue>VHSC</allowedValue>\r\n" 										\
						"<allowedValue>VIDEO8</allowedValue>\r\n"										\
						"<allowedValue>HI8</allowedValue>\r\n"											\
						"<allowedValue>CD-ROM</allowedValue>\r\n"										\
						"<allowedValue>CD-DA</allowedValue>\r\n"										\
						"<allowedValue>CD-R</allowedValue>\r\n" 										\
						"<allowedValue>CD-RW</allowedValue>\r\n"										\
						"<allowedValue>VIDEO-CD</allowedValue>\r\n" 									\
						"<allowedValue>SACD</allowedValue>\r\n" 										\
						"<allowedValue>MD-AUDIO</allowedValue>\r\n" 									\
						"<allowedValue>MD-PICTURE</allowedValue>\r\n"									\
						"<allowedValue>DVD-ROM</allowedValue>\r\n"										\
						"<allowedValue>DVD-VIDEO</allowedValue>\r\n"									\
						"<allowedValue>DVD-R</allowedValue>\r\n"										\
						"<allowedValue>DVD+RW</allowedValue>\r\n"										\
						"<allowedValue>DVD-RW</allowedValue>\r\n"										\
						"<allowedValue>DVD-RAM</allowedValue>\r\n"										\
						"<allowedValue>DVD-AUDIO</allowedValue>\r\n"									\
						"<allowedValue>DAT</allowedValue>\r\n"											\
						"<allowedValue>LD</allowedValue>\r\n"											\
						"<allowedValue>HDD</allowedValue>\r\n"											\
						"<allowedValue>MICRO-MV</allowedValue>\r\n" 									\
						"<allowedValue>NETWORK</allowedValue>\r\n"										\
						"<allowedValue>NONE</allowedValue>\r\n" 										\
						"<allowedValue>NOT_IMPLEMENTED</allowedValue>\r\n"								\
						"<allowedValue>SD</allowedValue>\r\n"											\
						"<allowedValue>PC-CARD</allowedValue>\r\n"										\
						"<allowedValue>MMC</allowedValue>\r\n"											\
						"<allowedValue>CF</allowedValue>\r\n"											\
						"<allowedValue>BD</allowedValue>\r\n"											\
						"<allowedValue>MS</allowedValue>\r\n"											\
						"<allowedValue>HD_DVD</allowedValue>\r\n"										\
						"</allowedValueList>\r\n"														\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>RecordStorageMedium</name>\r\n"											\
						"<dataType>string</dataType>\r\n"												\
						"<allowedValueList>\r\n"														\
						"<allowedValue>UNKNOWN</allowedValue>\r\n"										\
						"<allowedValue>DV</allowedValue>\r\n"											\
						"<allowedValue>MINI-DV</allowedValue>\r\n"										\
						"<allowedValue>VHS</allowedValue>\r\n"											\
						"<allowedValue>W-VHS</allowedValue>\r\n"										\
						"<allowedValue>S-VHS</allowedValue>\r\n"										\
						"<allowedValue>D-VHS</allowedValue>\r\n"										\
						"<allowedValue>VHSC</allowedValue>\r\n" 										\
						"<allowedValue>VIDEO8</allowedValue>\r\n"										\
						"<allowedValue>HI8</allowedValue>\r\n"											\
						"<allowedValue>CD-ROM</allowedValue>\r\n"										\
						"<allowedValue>CD-DA</allowedValue>\r\n"										\
						"<allowedValue>CD-R</allowedValue>\r\n" 										\
						"<allowedValue>CD-RW</allowedValue>\r\n"										\
						"<allowedValue>VIDEO-CD</allowedValue>\r\n" 									\
						"<allowedValue>SACD</allowedValue>\r\n" 										\
						"<allowedValue>MD-AUDIO</allowedValue>\r\n" 									\
						"<allowedValue>MD-PICTURE</allowedValue>\r\n"									\
						"<allowedValue>DVD-ROM</allowedValue>\r\n"										\
						"<allowedValue>DVD-VIDEO</allowedValue>\r\n"									\
						"<allowedValue>DVD-R</allowedValue>\r\n"										\
						"<allowedValue>DVD+RW</allowedValue>\r\n"										\
						"<allowedValue>DVD-RW</allowedValue>\r\n"										\
						"<allowedValue>DVD-RAM</allowedValue>\r\n"										\
						"<allowedValue>DVD-AUDIO</allowedValue>\r\n"									\
						"<allowedValue>DAT</allowedValue>\r\n"											\
						"<allowedValue>LD</allowedValue>\r\n"											\
						"<allowedValue>HDD</allowedValue>\r\n"											\
						"<allowedValue>MICRO-MV</allowedValue>\r\n" 									\
						"<allowedValue>NETWORK</allowedValue>\r\n"										\
						"<allowedValue>NONE</allowedValue>\r\n" 										\
						"<allowedValue>NOT_IMPLEMENTED</allowedValue>\r\n"								\
						"<allowedValue>SD</allowedValue>\r\n"											\
						"<allowedValue>PC-CARD</allowedValue>\r\n"										\
						"<allowedValue>MMC</allowedValue>\r\n"											\
						"<allowedValue>CF</allowedValue>\r\n"											\
						"<allowedValue>BD</allowedValue>\r\n"											\
						"<allowedValue>MS</allowedValue>\r\n"											\
						"<allowedValue>HD_DVD</allowedValue>\r\n"										\
						"</allowedValueList>\r\n"														\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>CurrentPlayMode</name>\r\n"												\
						"<dataType>string</dataType>\r\n"												\
						"<allowedValueList>\r\n"														\
						"<allowedValue>NORMAL</allowedValue>\r\n"										\
						"<allowedValue>SHUFFLE</allowedValue>\r\n"										\
						"<allowedValue>REPEAT_ONE</allowedValue>\r\n"									\
						"<allowedValue>REPEAT_ALL</allowedValue>\r\n"									\
						"<allowedValue>RANDOM</allowedValue>\r\n"										\
						"<allowedValue>DIRECT_1</allowedValue>\r\n" 									\
						"<allowedValue>INTRO</allowedValue>\r\n"										\
						"</allowedValueList>\r\n"														\
						"<defaultValue>NORMAL</defaultValue>\r\n"										\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>TransportPlaySpeed</name>\r\n"											\
						"<dataType>string</dataType>\r\n"												\
						"<allowedValueList>\r\n"														\
						"<allowedValue>1</allowedValue>\r\n"											\
						"</allowedValueList>\r\n"														\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>RecordMediumWriteStatus</name>\r\n"										\
						"<dataType>string</dataType>\r\n"												\
						"<allowedValueList>\r\n"														\
						"<allowedValue>WRITABLE</allowedValue>\r\n" 									\
						"<allowedValue>PROTECTED</allowedValue>\r\n"									\
						"<allowedValue>NOT_WRITABLE</allowedValue>\r\n" 								\
						"<allowedValue>UNKNOWN</allowedValue>\r\n"										\
						"<allowedValue>NOT_IMPLEMENTED</allowedValue>\r\n"								\
						"</allowedValueList>\r\n"														\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>CurrentRecordQualityMode</name>\r\n" 									\
						"<dataType>string</dataType>\r\n"												\
						"<allowedValueList>\r\n"														\
						"<allowedValue>0:EP</allowedValue>\r\n" 										\
						"<allowedValue>1:LP</allowedValue>\r\n" 										\
						"<allowedValue>2:SP</allowedValue>\r\n" 										\
						"<allowedValue>0:BASIC</allowedValue>\r\n"										\
						"<allowedValue>1:MEDIUM</allowedValue>\r\n" 									\
						"<allowedValue>2:HIGH</allowedValue>\r\n"										\
						"<allowedValue>NOT_IMPLEMENTED</allowedValue>\r\n"								\
						"</allowedValueList>\r\n"														\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>NumberOfTracks</name>\r\n"												\
						"<dataType>ui4</dataType>\r\n"													\
						"<allowedValueRange>\r\n"														\
						"<minimum>0</minimum>\r\n"														\
						"<maximum> 50 </maximum>\r\n"													\
						"</allowedValueRange>\r\n"														\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>CurrentTrack</name>\r\n" 												\
						"<dataType>ui4</dataType>\r\n"													\
						"<allowedValueRange>\r\n"														\
						"<minimum>0</minimum>\r\n"														\
						"<maximum> 50 </maximum>\r\n"													\
						"<step>1</step>\r\n"															\
						"</allowedValueRange>\r\n"														\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>CurrentTrackDuration</name>\r\n" 										\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>CurrentMediaDuration</name>\r\n" 										\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>CurrentTrackMetaData</name>\r\n" 										\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>CurrentTrackURI</name>\r\n"												\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>AVTransportURI</name>\r\n"												\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>AVTransportURIMetaData</name>\r\n"										\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>NextAVTransportURI</name>\r\n"											\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>NextAVTransportURIMetaData</name>\r\n"									\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>RelativeTimePosition</name>\r\n" 										\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>AbsoluteTimePosition</name>\r\n" 										\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>RelativeCounterPosition</name>\r\n"										\
						"<dataType>i4</dataType>\r\n"													\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>AbsoluteCounterPosition</name>\r\n"										\
						"<dataType>i4</dataType>\r\n"													\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>A_ARG_TYPE_SeekMode</name>\r\n"											\
						"<dataType>string</dataType>\r\n"												\
						"<allowedValueList>\r\n"														\
						"<allowedValue>REL_TIME</allowedValue>\r\n" 									\
						"<allowedValue>TRACK_NR</allowedValue>\r\n" 									\
						"</allowedValueList>\r\n"														\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>A_ARG_TYPE_SeekTarget</name>\r\n"										\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>A_ARG_TYPE_InstanceID</name>\r\n"										\
						"<dataType>ui4</dataType>\r\n"													\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>PossiblePlaybackStorageMedia</name>\r\n" 								\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>PossibleRecordStorageMedia</name>\r\n"									\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>PossibleRecordQualityModes</name>\r\n"									\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"no\">\r\n" 										\
						"<name>CurrentTransportActions</name>\r\n"										\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"<stateVariable sendEvents=\"yes\">\r\n"										\
						"<name>LastChange</name>\r\n"													\
						"<dataType>string</dataType>\r\n"												\
						"</stateVariable>\r\n"															\
						"</serviceStateTable>\r\n"														\
						"</scpd>\r\n"
				
#define AVTRANSPORT_DESCRIPTION_LEN strlen(AVTRANSPORT_DESCRIPTION)
				
#define RENDERING_CONTROL_SERVICE_LOCATION "/upnp/service/Rendering_Control_Service.xml"
				
#define RENDERING_CONTROL_SERVICE_DESCRIPTION "<?xml version=\"1.0\"?>\r\n"                                                \
						"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\r\n" 				 \
						"<specVersion>\r\n" 													 \
						"<major>1</major>\r\n"													 \
						"<minor>0</minor>\r\n"													 \
						"</specVersion>\r\n"													 \
						"<actionList>\r\n"														 \
						"<action>\r\n"															 \
						"<name>ListPresets</name>\r\n"											 \
						"<argumentList>\r\n"													 \
						"<argument>\r\n"														 \
						"<name>InstanceID</name>\r\n"											 \
						"<direction>in</direction>\r\n" 										 \
						"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\r\n" \
						"</argument>\r\n"														 \
						"<argument>\r\n"														 \
						"<name>CurrentPresetNameList</name>\r\n"								 \
						"<direction>out</direction>\r\n"										 \
						"<relatedStateVariable>PresetNameList</relatedStateVariable>\r\n"		 \
						"</argument>\r\n"														 \
						"</argumentList>\r\n"													 \
						"</action>\r\n "														 \
						"<action>\r\n"															 \
						"<name>SelectPreset</name>\r\n" 										 \
						"<argumentList>\r\n"													 \
						"<argument>\r\n"														 \
						"<name>InstanceID</name>\r\n"											 \
						"<direction>in</direction>\r\n" 										 \
						"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\r\n" \
						"</argument>\r\n"														 \
						"<argument>\r\n"														 \
						"<name>PresetName</name>\r\n"											 \
						"<direction>in</direction>\r\n" 										 \
						"<relatedStateVariable>A_ARG_TYPE_PresetName</relatedStateVariable>\r\n" \
						"</argument>\r\n"														 \
						"</argumentList>\r\n"													 \
						"</action>\r\n "														 \
						"<action>\r\n"															 \
						"<name>GetMute</name>\r\n"												 \
						"<argumentList>\r\n"													 \
						"<argument>\r\n"														 \
						"<name>InstanceID</name>\r\n"											 \
						"<direction>in</direction>\r\n" 										 \
						"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\r\n" \
						"</argument>\r\n"														 \
						"<argument>\r\n"														 \
						"<name>Channel</name>\r\n"												 \
						"<direction>in</direction>\r\n" 										 \
						"<relatedStateVariable>A_ARG_TYPE_Channel</relatedStateVariable>\r\n"	 \
						"</argument>\r\n"														 \
						"<argument>\r\n"														 \
						"<name>CurrentMute</name>\r\n"											 \
						"<direction>out</direction>\r\n"										 \
						"<relatedStateVariable>Mute</relatedStateVariable>\r\n" 				 \
						"</argument>\r\n"														 \
						"</argumentList>\r\n"													 \
						"</action>\r\n" 														 \
						"<action>\r\n"															 \
						"<name>SetMute</name>\r\n"												 \
						"<argumentList>\r\n"													 \
						"<argument>\r\n"														 \
						"<name>InstanceID</name>\r\n"											 \
						"<direction>in</direction>\r\n" 										 \
						"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\r\n" \
						"</argument>\r\n"														 \
						"<argument>\r\n"														 \
						"<name>Channel</name>\r\n"												 \
						"<direction>in</direction>\r\n" 										 \
						"<relatedStateVariable>A_ARG_TYPE_Channel</relatedStateVariable>\r\n"	 \
						"</argument>\r\n"														 \
						"<argument>\r\n"														 \
						"<name>DesiredMute</name>\r\n"											 \
						"<direction>in</direction>\r\n" 										 \
						"<relatedStateVariable>Mute</relatedStateVariable>\r\n" 				 \
						"</argument>\r\n"														 \
						"</argumentList>\r\n"													 \
						"</action>\r\n" 														 \
						"<action>\r\n"															 \
						"<name>GetVolume</name>\r\n"											 \
						"<argumentList>\r\n"													 \
						"<argument>\r\n"														 \
						"<name>InstanceID</name>\r\n"											 \
						"<direction>in</direction>\r\n" 										 \
						"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\r\n" \
						"</argument>\r\n"														 \
						"<argument>\r\n"														 \
						"<name>Channel</name>\r\n"												 \
						"<direction>in</direction>\r\n" 										 \
						"<relatedStateVariable>A_ARG_TYPE_Channel</relatedStateVariable>\r\n"	 \
						"</argument>\r\n"														 \
						"<argument>\r\n"														 \
						"<name>CurrentVolume</name>\r\n"										 \
						"<direction>out</direction>\r\n"										 \
						"<relatedStateVariable>Volume</relatedStateVariable>\r\n"				 \
						"</argument>\r\n"														 \
						"</argumentList>\r\n"													 \
						"</action>\r\n" 														 \
						"<action>\r\n"															 \
						"<name>SetVolume</name>\r\n"											 \
						"<argumentList>\r\n"													 \
						"<argument>\r\n"														 \
						"<name>InstanceID</name>\r\n"											 \
						"<direction>in</direction>\r\n" 										 \
						"<relatedStateVariable>A_ARG_TYPE_InstanceID</relatedStateVariable>\r\n" \
						"</argument>\r\n"														 \
						"<argument>\r\n"														 \
						"<name>Channel</name>\r\n"												 \
						"<direction>in</direction>\r\n" 										 \
						"<relatedStateVariable>A_ARG_TYPE_Channel</relatedStateVariable>\r\n"	 \
						"</argument>\r\n"														 \
						"<argument>\r\n"														 \
						"<name>DesiredVolume</name>\r\n"										 \
						"<direction>in</direction>\r\n" 										 \
						"<relatedStateVariable>Volume</relatedStateVariable>\r\n"				 \
						"</argument>\r\n"														 \
						"</argumentList>\r\n"													 \
						"</action>\r\n" 														 \
						"</actionList>\r\n" 													 \
						"<serviceStateTable>\r\n"												 \
						"<stateVariable sendEvents=\"no\">\r\n" 								 \
						"<name>Mute</name>\r\n" 												 \
						"<dataType>boolean</dataType>\r\n"										 \
						"</stateVariable>\r\n"													 \
						"<stateVariable sendEvents=\"no\">\r\n" 								 \
						"<name>Volume</name>\r\n"												 \
						"<dataType>ui2</dataType>\r\n"											 \
						"<allowedValueRange>\r\n"												 \
						"<minimum>0</minimum>\r\n"												 \
						"<maximum>100</maximum>\r\n"											 \
						"<step>1</step>\r\n"													 \
						"</allowedValueRange>\r\n"												 \
						"</stateVariable>\r\n"													 \
						"<stateVariable sendEvents=\"no\">\r\n" 								 \
						"<name>A_ARG_TYPE_Channel</name>\r\n"									 \
						"<dataType>string</dataType>\r\n"										 \
						"<allowedValueList>\r\n"												 \
						"<allowedValue>Master</allowedValue>\r\n"								 \
						"<allowedValue>LF</allowedValue>\r\n"									 \
						"<allowedValue>RF</allowedValue>\r\n"									 \
						"<allowedValue>CF</allowedValue>\r\n"									 \
						"<allowedValue>LFE</allowedValue>\r\n"									 \
						"<allowedValue>LS</allowedValue>\r\n"									 \
						"<allowedValue>RS</allowedValue>\r\n"									 \
						"<allowedValue>LFC</allowedValue>\r\n"									 \
						"<allowedValue>RFC</allowedValue>\r\n"									 \
						"<allowedValue>SD</allowedValue>\r\n"									 \
						"<allowedValue>SL</allowedValue>\r\n"									 \
						"<allowedValue>SR </allowedValue>\r\n"									 \
						"<allowedValue>T</allowedValue>\r\n"									 \
						"<allowedValue>B</allowedValue>\r\n"									 \
						"</allowedValueList>\r\n"												 \
						"</stateVariable>\r\n"													 \
						"<stateVariable sendEvents=\"no\">\r\n" 								 \
						"<name>A_ARG_TYPE_InstanceID</name>\r\n"								 \
						"<dataType>ui4</dataType>\r\n"											 \
						"</stateVariable>\r\n"													 \
						"<stateVariable>\r\n"													 \
						"<name>A_ARG_TYPE_header</name>\r\n"									 \
						"<sendEventsAttribute>no</sendEventsAttribute>\r\n" 					 \
						"<dataType>string</dataType>\r\n"										 \
						"</stateVariable>\r\n"													 \
						"<stateVariable>\r\n"													 \
						"<name>A_ARG_TYPE_Keycode</name>\r\n"									 \
						"<sendEventsAttribute>no</sendEventsAttribute>\r\n" 					 \
						"<dataType>string</dataType>\r\n"										 \
						"</stateVariable>\r\n"													 \
						"<stateVariable sendEvents=\"yes\">\r\n"								 \
						"<name>LastChange</name>\r\n"											 \
						"<dataType>string</dataType>\r\n"										 \
						"</stateVariable>\r\n"													 \
						"<stateVariable sendEvents=\"no\">\r\n" 								 \
						"<name>PresetNameList</name>\r\n"										 \
						"<dataType>string</dataType>\r\n"										 \
						"</stateVariable>\r\n"													 \
						"<stateVariable sendEvents=\"no\">\r\n" 								 \
						"<name>A_ARG_TYPE_PresetName</name>\r\n"								 \
						"<dataType>string</dataType>\r\n"										 \
						"<allowedValueList>\r\n"												 \
						"<allowedValue>FactoryDefaults</allowedValue>\r\n"						 \
						"<allowedValue>InstallationDefaults</allowedValue>\r\n" 				 \
						"<allowedValue>Vendor defined</allowedValue>\r\n"						 \
						"</allowedValueList>\r\n"												 \
						"</stateVariable>\r\n"													 \
						"</serviceStateTable>\r\n"												 \
						"</scpd>\r\n"
				
#define RENDERING_CONTROL_SERVICE_DESCRIPTION_LEN strlen(RENDERING_CONTROL_SERVICE_DESCRIPTION)

}
}
}
}
#endif
