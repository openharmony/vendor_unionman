/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import media from '@ohos.multimedia.media';
import common from '@ohos.app.ability.common';
import { GlobalContext } from '../utils/GlobalContext';
import Logger from '../utils/Logger';
import emitter from '@ohos.events.emitter';


const CASE_ZERO = 0;
const CASE_ONE = 1;
const CASE_TWO = 2;
const CASE_THREE = 3;

export default class AvPlayManage {
  private tag: string = 'AVPlayManage';
  private flag: boolean = false;
  private avPlayer: media.AVPlayer | null = null;
  private surfaceID: string = '';
  private currentTime: number = 0; // 视频当前时间
  private durationTime: number = 0; // 视频总长
  private videoSrc: string = '';
  private fileSrc: string = '';

  /**
   * 初始化视频
   */
  async initPlayer(surfaceId: string, url: string, callback: (avPlayer: media.AVPlayer) => void): Promise<void> {
    Logger.info(this.tag, 'initPlayer==initCamera surfaceId== ${surfaceId}');
    this.surfaceID = surfaceId;
    Logger.info(this.tag, 'initPlayer==this.surfaceID surfaceId== ${this.surfaceID}');
    try {
      Logger.info(this.tag, 'initPlayer videoPlay avPlayerDemo');
      // 创建avPlayer实例对象
      this.avPlayer = await media.createAVPlayer();
      // 创建状态机变化回调函数
      await this.setAVPlayerCallback(callback);
      Logger.info(this.tag, 'initPlayer videoPlay setAVPlayerCallback');
       this.avPlayer.url = url;
    } catch (e) {
      Logger.error(this.tag, 'initPlayer initPlayer err：${e}');
    }
  }

  // 注册avplayer回调函数
  async setAVPlayerCallback(callback: (avPlayer: media.AVPlayer) => void, videoSrc?: string): Promise<void> {
    if (this.avPlayer === null) {
      return;
    }
    // seek操作结果回调函数
    this.avPlayer.on('seekDone', (seekDoneTime) => {
      Logger.info(this.tag, 'setAVPlayerCallback AVPlayer seek succeeded, seek time is ${seekDoneTime}');
    });
    // error回调监听函数,当avPlayer在操作过程中出现错误时调用reset接口触发重置流程
    // this.avPlayer.on('error', (err) => {
    //   if (this.avPlayer === null) {
    //     return;
    //   }
    //   Logger.error(this.tag, `setAVPlayerCallback Invoke avPlayer failed, code is ${err.code}, message is ${err.message}`);
    //   this.avPlayer.reset();
    // });
    // 状态机变化回调函数
    this.avPlayer.on('stateChange', async (state, reason) => {
      if (this.avPlayer === null) {
        return;
      }
      switch (state) {
        case 'idle': // 成功调用reset接口后触发该状态机上报
          this.avPlayer.release();
          Logger.info(this.tag, 'setAVPlayerCallback AVPlayer state idle called.');
          break;
        case 'initialized': // avplayer 设置播放源后触发该状态上报
          Logger.info(this.tag, 'setAVPlayerCallback AVPlayerstate initialized called.');
          this.avPlayer.surfaceId = this.surfaceID; // 设置显示画面，当播放的资源为纯音频时无需设置
          Logger.info(this.tag, 'setAVPlayerCallback this.avPlayer.surfaceId = ${this.avPlayer.surfaceId}');
          this.avPlayer.prepare();
          break;
        case 'prepared': // prepare调用成功后上报该状态机
          Logger.info(this.tag, 'setAVPlayerCallback AVPlayer state prepared called.');
          this.durationTime = this.avPlayer.duration;
          this.currentTime = this.avPlayer.currentTime;
          this.avPlayer.play(); // 调用播放接口开始播放
          Logger.info(this.tag, 'setAVPlayerCallback this.speedSelect = ${this.speedSelect}');
          callback(this.avPlayer);
          break;
        case 'playing': // play成功调用后触发该状态机上报
          Logger.info(this.tag, 'setAVPlayerCallback AVPlayer state playing called.');
          let eventDataTrue: emitter.EventData = {
            data: {
              'flag': true
            }
          };
          let innerEventTrue: emitter.InnerEvent = {
            eventId: 2,
            priority: emitter.EventPriority.HIGH
          };
          emitter.emit(innerEventTrue, eventDataTrue);
          break;
        case 'completed': // 播放结束后触发该状态机上报
          Logger.info(this.tag, 'setAVPlayerCallback AVPlayer state completed called.');
          let eventDataFalse: emitter.EventData = {
            data: {
              'flag': false
            }
          };
          let innerEvent: emitter.InnerEvent = {
            eventId: 1,
            priority: emitter.EventPriority.HIGH
          };
          emitter.emit(innerEvent, eventDataFalse);
          break;
        default:
          Logger.info(this.tag, 'setAVPlayerCallback AVPlayer state unknown called.');
          break;
      }
    });
    // 时间上报监听函数
    this.avPlayer.on('timeUpdate', (time: number) => {
      this.currentTime = time;
      Logger.info(this.tag, 'setAVPlayerCallback timeUpdate success,and new time is = ${this.currentTime}');
    });
  }

  /*
  * 设置播放的URL
   */
  setUrl(url: string):  void {
    this.avPlayer!.url = url;
  }

  /**
   * 获取总时间
   */
  getDurationTime(): number {
    return this.durationTime;
  }

  /**
   * 获取当前时间
   */
  getCurrentTime(): number {
    return this.currentTime;
  }

  /**
   * 视频播放
   */
  videoPlay(): void {
    if (this.avPlayer) {
      try {
        this.avPlayer.play();
      } catch (e) {
        Logger.error(this.tag, 'videoPlay = ${JSON.stringify(e)}');
      }
    }
  }

  /**
   * 视频暂停
   */
  videoPause(): void {
    if (this.avPlayer) {
      try {
        this.avPlayer.pause();
        Logger.info(this.tag, 'videoPause==');
      } catch (e) {
        Logger.info(this.tag, 'videoPause== ${JSON.stringify(e)}');
      }
    }
  }

  /**
   * 调节1.0倍速
   */
  videoSpeedOne(): void {
    if (this.avPlayer) {
      try {
        this.avPlayer.setSpeed(media.PlaybackSpeed.SPEED_FORWARD_1_00_X);
        Logger.info(this.tag, 'videoSpeed_1_00');
      } catch (e) {
        Logger.info(this.tag, 'videoSpeed_1_00== ${JSON.stringify(e)}');
      }
    }
  }

  /**
   * 调节1.25倍速
   */
  videoSpeedOnePointTwentyFive(): void {
    if (this.avPlayer) {
      try {
        this.avPlayer.setSpeed(media.PlaybackSpeed.SPEED_FORWARD_1_25_X);
        Logger.info(this.tag, 'videoSpeed_1_25');
      } catch (e) {
        Logger.info(this.tag, 'videoSpeed_1_25== ${JSON.stringify(e)}');
      }
    }
  }

  /**
   * 调节1.75倍速
   */
  videoSpeedOnePointSeventyFive(): void {
    if (this.avPlayer) {
      try {
        this.avPlayer.setSpeed(media.PlaybackSpeed.SPEED_FORWARD_1_75_X);
        Logger.info(this.tag, 'videoSpeed_1_75');
      } catch (e) {
        Logger.info(this.tag, 'videoSpeed_1_75==' + JSON.stringify(e));
      }
    }
  }

  /**
   * 调节2.0倍速
   */
  videoSpeedTwo(): void {
    if (this.avPlayer) {
      try {
        this.avPlayer.setSpeed(media.PlaybackSpeed.SPEED_FORWARD_2_00_X);
        Logger.info(this.tag, 'videoSpeed_2_0');
      } catch (e) {
        Logger.info(this.tag, 'videoSpeed_2_0== ${JSON.stringify(e)}');
      }
    }
  }

  /**
   * 视频跳转
   */
  videoSeek(seekTime: number): void {
    if (this.avPlayer) {
      try {
        this.avPlayer.seek(seekTime, media.SeekMode.SEEK_PREV_SYNC);
        Logger.info(this.tag, `videoSeek== ${seekTime}`);
      } catch (e) {
        Logger.info(this.tag, 'videoSeek== ${JSON.stringify(e)}');
      }
    }
  }

  /**
   * 视频重置
   */
  async videoReset(): Promise<void> {
    this.avPlayer!.reset();
  }

  /**
   * 释放视频资源
   */
  async videoRelease(): Promise<void> {
    if (this.avPlayer === null) {
      return;
    }
    this.avPlayer.release((err) => {
      if (err == null) {
        Logger.info(this.tag, 'videoRelease release success');
      } else {
        Logger.error(this.tag, `videoRelease release filed,error message is = ${err.message}`);
      }
    });
  }

}