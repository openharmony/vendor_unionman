/*
 * Copyright 2023 Unionman Technology Co., Ltd.
 *
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
 *
 */
import media from '@ohos.multimedia.media';
import Logger from '../model/Logger';

const TAG: string = 'AVPlayerUtils';

export default class AVPlayerUtils {
  private avPlayer: media.AVPlayer = undefined
  private playPath: string = ''
  private timeUpdateCallBack: (time: number) => void = undefined

  async initVideoPlayer(playSrc: string, surfaceID: string) {
    await this.release()
    this.playPath = playSrc
    // 创建avPlayer实例对象
    this.avPlayer = await media.createAVPlayer()
    Logger.info(TAG, 'createVideoPlayer')
    this.avPlayer.url = this.playPath
    Logger.info(TAG, 'this.avPlayer.url' + this.avPlayer.url)
    this.avPlayer.on('timeUpdate', (time: number) => {
      Logger.info(TAG, 'timeUpdate success,and new time is :' + time)
      if (this.timeUpdateCallBack) {
        this.timeUpdateCallBack(time)
      }
    });
    this.avPlayer.surfaceId = surfaceID
    Logger.info(TAG, 'setDisplaySurface')
    await this.avPlayer.prepare();
    Logger.info(TAG, 'init VideoPlayer finish')
  }

  async play() {
    Logger.info(TAG, 'play')
    if (typeof (this.avPlayer) != 'undefined') {
      await this.avPlayer.play()
    }
  }

  async seek(time: number) {
    Logger.info(TAG, 'seek')
    if (typeof (this.avPlayer) != 'undefined') {
      this.avPlayer.seek(time * 1000)
    }
  }

  async setSpeed(speed: media.PlaybackSpeed) {
    Logger.info(TAG, 'setSpeed')
    if (typeof (this.avPlayer) != 'undefined') {
      this.avPlayer.setSpeed(speed)
    }
  }

  async pause() {
    Logger.info(TAG, 'pause')
    if (typeof (this.avPlayer) != 'undefined') {
      await this.avPlayer.pause()
    }
  }

  async stop() {
    Logger.info(TAG, 'stop')
    if (typeof (this.avPlayer) != 'undefined') {
      await this.avPlayer.stop()
    }
  }

  async reset(playSrc: string) {
    Logger.info(TAG, 'reset')
    if (typeof (this.avPlayer) != 'undefined') {
      this.playPath = playSrc
      await this.avPlayer.reset()
      this.avPlayer.url = this.playPath
      await this.avPlayer.prepare()
    }
  }

  async release() {
    Logger.info(TAG, 'release')
    if (typeof (this.avPlayer) != 'undefined') {
      await this.avPlayer.release()
      Logger.info(TAG, 'release success')
    }
  }

  setTimeUpdateCallBackCallBack(callback) {
    this.timeUpdateCallBack = callback
  }
}