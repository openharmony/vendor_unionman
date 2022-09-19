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

@Component
export struct DeviceTitle {
  deviceTitle: Resource = $r("app.string.smartCurtain")

  build() {
    Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center }) {
      Image($r("app.media.ic_public_back"))
        .width(24)
        .height(24)
        .onClick(() => {
          router.back()
        })
      Text(this.deviceTitle)
        .margin({ left: 16 })
        .fontSize(18)
        .width(193)
        .flexGrow(1)
      Image($r("app.media.icon_about"))
        .margin({ left: 39 })
        .width(24)
        .height(24)
        .onClick(() => {
        })
    }
    .height(60)
  }
}

@Component
export struct DeviceLogo {
  @State deviceLogo: Resource = $r("app.media.icon_curtain")

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Image(this.deviceLogo)
        .width(200)
        .height(200)
    }
    .margin({ top: 8 })
  }
}

@Component
export struct DeviceState {
  @State deviceSwitchText: Resource = $r("app.string.closed")
  deviceSwitchTextColor
  deviceState: Resource | string
  deviceStateColor: Resource
  currentState: Resource
  currentStateColor: Resource
  isAlert: boolean
  switchImg: Resource
  batteryImg: Resource
  electricQuantity: string
  callback: (event?: ClickEvent) => void

  build() {
    Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center }) {
      Text(this.deviceSwitchText)
        .fontSize(18)
        .fontWeight(13.5)
        .fontColor(this.deviceSwitchTextColor)
      Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
        Text(this.deviceState)
          .fontSize(24)
          .fontColor($r("app.color.background_green"))
          .opacity(0.9)
          .fontColor(this.deviceStateColor)
        Text(this.currentState)
          .fontSize(12)
          .opacity(0.6)
          .fontColor(this.currentStateColor)
      }
      .width(114)
      .layoutWeight(1)

      if (this.isAlert) {
        Image(this.switchImg)
          .width(56)
          .height(56)
          .onClick(this.callback)
      }
      else {
        Image(this.batteryImg)
          .width(24)
          .height(24)
        Text(this.electricQuantity)
          .width(30)
          .height(24)
          .fontSize(14)
          .margin({ left: 1 })
      }
    }
    .backgroundColor($r("app.color.background_white"))
    .height(88)
    .padding({ left: 16, right: 16 })
    .borderRadius(16)
  }
}

@Component
export struct componentsRow {
  text: Resource | string
  textSize: number
  @State textColor: Resource = $r("app.color.background_black")
  textSub: string
  textSubSize: number
  textSubColor: Resource
  @State image: Resource = $r("app.media.icon_close_door")
  frameWidth: string

  build() {
    Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceBetween }) {
      Text(this.text)
        .fontSize(this.textSize)
        .fontColor(this.textColor)
      if (this.textSub) {
        Text(this.textSub)
          .fontSize(this.textSubSize)
          .fontColor(this.textSubColor)
      } else {
        Image(this.image)
          .width(24)
          .height(24)
      }
    }
    .height(64)
    .width(this.frameWidth)
    .backgroundColor($r("app.color.background_white"))
    .padding({ left: 16, right: 16 })
    .borderRadius(16)
  }
}

@Component
export struct Stow {
  build() {
    Flex({ direction: FlexDirection.Column }) {
      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
        Text($r("app.string.stow"))
          .fontSize(16)
          .opacity(0.6)
        Image($r("app.media.icon_tucked"))
          .width(25)
          .height(25)
      }.margin({ top: 8 }).height(35)

      Text($r("app.string.equipmentServices")).fontSize(18).margin({ top: 16 })
    }
  }
}

@Component
export struct ProgressBar {
  text: Resource | string
  textSize: number
  @Link textProgress: number
  textProgressSize: number
  progressWidth: string
  isSendCommand: boolean
  deviceId: string

  build() {
    Row() {
      Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Start, justifyContent: FlexAlign.Center }) {
        Text(this.text)
          .fontSize(this.textSize)
        Text(this.textProgress.toFixed(0) + '%')
          .fontSize(this.textProgressSize)
          .fontColor($r("app.color.background_blue"))
      }

      Row() {
        Slider({
          value: this.textProgress,
          min: 0,
          max: 100,
          step: 1.8,
        })
          .blockColor(Color.Blue)
          .trackColor($r("app.color.background_grey"))
          .selectedColor(Color.Blue)
          .showSteps(true)
          .showTips(true)
          .onChange((value: number, mode: SliderChangeMode) => {
            this.textProgress = value
          })
      }
      .width(this.progressWidth)
    }
    .height(64)
    .width('100%')
    .backgroundColor($r("app.color.background_white"))
    .padding({ left: 16, right: 16 })
    .borderRadius(16)
    .margin({ top: 12 })
  }
}

@Component
export struct componentsRowColumn {
  text: Resource
  textSize: number
  textColumn: Resource
  textColumnSize: number
  isSwitchImge: boolean
  isSendCommand: boolean
  deviceId: string

  build() {
    Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceBetween }) {
      Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Start, justifyContent: FlexAlign.Center }) {
        Text(this.text)
          .fontSize(this.textSize)
        Text(this.textColumn)
          .fontSize(this.textColumnSize)
          .opacity(0.6)
      }

      if (this.isSwitchImge) {
        Image($r("app.media.img_switch_on"))
          .width(40)
          .height(24)
      } else {
        Image($r("app.media.icon_notice"))
          .width(25)
          .height(25)
      }
    }
    .height(64)
    .width('100%')
    .backgroundColor($r("app.color.background_white"))
    .padding({ left: 16, right: 16 })
    .borderRadius(16)
    .margin({ top: 12 })
  }
}