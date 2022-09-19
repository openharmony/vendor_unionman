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
import gpioLed from '@ohos.gpioLed';

@Component
@CustomDialog
struct MyDialog {
  dialogController: CustomDialogController;
  @Link isON: boolean
  @State select: number = 0
  private lights: string[] = ['客厅灯', '卧室灯', '餐厅灯', '阳台灯']
  @Link selectLight: string
  @Link pin: number
  @State selected: string = '客厅灯'

  build() {
    Column() {
      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center }) {
        Image($r("app.media.icon_not_ok"))
          .width(24)
          .height(24)
          .onClick(() => {
            this.dialogController.close()
          })
        Blank()
          .flexGrow(1)
        Image($r("app.media.ic_public_ok_filled"))
          .width(24)
          .height(24)
          .onClick(() => {
            this.dialogController.close()
            this.selectLight = this.selected
            gpioLed.getLedStatus(this.pin).then((result) => {
              this.isON = Boolean(result)
            })
          })
      }
      .padding({ left: 12, right: 12 })
      .height(48)

      TextPicker({ range: this.lights, selected: this.select })
        .height('50%')
        .onChange((value: string, index: number) => {
          this.selected = value
          this.pin = index + 384
        })
    }
  }
}

/**
 * 灯光控制界面
 */
@Entry
@Component
struct GpioDeviceControl {
  scroller: Scroller = new Scroller()
  @State @Watch("setCommandMessage") isON: boolean = false
  @State pin: number = 384
  deviceLogo: Resource = $r("app.media.icon_fraction_lamp_offline")
  deviceSwitchText: Resource = $r("app.string.closed")
  deviceSwitchTextColor = $r("app.color.background_black")
  @State switchImg: Resource = $r("app.media.icon_switch_off")
  @State selectedLight: string = '客厅灯'
  dialogController: CustomDialogController = new CustomDialogController({
    builder: MyDialog({ isON: $isON, selectLight: $selectedLight, pin: $pin }),
    autoCancel: false,
    alignment: DialogAlignment.Bottom,
  });

  setCommandMessage() {
    if (this.isON) {
      this.deviceLogo = $r("app.media.icon_fraction_lamp_online")
      this.switchImg = $r("app.media.icon_switch_on")
      this.deviceSwitchText = $r("app.string.opened");
    } else {
      this.deviceLogo = $r("app.media.icon_fraction_lamp_offline")
      this.switchImg = $r("app.media.icon_switch_off")
      this.deviceSwitchText = $r("app.string.closed");
    }
  }

  sendDeviceCommand() {
    if (this.isON) {
      gpioLed.setLedStatus(this.pin, gpioLed.LED_ON)
    } else {
      gpioLed.setLedStatus(this.pin, gpioLed.LED_OFF)
    }
  }

  onPageShow() {
    gpioLed.getLedStatus(this.pin).then((result) => {
      this.isON = Boolean(result)
    })
  }

  build() {
    Flex({ direction: FlexDirection.Column }) {
      DeviceTitle({ deviceTitle: $r("app.string.smartLight") })
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
                this.sendDeviceCommand()
              }
            })

            Column() {
              componentsRow({
                text: $r("app.string.quickControl"),
                textColor: $r("app.color.background_blue"),
                textSize: 16,
                textSub: this.selectedLight,
                textSubSize: 16,
                textSubColor: $r("app.color.background_blue"),
                frameWidth: '100%',
              })
            }.onClick(() => {
              this.pin = 384
              this.dialogController.open()
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