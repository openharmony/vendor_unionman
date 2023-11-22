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
import prompt from '@ohos.prompt';
import deviceManager from '@ohos.distributedHardware.deviceManager';
import featureAbility from '@ohos.ability.featureAbility';
import window from '@ohos.window';
import { KeyCode } from '@ohos.multimodalInput.keyCode'
import inputMonitor from '@ohos.multimodalInput.inputMonitor';



@Component
export struct localVideo {
  src: Resource = $r('app.media.EP11');

  @State videoIndex: number = 0
  @State SpeedIndex: number = 1
  @State title: string = "本地视频";
  // 当前视频播放速率
  @State currentProgressRate: number = 1;
  @State autoPlays: boolean = true;
  // 是否显示控制栏
  @State controls: number = 0;
  @State isFullScreen: boolean = false;
  // 是否正在播放
  @State isPlay: boolean = true;
  // 滑动条进度
  @State inVerticalSetValue: number = 0
  // 当前播放进度
  @State currentTime: number = 0
  // 视频时长
  @State duration: number = 0
  @State currentSpeed: string = '1.0X'
  @State intervalID: number = 0

  //按下左右键的时长
  @State keyDownTimes: number = 0
  startx: number
  starty: number
  endx: number
  endy: number
  //定时器的id
  timeoutID :number = 0
  // 设备管理对象
  controller: VideoController = new VideoController();
  scroller: Scroller = new Scroller();
  @State isKeyboardDelegate: boolean = false;

  onPageShow() {
    var context = featureAbility.getContext()
    context.verifyPermission("ohos.permission.DISTRIBUTED_DATASYNC").then((data) => {
      console.info("======================>verifyPermissionCallback====================>");
      console.info("====>data====>" + JSON.stringify(data));
      if (data != 0) {
        context.requestPermissionsFromUser(
          ["ohos.permission.DISTRIBUTED_DATASYNC"],
          1).then((data) => {
          console.info("====>requestdata====>" + JSON.stringify(data));
        });
      }
    })
  }

  onInit() {
    this.SpeedIndex = 1
    this.isPlay = true
    this.currentTime = 0
    this.currentSpeed = '1.0X'
    this.currentProgressRate = 1
    this.inVerticalSetValue = 0

  }
  changeScreenmode(){
    this.isFullScreen = !this.isFullScreen
    //获取当前窗口
    var FullScreen = this.isFullScreen;
    var windowClass = null;
    let promise = window.getTopWindow();
    promise.then((data) => {
      windowClass = data;
      console.info('Succeeded in obtaining the top window. Data: ' + JSON.stringify(data));
      //设置全屏状态
      let promise = windowClass.setFullScreen(FullScreen);
      promise.then(() => {
        console.info('Succeeded in enabling the full-screen mode. ');
      }).catch((err) => {
        console.error('Failed to enable the full-screen mode. Cause: ' + JSON.stringify(err));
      });
    }).catch((err) => {
      console.error('Failed to obtain the top window. Cause: ' + JSON.stringify(err));
    })
  }

  setScreenKeepOn() {
    //获取当前窗口
    var windowClass = null;
    let promise = window.getTopWindow();
    promise.then((data) => {
      console.info('XXXXX Succeeded in obtaining the top window. Data: ' + JSON.stringify(data));
      windowClass = data;

      //设置常亮状态
      var isKeepScreenOn = true;
      let promise = windowClass.setKeepScreenOn(isKeepScreenOn);
      console.log("XXXXX setKeepScreenOn")
      promise.then(() => {
        console.info('XXXXX Succeeded in setting the screen to be always on.');
      }).catch((err) => {
        console.info('XXXXX Failed to set the screen to be always on. Cause:  ' + JSON.stringify(err));

      });

    }).catch((err) => {
      console.error('XXXXX Failed to obtain the top window. Cause: ' + JSON.stringify(err));
    })
  }

  timeToString(time){
    let second = Math.floor(time%60)
    let minute = Math.floor(time/60)

    return minute+':'+second
  }
  build() {
    Stack({ alignContent: Alignment.Bottom }) {

      Video({
        src: this.src,
        currentProgressRate: this.currentProgressRate,
        controller: this.controller
      })
        .width('100%')
        .height('100%')
        .autoPlay(this.autoPlays)
        .controls(false)
        .loop(true)
        .focusable(false)
        .onStart(() => {
          console.log("XXX onStart")
          this.isPlay = true
          this.setScreenKeepOn()
          //this.setTimer()
        })
        .onPause(() => {
          console.log("XXX onPause")
          this.isPlay = false
        })
        .onFinish(() => {
          console.log("XXX onFinish")
          //this.onInit()
          //this.controller.setCurrentTime(0, SeekMode.Accurate)
        })
        .onError(() => {
          this.isPlay = false
        })
        .onPrepared((e) => {
          console.error('XXX onPrepared is ' + e.duration);
          this.duration = e.duration
        })
        .onSeeked((e) => {
          console.error('XXX onSeekedis ' + e.time);
          this.currentTime = e.time
        })
        .onUpdate((e) => {
          this.currentTime = e.time
        })

        .gesture(
        TapGesture({ count: 1 })
          .onAction(() => {
            if (this.controls) {
              this.controls = 0;
            } else {
              this.controls = 1;
            }
          })
        )
        .gesture(
        TapGesture({ count: 2 })
          .onAction(() => {
            if (this.isPlay) {
              this.isPlay = !this.isPlay
              this.controller.pause();
            } else {
              this.isPlay = !this.isPlay
              this.controller.start();
            }
          })
        )
        .onTouch((event: TouchEvent) => {
          if (event.type === TouchType.Down) {
            this.startx = event.touches[0].screenX
            this.starty = event.touches[0].screenY
          }
          if (event.type === TouchType.Up) {
            this.endx = event.touches[0].screenX
            this.endy = event.touches[0].screenY
            //右滑
            if (this.endx > this.startx && Math.abs(this.endy - this.starty) < 100) {
              this.videoIndex--
              this.onInit()
            }
            //左滑
            else if (this.endx < this.startx && Math.abs(this.endy - this.starty) < 100) {
              this.videoIndex++
              this.onInit()
            }
          }
        })
      // 控制栏
      Column() {
        Row() {
          Image(this.isPlay ? $r('app.media.ic_public_pause') : $r('app.media.ic_public_play'))
            //Image($r('app.media.ic_public_play'))
            .id("Play")
            .width(32)
            .height(32)
            .focusable(false)
            .onClick(() => {
              this.isPlay = !this.isPlay
              if (this.isPlay) {
                this.controller.start()
              } else {
                this.controller.pause()
              }
            })

          Slider({
            value: this.currentTime,
            min: 0,
            max: this.duration,
            step: 1,
          })
            .id("Slider")
            .width('54%')
            .blockColor(Color.White)
            .trackColor(Color.Gray)
            .selectedColor(Color.White)
            .onChange((value: number, mode: SliderChangeMode) => {

              this.inVerticalSetValue = value
              this.currentTime = value
              this.controller.setCurrentTime(value, SeekMode.Accurate)
                 console.error('XXXX value:' + value + 'mode:' + mode.toString())




            })
            .focusable(true)
            .onFocus(() => {
              console.error("XXXX onFocus4")
            })
            .onKeyEvent((event: KeyEvent) => {
              event.stopPropagation()
              console.error("onKeyEvent"+event.keyText)
              if (event.type == KeyType.Up && event.keyCode === KeyCode.KEYCODE_ENTER) {
                console.error("XXXX 按下home键 4 time" + event.timestamp)
                if(this.isPlay == true)
                  this.controller.pause()
                else
                  this.controller.start()
              }
            })
          Text(this.timeToString(this.currentTime) + "/" + this.timeToString(this.duration))
          .fontColor(Color.White)
          .fontSize(20)
        }
        .align(Alignment.Bottom)
        //        .border({ width: 1, color: 'red', style: BorderStyle.Solid })
      }
      .padding(10)
      //      .border({ width: 1, color: 'red', style: BorderStyle.Solid })
      .justifyContent(FlexAlign.Center)
      .width('100%')
      .visibility(this.controls)

    }

  }
}
