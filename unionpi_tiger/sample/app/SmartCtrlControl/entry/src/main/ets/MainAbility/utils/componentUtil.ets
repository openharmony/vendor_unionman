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
@CustomDialog
export struct deleteDialog {
  dialogController: CustomDialogController;
  confirm: () => void
  @State confirmed: boolean = true

  build() {
    Column() {
      Row() {
        Text('此操作将删除所有笔记。是否删除？').fontSize(16)
      }
      .margin({ top: 25 })
      .width('100%')
      .justifyContent(FlexAlign.Center)

      Row() {
        Toggle({ type: ToggleType.Checkbox, isOn: this.confirmed })
          .size({ width: 20, height: 20 })
          .selectedColor($r('app.color.checked_blue'))
          .onChange((isOn: boolean) => {
            this.confirmed = isOn
          })
        Text('我已阅读并了解').fontSize(16)
      }
      .justifyContent(FlexAlign.Start)
      .onClick(() => {
        this.confirmed = !this.confirmed
      })

      Flex({ justifyContent: FlexAlign.SpaceEvenly }) {
        Text('取消')
          .fontSize(16)
          .backgroundColor(0xffffff)
          .fontColor($r('app.color.background_blue'))
          .onClick(() => {
            this.dialogController.close()
          })
        Divider()
          .height(5)
          .vertical(true)
          .color('#D1D1D1')
          .strokeWidth(2)

        Button('删除', { type: ButtonType.Normal })
          .enabled(this.confirmed)
          .fontSize(16)
          .backgroundColor(0xffffff)
          .opacity(this.confirmed ? 1 : 0.4)
          .fontColor(Color.Red)
          .onClick(() => {
            this.dialogController.close()
            this.confirm()
            router.replace({
              url: 'pages/SpiDevice',
            })
          })
      }.margin({ bottom: 15 })
    }
    .alignItems(HorizontalAlign.Start)
    .justifyContent(FlexAlign.SpaceBetween)
    .width('70%')
    .height(180)
    .borderRadius(25)
    .backgroundColor(Color.White)
  }
}

@Component
export struct noteItem {
  @State index: number = 0
  @State title: string = ''
  @State date: string = ''
  @State backgroundColor: ResourceColor = $r('app.color.background_white')
  callback: (event?: ClickEvent) => void

  build() {
    Button({ type: ButtonType.Normal }) {
      Column() {
        Text(this.title)
          .fontSize(20)
          .textOverflow({ overflow: TextOverflow.Ellipsis })
          .maxLines(1)
        Text(this.date)
          .fontColor($r('app.color.placeholderColor_grey'))
          .fontSize(14)
          .margin({ top: 5 })
      }
      .width('98%')
      .alignItems(HorizontalAlign.Start)
      .justifyContent(FlexAlign.Center)
      .padding({ left: 10 })
    }
    .height(80)
    .borderRadius(20)
    .backgroundColor(this.backgroundColor)
    .onClick(this.callback)
  }
}

@Component
export struct clickItem {
  @State imgSrc: Resource = $r('app.media.icon')
  @State text: string = ''
  callback: (event?: ClickEvent) => void

  build() {
    Column() {
      Image(this.imgSrc)
        .width(24)
        .height(24)
      Text(this.text)
        .margin({ top: 5 })
        .fontSize(12)
    }
    .onClick(this.callback)
  }
}

@Component
export struct cardItem {
  text: string | Resource
  url: string

  build() {
    Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.Center, alignItems: ItemAlign.Center }) {
      Text(this.text)
        .fontSize(25)
        .fontColor(Color.White)
    }
    .margin({ left: 30 })
    .height('42%')
    .width('20%')
    .borderRadius(30)
    .backgroundColor($r('app.color.background_blue'))
    .onClick(() => {
      router.push({
        url: this.url,
      })
    })
  }
}

@Component
export struct TextRow {
  @State text: string = ''
  @State value: string = ''

  build() {
    Row() {
      Column() {
        Text(this.text)
          .fontSize(40)
          .fontColor(Color.White)
      }.width('48%')

      Column() {
        Text(this.value)
          .fontSize(40)
          .fontColor(Color.White)
      }.width('48%')
    }
  }
}