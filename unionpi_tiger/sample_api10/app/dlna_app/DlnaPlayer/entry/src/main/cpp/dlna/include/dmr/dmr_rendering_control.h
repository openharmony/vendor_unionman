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

#ifndef DMR_RENDERING_CONTROL_H
#define DMR_RENDERING_CONTROL_H

#include "dlna_types.h"
#include "dlna_common.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

inline constexpr int DLNA_DMR_MAX_VOLUME_SUPPORTED = 100;
inline constexpr int DMR_RENDER_CONTROL_PRESETNAME_FACTORY_DEFAULTS = 1;
inline constexpr int DMR_RENDER_CONTROL_PRESETNAME_INSTAL_DEFAULTS = 2;
inline constexpr int DMR_RENDER_CONTROL_CHANNEL_NUM = 8;
inline constexpr int DMR_RENDER_CONTROL_AUTO_NOTIFY_TIME = 200;

inline constexpr char *DMR_RENDER_CONTROL_ACTION_GET_MUTE = "GetMute";
inline constexpr char *DMR_RENDER_CONTROL_ACTION_SET_MUTE = "SetMute";
inline constexpr char *DMR_RENDER_CONTROL_ACTION_GET_VOLUME = "GetVolume";
inline constexpr char *DMR_RENDER_CONTROL_ACTION_SET_VOLUME = "SetVolume";
inline constexpr char *DMR_RENDER_CONTROL_ACTION_LIST_PRESETS = "ListPresets";
inline constexpr char *DMR_RENDER_CONTROL_ACTION_SELECT_PRESET = "SelectPreset";
inline constexpr char *DMR_VOLUME_CUR = "CurrentVolume";
inline constexpr char *DMR_RENDER_CONTROL_CURRENT_MUTE = "CurrentMute";
inline constexpr char *CURRENT_PRESET_NAMELIST = "CurrentPresetNameList";
inline constexpr char *PRESET_NAME = "PresetName";

/* Rendering Control Service ERROR CODE */
inline constexpr int DMR_RENDER_CONTROL_ERROR_701 = 701;
inline constexpr int DMR_RENDER_CONTROL_ERROR_702 = 702;
inline constexpr int DMR_RENDER_CONTROL_ERROR_703 = 703;
inline constexpr int DMR_RENDER_CONTROL_ERROR_704 = 704;
inline constexpr int DMR_RENDER_CONTROL_ERROR_705 = 705;
inline constexpr int DMR_RENDER_CONTROL_ERROR_706 = 706;
inline constexpr int DMR_RENDER_CONTROL_ERROR_707 = 707;
inline constexpr int DMR_RENDER_CONTROL_ERROR_708 = 708;
inline constexpr int DMR_RENDER_CONTROL_ERROR_709 = 709;
inline constexpr int DMR_RENDER_CONTROL_ERROR_710 = 710;
inline constexpr int DMR_RENDER_CONTROL_ERROR_711 = 711;
inline constexpr int DMR_RENDER_CONTROL_ERROR_712 = 712;
/* Rendering Control Service ERROR STRING */
inline constexpr char *DMR_RENDER_CONTROL_ERROR_701_STRING = "Invalid Name";
inline constexpr char *DMR_RENDER_CONTROL_ERROR_702_STRING = "Invalid InstanceID";
inline constexpr char *DMR_RENDER_CONTROL_ERROR_703_STRING = "Invalid Channel";
inline constexpr char *DMR_RENDER_CONTROL_ERROR_704_STRING = "Invalid StateVariableList";
inline constexpr char *DMR_RENDER_CONTROL_ERROR_705_STRING = "Ill-formed CSV List";
inline constexpr char *DMR_RENDER_CONTROL_ERROR_706_STRING = "Invalid State Variable Value";
inline constexpr char *DMR_RENDER_CONTROL_ERROR_707_STRING = "Invalid MediaRenderer\'s UDN";
inline constexpr char *DMR_RENDER_CONTROL_ERROR_708_STRING = "Invalid Service Type";
inline constexpr char *DMR_RENDER_CONTROL_ERROR_709_STRING = "Invalid Service Id";
inline constexpr char *DMR_RENDER_CONTROL_ERROR_710_STRING = "State Variables Specified Improperly";
inline constexpr char *DMR_RENDER_CONTROL_ERROR_711_STRING = "Invalid keycode header value";
inline constexpr char *DMR_RENDER_CONTROL_ERROR_712_STRING = "Invalid keycode value";

inline constexpr char *DMR_RENDERING_CONTROL_SERVICE_ID = "urn:upnp-org:serviceId:RenderingControl";
inline constexpr char *DMR_RENDERING_CONTROL_SERVICE_TYPE = "urn:schemas-upnp-org:service:RenderingControl:1";

inline const char *rcsSubVariable_ = { "LastChange" };

#define DMR_SET_RCS_NOTIFY_MUTE(NotifyBitSet) ((NotifyBitSet) = (NotifyBitSet) | 0x01)

#define DMR_RENDER_CONTROL_IS_MUTE_TO_NOTIFY(NotifyBitSet) ((NotifyBitSet) & 0x01)

#define DMR_SET_RCS_NOTIFY_VOLUME(NotifyBitSet) ((NotifyBitSet) = (NotifyBitSet) | 0x02)

#define DMR_RENDER_CONTROL_IS_VOLUME_TO_NOTIFY(NotifyBitSet) ((NotifyBitSet) & 0x02)

#define DMR_SET_RCS_NOTIFY_PRESETNAME(NotifyBitSet) ((NotifyBitSet) = (NotifyBitSet) | 0x04)

#define DMR_RENDER_CONTROL_IS_PRESETNAME_TO_NOTIFY(NotifyBitSet) ((NotifyBitSet) & 0x04)

#define DMR_RENDER_CONTROL_IS_ANY_STATE_VAR_TOBE_NOTIFIED(NotifyBitSet)                                 \
    (DMR_RENDER_CONTROL_IS_MUTE_TO_NOTIFY(NotifyBitSet) || DMR_RENDER_CONTROL_IS_VOLUME_TO_NOTIFY(NotifyBitSet) || \
        DMR_RENDER_CONTROL_IS_PRESETNAME_TO_NOTIFY(NotifyBitSet))

#define DMR_RENDER_CONTROL_DO_IMMEDIATE_EVENTING() if (DlnaCommon::getInstance().GetDmrHdl()->bRCSSendQuickNotify) {         \
        (void)NotifyResponseImmediately(); \
    }

class DmrRenderingControl{
public:
	static DmrRenderingControl &getInstance();
	DmrServiceAction* getActionFuncTable();
	static int CreateNotifyResponse(int isInitialNotify, char **eventRespText);
	
private:
	static int CreateOptNotifyContents(int isInitialNotify, void *pEventDoc, void *eleInstanceID);
	static int IsValidChannel(const char *pcChannel);
	static unsigned int CreateMuteContents(int isInitialNotify, void *pEventDoc, void *eleInstanceId);
	static unsigned int CreateVolumeContents(int isInitialNotify, void *pEventDoc, void *eleInstanceId);
	static void EventNotifyTimeout(void *pvArgument);
	static int NotifyResponseImmediately();
	static int GetDesiredMute(UpnpActionRequest **pstActionRequest, int *bDesiredMute,DmrActionParam *pstActionEvent, char **pcChannel);
	static int GetVolumeInit(DmrActionParam *pstActionEvent, int *iInstVal, char **pcChannel,UpnpActionRequest **pstActionRequest);
	static int GetMute(DmrActionParam *pstActionEvent);
	static int SetMute(DmrActionParam *pstActionEvent);
	static int GetVolume(DmrActionParam *pstActionEvent);
	static int SetVolume(DmrActionParam *pstActionEvent);
	static int ListPresets(DmrActionParam *pstActionEvent);
	static int SelectPreset(DmrActionParam *pstActionEvent);
	static char g_aucDlnaDmrRcsChannel[DMR_RENDER_CONTROL_CHANNEL_NUM];
};

}
}
}
}
#endif /* DMR_RENDERING_CONTROL_H */
