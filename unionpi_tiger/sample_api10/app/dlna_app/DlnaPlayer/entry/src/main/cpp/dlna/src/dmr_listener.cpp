/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: dlna listener
 * Author: 
 * Create: 2023-10-20
 */

#include <unistd.h>
#include <fcntl.h>
#include "dlna_log.h"
// #include "cast_engine_common.h"
#include "dmr_listener.h"
#include "dmr_av_transport.h"
#include "dmr_rendering_control.h"
#include "json.hpp"
#include "securec.h"
using namespace std;

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {
	using nlohmann::json;
	using namespace std;
	DlnaDmrAction dmrState_;
	int position_;
	int duration_;
	int curVolume_;
	int maxVolume_;
	char curtime_[64];
	char alltime_[64];
	const std::string KEY_ACTION = "ACTION";
    const std::string KEY_CALLBACK_ACTION = "CALLBACK_ACTION";
    const std::string KEY_DATA = "DATA";
    const std::string KEY_CURRENT_INDEX = "CURRENT_INDEX";
    const std::string KEY_PROGRESS_INTERVAL = "PROGRESS_INTERVAL";
    const std::string KEY_LIST = "LIST";
    const std::string KEY_MEDIA_ID = "MEDIA_ID";
    const std::string KEY_MEDIA_NAME = "MEDIA_NAME";
    const std::string KEY_MEDIA_URL = "MEDIA_URL";
    const std::string KEY_MEDIA_TYPE = "MEDIA_TYPE";
    const std::string KEY_MEDIA_SIZE = "MEDIA_SIZE";
    const std::string KEY_START_POSITION = "START_POSITION";
    const std::string KEY_DURATION = "DURATION";
    const std::string KEY_CLOSING_CREDITS_POSITION = "CLOSING_CREDITS_POSITION";
    const std::string KEY_ALBUM_COVER_URL = "ALBUM_COVER_URL";
    const std::string KEY_ALBUM_TITLE = "ALBUM_TITLE";
    const std::string KEY_MEDIA_ARTIST = "MEDIA_ARTIST";
    const std::string KEY_LRC_URL = "LRC_URL";
    const std::string KEY_LRC_CONTENT = "LRC_CONTENT";
    const std::string KEY_APP_ICON_URL = "APP_ICON_URL";
    const std::string KEY_APP_NAME = "APP_NAME";
    const std::string KEY_VOLUME = "VOLUME";
    const std::string KEY_MAX_VOLUME = "MAX_VOLUME";
    const std::string KEY_MODE = "MODE";
    const std::string KEY_SPEED = "SPEED";
    const std::string KEY_POSITION = "POSITION";
    const std::string KEY_BUFFER_POSITION = "BUFFER_POSITION";
    const std::string KEY_PLAY_INFO = "PLAY_INFO";
    const std::string KEY_ERROR_CODE = "ERROR_CODE";
    const std::string KEY_ERROR_MSG = "ERROR_MSG";
    const std::string KEY_PLAY_BACK_STATE = "PLAYBACK_STATE";
    const std::string KEY_IS_PLAY_WHEN_READY = "IS_PLAY_WHEN_READY";
    const std::string KEY_IS_LOOPING = "IS_LOOPING";

	
std::shared_ptr<IDlnaListener> listener_;

//DEFINE_DLNA_LABEL("Cast-dmr-listener");
#define DLNA_LOG_TAG "Dlna_dmr-listener"

DmrListener &DmrListener::getInstance()
{
	static DmrListener instance;
	return instance;
}

DmrListener::DmrListener()
{
	position_ = 0;
	duration_ = 0;	
}

void DmrListener::SetListener(std::shared_ptr<IDlnaListener> listener)
{
    OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "set listener");
	listener_ = listener;
}
	
unsigned int DmrListener::SetMediaUri(MediaUrlData mediaUrlData)
{
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "CallbackDlnaDmrSetMediaUri: %{public}s", mediaUrlData.pucUri);
	json jsondata, dataBody;
	std::string param;

	dmrState_ = DlnaDmrAction::DLNA_DMR_ACTION_INVALID;
	dataBody[KEY_MEDIA_URL] = (const char *)mediaUrlData.pucUri;
	jsondata[KEY_ACTION] = DMR_AV_TRANSPORT_ACTION_SET_AVTRANSPORT_URI;
	jsondata[KEY_DATA] = dataBody;

	param = jsondata.dump();
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "param = %{public}s", param.c_str());
	if(listener_) {
        listener_->NotifyEventChange(1009, 100, param);
	}
	return 0;
}
	
unsigned int DmrListener::Play(const PlayData *data)
{
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "CallbackDlnaDmrPlayInd .");
	json jsondata, dataBody;
	std::string param;

	dataBody[KEY_LIST] = "none";
	jsondata[KEY_ACTION] = DMR_AV_TRANSPORT_ACTION_PLAY;
	jsondata[KEY_DATA] = dataBody;

	param = jsondata.dump();
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "param = %{public}s", param.c_str());
	if(listener_) {
		listener_->NotifyEventChange(1009, 100, param);
	}
	return 0;
}
	
unsigned int DmrListener::Pause(const PauseData *data)
{
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "CallbackDlnaDmrPauseInd .");
	json jsondata, dataBody;
	std::string param;

	dataBody[KEY_LIST] = "none";
	jsondata[KEY_ACTION] = DMR_AV_TRANSPORT_ACTION_PAUSE;
	jsondata[KEY_DATA] = dataBody;

	param = jsondata.dump();
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "param = %{public}s", param.c_str());
	if(listener_) {
		listener_->NotifyEventChange(1009, 100, param);
	}
	return 0;
}
	
unsigned int DmrListener::Stop(const StopData *data)
{
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "CallbackDlnaDmrStopInd .");
	json jsondata, dataBody;
	std::string param;

	dataBody[KEY_LIST] = "none";
	jsondata[KEY_ACTION] = DMR_AV_TRANSPORT_ACTION_STOP;
	jsondata[KEY_DATA] = dataBody;

	param = jsondata.dump();
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "param = %{public}s", param.c_str());
	if(listener_) {
		listener_->NotifyEventChange(1009, 100, param);
	}
	return 0;
}
	
unsigned int DmrListener::Seek(const SeekData *data)
{
	json jsondata, dataBody;
	std::string param;
	std::string positionString;
	int position;

	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "data->seekMode = %{public}d, data->dataType = %{public}d, data->clientIP = %{public}s", data->seekMode, data->dataType, data->clientIP);
	if (data->dataType == DlnaParaType::DLNA_PARA_TYPE_STRING) {
		OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "pucBuf = %{public}s", data->seekTarget->stStrVal.pucBuf);
		int hour,min,second;
		sscanf_s((char *)data->seekTarget->stStrVal.pucBuf, "%d:%d:%d", &hour, &min, &second);
		position = hour*3600 + min*60 + second;
		OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "hour = %{public}d, min = %{public}d, second = %{public}d, position = %{public}d", hour, min, second, position);
	} else if (data->dataType == DlnaParaType::DLNA_PARA_TYPE_INT32) {
		position = data->seekTarget->iVal;
		OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "iVal = %{public}d, position = %{public}d", data->seekTarget->iVal, position);
	} else if (data->dataType == DlnaParaType::DLNA_PARA_TYPE_UINT32) {
		position = data->seekTarget->ulVal;
		OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "ulVal = %{public}d, position = %{public}u", data->seekTarget->ulVal, position);
	} else if (data->dataType == DlnaParaType::DLNA_PARA_TYPE_FLOAT) {
		OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "fVal = %{public}f", data->seekTarget->fVal);
	} else {
		OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "no support");
		return 0;
	}
	dataBody[KEY_POSITION] = position;
	jsondata[KEY_ACTION] = DMR_AV_TRANSPORT_ACTION_SEEK;
	jsondata[KEY_DATA] = dataBody;

	param = jsondata.dump();
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "param = %{public}s", param.c_str());
	if(listener_) {
		listener_->NotifyEventChange(1009, 100, param);
	}
	return 0;
}
	
char* DmrListener::GetCurrTime()
{
	sprintf(curtime_, "%02d:%02d:%02d", position_/3600, (position_%3600)/60, position_%60);
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "position_ = %{public}d, curtime_ = %{public}s", position_, curtime_);
	return curtime_;
}
	
char* DmrListener::GetAllTime()
{
	sprintf(alltime_, "%02d:%02d:%02d", duration_/3600, (duration_%3600)/60, duration_%60);
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "duration_ = %{public}d, alltime_ = %{public}s", duration_, alltime_);
	return alltime_;
}
	
unsigned int DmrListener::SetVol(unsigned int ulDesiredVol, const char *pcChannel)
{
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "in ");
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "ulDesiredVol = %{public}d, pcChannel = %{public}s", ulDesiredVol, pcChannel);
	json jsondata, dataBody;
	std::string param;
	dataBody[KEY_VOLUME] = ulDesiredVol;
	jsondata[KEY_ACTION] = DMR_RENDER_CONTROL_ACTION_SET_VOLUME;
	jsondata[KEY_DATA] = dataBody;

	param = jsondata.dump();
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "param = %{public}s", param.c_str());
	if(listener_) {
		listener_->NotifyEventChange(1009, 100, param);
	}
	return 0;
}
	
unsigned int DmrListener::GetVol(int *ulVol, const char *pcChannel)
{
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "curVolume_ = %{public}d", curVolume_);
	return curVolume_;
}
	
unsigned int DmrListener::SetMute(bool bDesiredMute, const char *pcChannel)
{
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "SetMute .");
	return 0;
}

bool DmrListener::SendEventChange(int moduleId, int event, const std::string &param)
{
	OH_LOG_Print(LOG_APP, LOG_DEBUG, 0xFF00, DLNA_LOG_TAG,  "param = %{public}s", param.c_str());
	json body, body1;
    std::string tmpstring;
	
    if (!body.accept(param)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "something wrong for the json data!");
        return false;
    }
    body = json::parse(param, nullptr, false);
	if(body.contains(KEY_CALLBACK_ACTION)) {
		tmpstring = body[KEY_CALLBACK_ACTION];
		OH_LOG_Print(LOG_APP, LOG_DEBUG, 0xFF00, DLNA_LOG_TAG,  "body[KEY_CALLBACK_ACTION] = %{public}s", tmpstring.c_str());
		if(tmpstring.compare(ACTION_PLAYER_STATUS_CHANGED) == 0) {
			if(body.contains(KEY_DATA))
			{
				body1 = body[KEY_DATA];
				if (body1.contains(KEY_IS_PLAY_WHEN_READY) && body1[KEY_IS_PLAY_WHEN_READY].is_boolean()) {
					bool ready = body1[KEY_IS_PLAY_WHEN_READY];
					OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "ready = %{public}d", ready);	
				}
				if (body1.contains(KEY_PLAY_BACK_STATE) && body1[KEY_PLAY_BACK_STATE].is_number()) {
					PlayerStates state = body1[KEY_PLAY_BACK_STATE];
					switch(state) {
						case PlayerStates::PLAYER_STATE_ERROR:
					    case PlayerStates::PLAYER_IDLE:
							dmrState_ = DlnaDmrAction::DLNA_DMR_ACTION_INVALID;
							break;
					    case PlayerStates::PLAYER_INITIALIZED:
					    case PlayerStates::PLAYER_PREPARING:
					    case PlayerStates::PLAYER_PREPARED:
							dmrState_ = DlnaDmrAction::DLNA_DMR_ACTION_OPEN;
							break;
					    case PlayerStates::PLAYER_STARTED:
						case PlayerStates::PLAYER_PLAYBACK_COMPLETE:	
							dmrState_ = DlnaDmrAction::DLNA_DMR_ACTION_PLAY;
							break;
					    case PlayerStates::PLAYER_PAUSED:
							dmrState_ = DlnaDmrAction::DLNA_DMR_ACTION_PAUSE;
							break;
					    case PlayerStates::PLAYER_STOPPED:
							dmrState_ = DlnaDmrAction::DLNA_DMR_ACTION_STOP;
							break;
					    case PlayerStates::PLAYER_RELEASED:
							dmrState_ = DlnaDmrAction::DLNA_DMR_ACTION_FINISH;
							break;
						default:
							dmrState_ = DlnaDmrAction::DLNA_DMR_ACTION_INVALID;
							break;
					}
					OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "state = %{public}d, dmrState_ = %{public}d", state, dmrState_);
					DmrAVTransport::getInstance().SetCurrPlayState(dmrState_);
				}
			}	
		} else if(tmpstring.compare(ACTION_POSITION_CHANGED) == 0) {
			if(body.contains(KEY_DATA))
			{
				body1 = body[KEY_DATA];
				if (body1.contains(KEY_POSITION) && body1[KEY_POSITION].is_number()) {
					int tmp = body1[KEY_POSITION];
					position_ = tmp/1000;
					OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "position_ = %{public}d", position_);	
				}
				if (body1.contains(KEY_DURATION) && body1[KEY_DURATION].is_number()) {
					int tmp = body1[KEY_DURATION];
					duration_ = tmp/1000;
					OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "position_ = %{public}d", duration_);	
				}
			}
		} else if(tmpstring.compare(ACTION_VOLUME_CHANGED) == 0) {
			if(body.contains(KEY_DATA))
			{
				body1 = body[KEY_DATA];
				if (body1.contains(KEY_MAX_VOLUME) && body1[KEY_MAX_VOLUME].is_number()) {
					maxVolume_ = body1[KEY_MAX_VOLUME];
					OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "maxVolume_ = %{public}d", maxVolume_);	
				}
				if (body1.contains(KEY_VOLUME) && body1[KEY_VOLUME].is_number()) {
					curVolume_ = body1[KEY_VOLUME];
					OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "curVolume_ = %{public}d", curVolume_);	
				}
			}
		}
	} else if(body.contains(KEY_ACTION)) {
		tmpstring = body[KEY_ACTION];
		OH_LOG_Print(LOG_APP, LOG_DEBUG, 0xFF00, DLNA_LOG_TAG,  "body[KEY_ACTION] = %{public}s", tmpstring.c_str());
		if(tmpstring.compare(ACTION_SEEK_DONE) == 0) {
			DmrAVTransport::getInstance().SetCurrPlayState(dmrState_);
		}
	}
	return true;
}
}
}
}
}