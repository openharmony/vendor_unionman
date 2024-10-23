/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: dmr listener
 * Author: 
 * Create: 2023-10-20
 */

#ifndef DMR_LISTENER_H
#define DMR_LISTENER_H

#include <vector>
#include <unistd.h>
#include "dlna_types.h"
#include <string>
#include "i_dlna_listener.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

using namespace  std;

class DmrListener {
public:
	static DmrListener &getInstance();
    DmrListener();
	void SetListener(std::shared_ptr<IDlnaListener> listener);
	static unsigned int SetMediaUri(MediaUrlData mediaUrlData);
    static unsigned int Play(const PlayData *data);
	static unsigned int Pause(const PauseData *data);
	static unsigned int Stop(const StopData *data);
	static unsigned int Seek(const SeekData *data);
	static char *GetCurrTime();
	static char *GetAllTime();
	static unsigned int SetVol(unsigned int ulDesiredVol, const char *pcChannel);
	static unsigned int GetVol(int *ulVol, const char *pcChannel);
	static unsigned int SetMute(bool bDesiredMute, const char *pcChannel);
	bool SendEventChange(int moduleId, int event, const std::string &param);
	
private:
	const std::string ACTION_PLAY = "play";
    const std::string ACTION_LOAD = "load";
    const std::string ACTION_PAUSE = "pause";
    const std::string ACTION_RESUME = "resume";
    const std::string ACTION_STOP = "stop";
    const std::string ACTION_NEXT = "next";
    const std::string ACTION_PREVIOUS = "previous";
    const std::string ACTION_SEEK = "seek";
    const std::string ACTION_SET_VOLUME = "setVolume";
    const std::string ACTION_SET_REPEAT_MODE = "setRepeatMode";
    const std::string ACTION_SET_SPEED = "setSpeed";
    const std::string ACTION_PLAYER_STATUS_CHANGED = "onPlayerStatusChanged";
    const std::string ACTION_POSITION_CHANGED = "onPositionChanged";
    const std::string ACTION_MEDIA_ITEM_CHANGED = "onMediaItemChanged";
    const std::string ACTION_VOLUME_CHANGED = "onVolumeChanged";
    const std::string ACTION_REPEAT_MODE_CHANGED = "onRepeatModeChanged";
    const std::string ACTION_SPEED_CHANGED = "onPlaySpeedChanged";
    const std::string ACTION_PLAYER_ERROR = "onPlayerError";
    const std::string ACTION_NEXT_REQUEST = "onNextRequest";
    const std::string ACTION_PREVIOUS_REQUEST = "onPreviousRequest";
    const std::string ACTION_SEEK_DONE = "onSeekDone";
    const std::string ACTION_END_OF_STREAM = "onEndOfStream";
    
};

}
}
}
}
#endif