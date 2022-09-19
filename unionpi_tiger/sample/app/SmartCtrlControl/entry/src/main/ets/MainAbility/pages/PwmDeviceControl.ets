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

import { DeviceTitle, DeviceLogo, DeviceState, ProgressBar, Stow, componentsRow } from '../utils/devicepagecomponent';
import pwmtest from '@ohos.pwmtest';

/**
 * 窗帘控制界面
 */
@Entry
@Component
struct PwmDeviceControl {
  scroller: Scroller = new Scroller()
  @State isON: boolean = false
  deviceLogo: Resource = $r("app.media.icon_curtain")
  deviceSwitchText: Resource = $r("app.string.closed")
  deviceSwitchTextColor = $r("app.color.background_black")
  @State switchImg: Resource = $r("app.media.icon_switch_off")
  @State @Watch('sendDeviceCommand') progressValue: number = 50
  openStateImage: Resource = $r("app.media.icon_curtain_open_unselected")
  closeStateImage: Resource = $r("app.media.icon_curtain_close_selected")

  setCommandMessage() {
    if (this.isON) {
      this.openStateImage = $r("app.media.icon_curtain_open_selected")
      this.closeStateImage = $r("app.media.icon_curtain_close_unselected")
      this.switchImg = $r("app.media.icon_switch_on")
      this.deviceSwitchText = $r("app.string.opened");
    } else {
      this.openStateImage = $r("app.media.icon_curtain_open_unselected")
      this.closeStateImage = $r("app.media.icon_curtain_close_selected")
      this.switchImg = $r("app.media.icon_switch_off")
      this.deviceSwitchText = $r("app.string.closed");
    }
  }

  sendDeviceCommand() {
    if (this.isON) {
      pwmtest.setPwmStatus(1, this.progressValue)
    }
  }

  build() {
    Flex({ direction: FlexDirection.Column }) {
      DeviceTitle({ deviceTitle: $r("app.string.smartCurtain") })
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
                text: "打开",
                textSize: 16,
                image: this.openStateImage,
                frameWidth: '48%',
              })
              componentsRow({
                text: "关闭",
                textSize: 16,
                image: this.closeStateImage,
                frameWidth: '48%',
              })
            }
            .height(64)
            .margin({ top: 12 })
            .width('100%')

            ProgressBar({
              text: "开合度",
              textSize: 16,
              textProgress: $progressValue,
              textProgressSize: 12,
              progressWidth: '80%',
              isSendCommand: true,
            })

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
      }
    }
    .backgroundColor($r("app.color.background_grey"))
    .padding({ left: 16, right: 16 })
  }
}