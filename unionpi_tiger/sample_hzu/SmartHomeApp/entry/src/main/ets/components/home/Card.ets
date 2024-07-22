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

@Component
export struct Card {
  @Link isOn: boolean
  @State deviceName: string = '舵机'
  @State noActiveColor: string = '#6D7198'
  @State activeColor: string = '#61f3ec'
  @Link iconUrl: Resource
  @State cardBg: Resource = $r('app.media.card_bg')
  @Link isChangeIconColor: string

  build() {
    Column() {
      Flex({
        direction: FlexDirection.Column
      }) {
        Flex({ direction: FlexDirection.RowReverse }) {
          Image(this.iconUrl)
            .width('50%')
            .height('100%')
            .objectFit(ImageFit.Contain)
            .fillColor(this.isChangeIconColor)
        }
        .height('55%')
        .width('100%')

        Column() {
          Text((this.isOn ? 'ON' : 'OFF'))
            .fontSize(45)
            .fontColor((this.isOn ? this.activeColor : this.noActiveColor))
            .fontWeight(700)
            .margin({
              bottom: 10
            })

          Text(this.deviceName)
            .fontSize(25)
            .fontColor((this.isOn ? '#fff' : '#6d95b2'))
        }
        .margin({
          left: 30
        })
      }
    }
    .width('100%')
    .height('100%')
    .backgroundImage(this.card_bg)
    .backgroundImageSize({
      width: '100%',
      height: '100%'
    })
    .padding(15)
    .borderRadius(25)
    .borderWidth((this.isOn ? 2 : 0))
    .borderStyle(BorderStyle.Solid)
    .borderColor(this.activeColor)
    .shadow({
      radius: 10,
      color: '#89949393',
      offsetY: 15,
      offsetX: 15
    })
  }
}