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

import audio from '@ohos.multimedia.audio';
import PlayerModel from '../common/PlayerModel';
@Component
export struct  PlayComponent {
  //视频地址
  videoSrc: string = "data/2.mp4";
  // 当前视频播放速率
  @State currentProgressRate: number = 1;
  //音频管理器
  audioManager: any;
  //音频渲染器
  audioRenderer: any;
  //音频title
  @State audioTitle: string = 'title';
  //是否静音
  @State isMute: boolean = false;
  //音量:取值范围为[0.00-1.00]
  @State volume: number = 0;
  //状态
  @State active: number = -1;
  //播放进度
  @State audioTime: number = 0;
  //播放时长
  @State audioMaxTime: number = 0;
  //是否播放
  @State audioIsPlaying: boolean = false;
  //定时器id
  intervalID ;
  //播放暂停图
  @State audioUrl: string = '';
  //音量图
  @State volumeUrl: string = 'common/images/volume.png';
  //最大最小音量
  @State audioMaxVolume: number = 0;
  @State audioMinVolume: number = 0;
  playerModel = new PlayerModel();
  // Web控制器
  private webController: WebController = null
  index = 1;
  msgPort: any[] = null;
  //接口函数
  onPlayClick() {
    if (this.playerModel.isPlaying) {
      this.playerModel.pause();
    } else if(this.audioTime > 0){
      this.playerModel.play(this.audioTime, true);
    } else{
      this.playerModel.preLoad(this.index, () => {
        this.playerModel.play(-1, true);
      });
    }
  }

  createUpdateAudioTime() {
    if (typeof (this.intervalID) === 'undefined') {
      this.intervalID = setInterval(() => {
        this.audioTime = this.playerModel.getCurrentMs();
        console.log('MusicPlayer[IndexPage] createUpdateAudioTime：' + this.audioTime);
      }, 500);
    }
    else
    console.log('MusicPlayer[IndexPage] createUpdateAudioTime IntervalID：' + this.intervalID);
  }

  cancelUpdateAudioTime() {
    if (typeof (this.intervalID) === 'undefined')
    console.log('MusicPlayer[IndexPage] deleteUpdateAudioTime：no Interval');
    else {
      clearInterval(this.intervalID);
      this.intervalID = undefined;
    }
  }

  msToTime(ms) {
    let s = Math.floor(ms / 1000);
    let mm = Math.floor(s / 60);
    s = Math.floor(s % 60);
    return mm + ':' + s;
  }

  async aboutToAppear() {
    this.audioUrl = 'common/images/pause.png';
    this.playerModel.setOnStatusChangedListener((isPlaying) => {
      this.audioIsPlaying = isPlaying;
      console.info('MusicPlayer[IndexPage] on player status changed, isPlaying=' + isPlaying + ', refresh ui');

      if (isPlaying) {
        this.audioUrl = 'common/images/play.png';
        this.createUpdateAudioTime();
        this.audioMaxTime = this.playerModel.getDurationMs();
        this.playerModel.getMaxVolume().then((vol)=>{
          this.audioMaxVolume = vol;
        })
        this.playerModel.getMinVolume().then((vol)=>{
          this.audioMinVolume = vol;
        })
        this.playerModel.getVolume().then((vol)=>{
          this.volume = vol;
        })

      } else {
        this.audioUrl = 'common/images/pause.png';
        this.cancelUpdateAudioTime();
      }
    });
    this.playerModel.getPlaylist(() => {
      console.info('MusicPlayer[IndexPage] on playlist generated, refresh ui');
    });
    this.playerModel.notifyPlayingStatus(this.audioIsPlaying);
    this.audioTitle = this.playerModel.playlist.audioFiles[this.index].name;
    this.audioManager = audio.getAudioManager();


  }

  build() {
      Column() {
        Text(this.audioTitle)
          .height(60)
        Row() {
          Image(this.audioUrl)
            .height(60)
            .width(60)
            .onClick(() => {
              this.onPlayClick();
            })
          Slider({
            value: this.audioTime,
            min: 0,
            max: this.audioMaxTime,
            step: 10,
            style: SliderStyle.OutSet,
            direction: Axis.Horizontal,
            reverse: false
          })
            .onChange(async(ms: number,mode: SliderChangeMode) => {
              this.playerModel.pause();
              this.playerModel.play(ms,!this.audioIsPlaying);
              console.log('MusicPlayer[IndexPage] seek :' + ms +' mode:'+mode.valueOf()+' this.audioIsPlaying:' + this.audioIsPlaying.valueOf());
            })
            .width(260)
            .height(60)
          Text(this.msToTime(this.audioTime) + '/' + this.msToTime(this.audioMaxTime))
            .fontSize(20)
            .height(60)
        }

        Row() {
          Image(this.volumeUrl)
            .height(60)
            .width(60)
          Slider({
            value: this.volume,
            min: this.audioMinVolume,
            max: this.audioMaxVolume,
            step: 1,
            style: SliderStyle.OutSet,
            direction: Axis.Horizontal,
            reverse: false
          })
            .width(260)
            .height(60)
            .onChange(async(value: number) => {
              this.playerModel.setVolume(value);
              this.volume = await this.playerModel.getVolume();
              console.log('MusicPlayer[IndexPage] setVolume:' + value);
            })
          Text(this.volume+'/'+this.audioMaxVolume)
            .height(60)
            .width(100)
        }
      }
      .width('100%')
      .height('100%')

  }
}