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

import { DeviceTitle, DeviceLogo, DeviceState, Stow, componentsRow } from '../utils/devicepagecomponent';
import i2cnapidemo from '@ohos.i2cnapidemo';

/**
 * 温湿度计控制界面
 */
@Entry
@Component
struct I2cDeviceControl {
  @State ReadTemperatureC: number = 0
  @State ReadHumidity: number = 0
  intervalID: number
  scroller: Scroller = new Scroller()
  @State isON: boolean = false
  deviceLogo: Resource = $r("app.media.icon_temp_and_hum_meter")
  deviceSwitchText: Resource = $r("app.string.closed")
  deviceSwitchTextColor = $r("app.color.background_black")
  @State switchImg: Resource = $r("app.media.icon_switch_off")

  onPageShow(){
    this.setCommandMessage()
  }

  startWork() {
    i2cnapidemo.SoftReset()
    i2cnapidemo.ModeSet(1, 2)
    this.intervalID = setInterval(() => {
      i2cnapidemo.ReadData()
      this.ReadTemperatureC = i2cnapidemo.ReadTemperatureC()
      this.ReadHumidity = i2cnapidemo.ReadHumidity()
    }, 100)
  }

  setCommandMessage() {
    if (this.isON) {
      this.switchImg = $r("app.media.icon_switch_on")
      this.deviceSwitchText = $r("app.string.opened")
      this.startWork()
    } else {
      this.switchImg = $r("app.media.icon_switch_off")
      this.deviceSwitchText = $r("app.string.closed")
      i2cnapidemo.SoftReset()
      clearInterval(this.intervalID)
    }
  }

  build() {
    Flex({ direction: FlexDirection.Column }) {
      DeviceTitle({ deviceTitle: $r("app.string.smart_temp_and_hum_meter") })
      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center }) {
        DeviceLogo({ deviceLogo: this.deviceLogo })
        Scroll(this.scroller) {
          Column({ space: 12 }) {
            DeviceState({
              deviceSwitchText: this.deviceSwitchText,
              deviceSwitchTextColor: this.deviceSwitchTextColor,
              isAlert: true,
              switchImg: this.switchImg,
              callback: () => {
                this.isON = !this.isON
                this.setCommandMessage()
              }
            })

            Flex({ direction: FlexDirection.Row, justifyContent: FlexAlign.SpaceBetween }) {
              componentsRow({
                text: "温度：",
                textColor: $r("app.color.background_blue"),
                textSize: 16,
                textSub: this.ReadTemperatureC.toFixed(1) + "°C",
                textSubSize: 16,
                textSubColor: $r("app.color.background_blue"),
                frameWidth: '48%',
              })
              componentsRow({
                text: "湿度：",
                textColor: $r("app.color.background_blue"),
                textSize: 16,
                textSub: this.ReadHumidity.toFixed(1) + "%RH",
                textSubSize: 16,
                textSubColor: $r("app.color.background_blue"),
                frameWidth: '48%',
              })
            }
            .height(64)
            .margin({ top: 12 })
            .width('100%')

            Stow()
            Flex({ direction: FlexDirection.Row, justifyContent: FlexAlign.SpaceBetween }) {
              componentsRow({
                text: $r("app.string.moreProducts"),
                textSize: 14,
                image: $r("app.media.icon_more_device"),
                frameWidth: '48%',
              })
              componentsRow({
                text: $r("app.string.partsReplacement"),
                textSize: 14,
                image: $r("app.media.icon_replace"),
                frameWidth: '48%',
              })
            }
            .height(64)
            .margin({ top: 12 })
            .width('100%')
          }
        }
        .flexGrow(1)
      }
    }
    .backgroundColor($r("app.color.background_grey"))
    .padding({ left: 16, right: 16 })
  }
}