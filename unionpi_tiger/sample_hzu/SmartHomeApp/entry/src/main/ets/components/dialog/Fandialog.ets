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

import pwmtest from '@ohos.pwmtest'

@Component
struct MyBtn {
  @State context: string = ''
  @State btnWight: string = '60%'
  @State color: string = ''

  build() {
    Button(this.context, { type: ButtonType.Capsule })
      .width(this.btnWight)
      .padding(10)
      .backgroundColor(this.color)
      .shadow({
        color: '#89909090',
        radius: 10,
        offsetY: 2,
        offsetX: 2
      })
  }
}


@Component
export struct Fandialog {
  @State title: string = '舵机'
  @State btnGroupColor: string[] = [
    '#015ffd', '#fe82d6', '#20b9fe', '#63be25'
  ]
  @State btnGroupContext: string[] = [
    '旋转45度', '旋转90度', '旋转180度', '旋转0度'
  ]

  build() {
    Column() {
      Flex({ direction: FlexDirection.Column,
        justifyContent: FlexAlign.Center,
        alignItems: ItemAlign.Center }) {
        Text(this.title)
          .fontSize(45)
          .fontColor('#ffffff')

        Divider()
          .vertical(false)
          .color('#d5d5d5')
          .strokeWidth(5)
          .width(50)
          .margin({ top: 5 })
      }
      .width('100%')
      .height('20%')

      Row() {
        Image($r('app.media.fan_sm_bg'))
          .objectFit(ImageFit.Contain)
          .width('60%')
          .height('100%')
          .margin({ bottom: 50 })


        Column({ space: 20 }) {
          ForEach(this.btnGroupContext, (item: string, index: number) => {
            MyBtn({
              context: item,
              color: this.btnGroupColor[index]
            })
              .onClick((e: ClickEvent) => {
                if (index == 0) { // 旋转45度
                  pwmtest.setPwmStatus(1, 45)
                } else if (index == 1) { // 旋转90度
                  pwmtest.setPwmStatus(1, 90)
                } else if (index == 2) { // 旋转180度
                  pwmtest.setPwmStatus(1, 180)
                } else if (index == 3) { // 旋转0度
                  pwmtest.setPwmStatus(1, 0)
                }
              })

          }, (index: number) => index.toString())
        }
        .width('40%')
        .height('100%')
        .justifyContent(FlexAlign.Center)
        .margin({ right: 20 })
      }
      .width('100%')
      .height('80%')
    }
    .width('100%')
    .height('100%')
    .backgroundImage($r('app.media.fan_card_bg'))
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