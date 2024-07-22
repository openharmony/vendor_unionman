/* Copyright 2023 Unionman Technology Co., Ltd.
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

import hilog from '@ohos.hilog'
import http from '@ohos.net.http'
import { sendRequest } from '../../utils/request'
import { InitDeviceDto } from '../interface'

@Component
struct FengMingQiDialogMain {
  @State btnSwitch: boolean = false

  build() {
    Row() {
      Row() {
        Image($r('app.media.fengmingqi'))
          .objectFit(ImageFit.Contain)
          .margin({ left: 50 })
      }
      .width('60%')
      .height('100%')
      .padding(25)
      .justifyContent(FlexAlign.Center)


      Column({ space: 20 }) {
        Button('ON', { type: ButtonType.Capsule })
          .backgroundColor(this.btnSwitch ? '#007dfe' : '#a9a9a9')
          .width('45%')
          .height(45)
          .onClick((e: ClickEvent) => {
            this.btnSwitch = true
            hilog.debug(0x0000, 'Test Tag', '发出蜂鸣器')
            sendRequest('/device/buzzer/on',
              {
                method: http.RequestMethod.GET
              })
              .then((res) => {
                hilog.debug(0x0000, 'Test Tag', res.result.toString())
              })
              .catch((err: Error) => {
                hilog.debug(0x0000, 'Test Tag', err.message)
              })
          })

        Button('OFF', { type: ButtonType.Capsule })
          .backgroundColor(this.btnSwitch ? '#a9a9a9' : '#007dfe')
          .width('45%')
          .height(45)
          .onClick((e: ClickEvent) => {
            this.btnSwitch = false
            hilog.debug(0x0000, 'Test Tag', '关闭蜂鸣器')
            sendRequest('/device/buzzer/off',
              {
                method: http.RequestMethod.GET
              })
              .then((res) => {
                hilog.debug(0x0000, 'Test Tag', res.result.toString())
              })
              .catch((err: Error) => {
                hilog.debug(0x0000, 'Test Tag', err.message)
              })
          })
      }
      .width('40%')
      .height('100%')
      .justifyContent(FlexAlign.Center)
    }
    .width('100%')
    .height('100%')
  }
}


@Component
struct LedDialogMain {
  openThreeLed(isOn: boolean) {
    if (isOn) {
      hilog.debug(0x0000, 'Test Tag', '发出蜂鸣器')
      sendRequest('/device/light/on',
        {
          method: http.RequestMethod.GET
        })
        .then((res) => {
          hilog.debug(0x0000, 'Test Tag', res.result.toString())
        })
        .catch((err: Error) => {
          hilog.debug(0x0000, 'Test Tag', err.message)
        })

      this.isOn = isOn
    }
  }

  redLed(isOn: boolean) {
    if (isOn) {
      hilog.debug(0x0000, 'Test Tag', '打开红灯')
      sendRequest('/device/light/red/on',
        {
          method: http.RequestMethod.GET
        })
        .then((res) => {
          hilog.debug(0x0000, 'Test Tag', res.result.toString())
        })
        .catch((err: Error) => {
          hilog.debug(0x0000, 'Test Tag', err.message)
        })
    } else {
      hilog.debug(0x0000, 'Test Tag', '关闭红灯')
      sendRequest('/device/light/red/off',
        {
          method: http.RequestMethod.GET
        })
        .then((res) => {
          hilog.debug(0x0000, 'Test Tag', res.result.toString())
        })
        .catch((err: Error) => {
          hilog.debug(0x0000, 'Test Tag', err.message)
        })
    }
  }

  yellowLed(isOn: boolean) {
    if (isOn) {
      hilog.debug(0x0000, 'Test Tag', '打开黄灯')
      sendRequest('/device/light/yellow/on',
        {
          method: http.RequestMethod.GET
        })
        .then((res) => {
          hilog.debug(0x0000, 'Test Tag', res.result.toString())
        })
        .catch((err: Error) => {
          hilog.debug(0x0000, 'Test Tag', err.message)
        })
    } else {
      hilog.debug(0x0000, 'Test Tag', '关闭黄灯')
      sendRequest('/device/light/yellow/off',
        {
          method: http.RequestMethod.GET
        })
        .then((res) => {
          hilog.debug(0x0000, 'Test Tag', res.result.toString())
        })
        .catch((err: Error) => {
          hilog.debug(0x0000, 'Test Tag', err.message)
        })
    }
  }

  greenLed(isOn: boolean) {
    if (isOn) {
      hilog.debug(0x0000, 'Test Tag', '打开绿灯')
      sendRequest('/device/light/green/on',
        {
          method: http.RequestMethod.GET
        })
        .then((res) => {
          hilog.debug(0x0000, 'Test Tag', res.result.toString())
        })
        .catch((err: Error) => {
          hilog.debug(0x0000, 'Test Tag', err.message)
        })
    } else {
      hilog.debug(0x0000, 'Test Tag', '关闭绿灯')
      sendRequest('/device/light/green/off',
        {
          method: http.RequestMethod.GET
        })
        .then((res) => {
          hilog.debug(0x0000, 'Test Tag', res.result.toString())
        })
        .catch((err: Error) => {
          hilog.debug(0x0000, 'Test Tag', err.message)
        })
    }
  }

  @State isOn: boolean = false

  aboutToDisappear() {
    hilog.debug(0x0000, 'Test Tag', '关闭红灯')
    sendRequest('/device/light/red/off',
      {
        method: http.RequestMethod.GET
      })
      .then((res) => {
        hilog.debug(0x0000, 'Test Tag', res.result.toString())
      })
      .catch((err: Error) => {
        hilog.debug(0x0000, 'Test Tag', err.message)
      })
    hilog.debug(0x0000, 'Test Tag', '关闭红灯')
    sendRequest('/device/light/yellow/off',
      {
        method: http.RequestMethod.GET
      })
      .then((res) => {
        hilog.debug(0x0000, 'Test Tag', res.result.toString())
      })
      .catch((err: Error) => {
        hilog.debug(0x0000, 'Test Tag', err.message)
      })
    hilog.debug(0x0000, 'Test Tag', '关闭绿灯')
    sendRequest('/device/light/green/off',
      {
        method: http.RequestMethod.GET
      })
      .then((res) => {
        hilog.debug(0x0000, 'Test Tag', res.result.toString())
      })
      .catch((err: Error) => {
        hilog.debug(0x0000, 'Test Tag', err.message)
      })


  }

  build() {
    Row() {
      Row() {
        Image($r('app.media.threeled'))
          .objectFit(ImageFit.Contain)
          .margin({ left: 60 })
      }
      .width('60%')
      .height('100%')
      .padding({ top: 20 })

      Column() {
        Row() {
          Text('开机')
            .fontSize(18)
          Toggle({ type: ToggleType.Switch, isOn: false })
            .selectedColor($r('app.color.app_header_color'))
            .switchPointColor('#FFFFFF')
            .onChange(this.openThreeLed)
            .width(60)
            .height(30)
        }

        Row() {
          Text('危险')
            .fontSize(18)
          Toggle({ type: ToggleType.Switch, isOn: false })
            .selectedColor(Color.Red)
            .switchPointColor('#FFFFFF')
            .onChange(this.redLed)
            .width(60)
            .height(30)
        }

        Row() {
          Text('安全')
            .fontSize(18)
          Toggle({ type: ToggleType.Switch, isOn: true })
            .selectedColor(Color.Green)
            .switchPointColor('#FFFFFF')
            .onChange(this.greenLed)
            .width(60)
            .height(30)
        }

        Row() {
          Text('警告')
            .fontSize(18)
          Toggle({ type: ToggleType.Switch, isOn: false })
            .selectedColor('#ffe202')
            .switchPointColor('#FFFFFF')
            .onChange(this.yellowLed)
            .width(60)
            .height(30)
        }
      }
      .width('40%')
      .height('100%')
      .justifyContent(FlexAlign.Center)
      .alignItems(HorizontalAlign.Start)
      .padding({
        top: 10,
        bottom: 10
      })
    }
    .width('100%')
    .height('100%')
  }
}


@Component
export struct ConnectDeviceDialog {
  @State name: string = '润和3861'
  @State signal: string = '强'
  @State ip: string = '192.168.248.12'
  @State connectTime: string = '2024-06-05 21:34'
  @State exteriorNum: number = 2
  @State textFontSize: number = 20
  private controller1: TabsController = new TabsController()
  @State activeIndex: number = 0
  @Link deviceInfo: InitDeviceDto

  @Builder myText(title: string, index: number) {
    Row() {
      Text(title)
        .fontSize(20)
        .fontWeight(this.activeIndex == index ? 700 : 600)
        .fontColor(this.activeIndex == index ? $r('app.color.app_header_color') : $r('app.color.icon_fuzhu_color'))
    }
  }

  build() {
    Column() {
      Row() {

        Row() {
          Image($r('app.media.device_icon'))
            .objectFit(ImageFit.Fill)
            .fillColor($r('app.color.app_header_color'))
        }
        .height('100%')
        .width('25%')
        .justifyContent(FlexAlign.Center)
        .border({
          width: 2,
          style: BorderStyle.Solid,
          color: '#1e49ed'
        })

        Column({ space: 5 }) {
          Text('板子名称: ' + this.deviceInfo.deviceName)
            .fontSize(this.textFontSize)
          Text('连接信号: ' + this.deviceInfo.signal)
            .fontSize(this.textFontSize)
          Text('设备Ip: ' + this.deviceInfo.ip)
            .fontSize(this.textFontSize)
        }
        .justifyContent(FlexAlign.Center)
        .alignItems(HorizontalAlign.Start)
        .width('75%')
        .height('100%')
        .margin({ left: 20 })

      }
      .borderWidth({
        bottom: 3
      })
      .borderColor('#d5d5d5')
      .borderStyle(BorderStyle.Solid)
      .width('100%')
      .height('35%')
      .padding(20)

      Row() {
        Tabs({ controller: this.controller1 }) {
          TabContent() {
            FengMingQiDialogMain()
              .width('100%')
              .height('100%')
          }
          .tabBar(this.myText('蜂鸣器', 0))

          TabContent() {
            LedDialogMain()
              .width('100%')
              .height('100%')
          }
          .tabBar(this.myText('状态灯', 1))
        }
        .vertical(true)
        .scrollable(true)
        .barMode(BarMode.Fixed)
        .barWidth(90)
        .barHeight(200)
        .animationDuration(400)
        .onChange((index: number) => {
          this.activeIndex = index
        })
        .height('65%')
        .margin({ bottom: '12vp' })
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#ffffff')
  }
}