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
import { ResponseResult, sendRequest } from '../../utils/request'
import { runRequest } from '../../utils/Task'
import { InitDeviceDto } from '../interface'

@Component
export struct LedDialog {
  @State title: string = 'led灯'
  @Link ledIsOn: boolean
  @State intervalSeqs: number[] = []
  @State deviceInfo1: InitDeviceDto = {
    deviceName: '无',
    device: [],
    deviceNum: 0,
    ip: '',
    signal: ''
  }
  initCallback = (err: Error, res: http.HttpResponse) => {
    if (err) {
      return
    }
    const data: ResponseResult = JSON.parse(res.result.toString()) as ResponseResult
    if (data.code == 200) {
      hilog.debug(0x0000, 'Test Tag', JSON.stringify(data.result))
      this.deviceInfo1 = data.result as InitDeviceDto
      this.intervalSeqs.forEach((intervalSeq: number) => clearInterval(intervalSeq))
      hilog.debug(0x0000, 'Test Tag', '清除轮询器: ' + this.intervalSeqs.toString())
    }
    hilog.debug(0x0000, 'Test Tag', JSON.stringify(data.msg))
  }

  aboutToAppear() {
    const timer = runRequest({
      url: '/device/init?key=testtopic/led',
      options: {
        method: http.RequestMethod.GET
      },
      callback: this.initCallback
    })
    this.intervalSeqs.push(timer)
  }

  build() {
    Row() {
      Row() {
        Image(this.ledIsOn ? $r('app.media.led_icon') : $r('app.media.led_noicon'))
          .objectFit(ImageFit.Contain)
          .margin({ top: 40 })
      }
      .width('60%')
      .height('100%')

      Column() {
        Column() {
          Text(this.ledIsOn ? 'on' : 'off')
            .fontSize(65)
            .fontWeight(FontWeight.Bold)
            .fontColor('#ccdfed')
          Text(this.ledIsOn ? '照明' : '关闭')
            .fontSize(25)
            .fontColor('#ccdfed')
        }

        Button({ type: ButtonType.Circle }) {
          Image($r('app.media.switch_icon'))
            .width('100%')
            .height('100%')
            .objectFit(ImageFit.Contain)
            .fillColor('#fde101')
            .margin({ top: -30 })
        }
        .backgroundColor('#007dfe')
        .width('60%')
        .height('60%')
        .shadow({
          radius: 10,
          color: '#99898989',
          offsetX: 2,
          offsetY: 2
        })
        .onClick((e: ClickEvent) => {
          this.ledIsOn = !this.ledIsOn
          if (this.ledIsOn) {
            hilog.debug(0x0000, 'Test Tag', '打开led灯')
            sendRequest('/device/led/on',
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
            hilog.debug(0x0000, 'Test Tag', '关闭led灯')
            sendRequest('/device/led/off',
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
        })
      }
      .width('40%')
      .height('100%')
      .justifyContent(FlexAlign.SpaceEvenly)
      .alignItems(HorizontalAlign.Start)
    }
    .width('100%')
    .height('100%')
    // .backgroundColor('#3090e3')
    .backgroundImage($r('app.media.led_card_bg'))
    .backgroundImageSize({
      width: '100%',
      height: '100%'
    })
    .shadow({
      radius: 25,
      color: '#333333',
      offsetX: -10,
      offsetY: -10
    })
  }
}