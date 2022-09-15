/*
 * Copyright 2022 Unionman Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import router from '@ohos.router';
import { cardItem, TextRow } from '../utils/componentUtil'
import { getFullYear, getDate, getTime, getSeconds } from '../utils/functionUtil'
import adc_napi from '@ohos.adc_napi';
import i2cnapidemo from '@ohos.i2cnapidemo';

@Entry
@Component
struct Index {
  @State currentDate: string = getFullYear() + getDate()
  @State currentTime: string = getTime() + getSeconds()
  @State temp: number = 0
  quality: string = '优'
  @State humidity: number = 0
  @State Lux: number = 1010
  longitude: number
  latitude: number

  aboutToAppear() {
    setInterval(() => {
      i2cnapidemo.ReadData()
      this.temp = i2cnapidemo.ReadTemperatureC()
      this.humidity = i2cnapidemo.ReadHumidity()
    }, 100)
    this.Lux = adc_napi.sync_get_adc_value()
  }

  onPageShow() {
    setInterval(() => {
      let date = new Date
      this.Lux = adc_napi.sync_get_adc_value()
      this.currentTime = (date.getHours() < 10 ? '0' + date.getHours() : date.getHours()) + ':' + (date.getMinutes() < 10 ? '0' + date.getMinutes() : date.getMinutes()) + ':' + (date.getSeconds() < 10 ? '0' + date.getSeconds() : date.getSeconds())
      if (date.getHours() == 0) {
        this.currentDate = date.getFullYear() + '年' + (date.getMonth() + 1) + '月' + date.getDate() + '日'
      }
    }, 1000)
  }

  build() {
    Row() {
      Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.SpaceEvenly, alignItems: ItemAlign.Center }) {
        Flex({ justifyContent: FlexAlign.SpaceAround }) {
          if (this.Lux > 100) {
            Image($r('app.media.sun'))
              .width(100)
              .height(100)
              .objectFit(ImageFit.None)
          } else {
            Image($r('app.media.moon'))
              .width(100)
              .height(100)
              .objectFit(ImageFit.None)
          }
          Text(this.currentDate + '\n' + this.currentTime)
            .fontSize(36)
            .fontColor(Color.White)
        }
        .width('100%')

        TextRow({ text: '温度', value: this.temp.toFixed(0) + '℃' })
        TextRow({ text: '空气质量', value: this.quality })
        TextRow({ text: '空气湿度', value: this.humidity.toFixed(0) + "%RH" })
      }
      .margin({ left: 30 })
      .height('88%')
      .width('45%')
      .borderRadius(30)
      .backgroundColor($r('app.color.background_blue'))
      .onClick(() => {
        router.push({
          url: 'pages/I2cDeviceControl',
        })
      })

      Column() {
        Row() {
          cardItem({ text: $r('app.string.smartLight'), url: 'pages/GpioDeviceControl' })
          cardItem({ text: $r('app.string.smartCurtain'), url: 'pages/PwmDeviceControl' })
        }

        Row() {
          cardItem({ text: '音乐播放' })
          cardItem({ text: '家庭备忘录', url: 'pages/SpiDevice' })
        }
        .margin({ top: 25 })
      }
    }
    .width('100%')
    .height('100%')
    .backgroundImage(this.Lux > 100 ? $r('app.media.bgDot_day') : $r('app.media.bgDot_night'))
    .backgroundImageSize(ImageSize.Cover)
  }
}