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

import util from '@ohos.util'
import hilog from '@ohos.hilog'


@Component
export struct WenShiduDialog {
  @Link isOn: boolean
  @State wendu1: number = 0
  @State wendu2: number = 0
  @State shidu1: number = 0

  randomDouble(minv: number, maxv: number): number {
    const randomNum: number = Math.random() * (maxv - minv + 1) + minv
    return Number.parseFloat(randomNum.toFixed(1))
  }

  transValue(value: number) {
    return Number.parseFloat(value.toFixed(1))
  }

  build() {
    Flex({ direction: FlexDirection.Row }) {
      Row() {
        Row() {
          Image($r('app.media.wenshidu_sm_icon'))
            .objectFit(ImageFit.Contain)
            .width('100%')
        }
        .width('80%')
        .height('80%')
        .backgroundColor('#90eeeeee')
        .borderRadius(15)
      }
      .width('60%')
      .height('100%')
      .justifyContent(FlexAlign.Center)

      Column() {
        Column() {

          Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.Center }) {
            Text('当前温度(摄氏度)')
            Text(String(this.wendu1) + '°C')
              .fontSize(25)
              .fontWeight(FontWeight.Bold)
              .margin({ top: 5 })
              .textAlign(TextAlign.Center)
          }
          .width('100%')
          .height('20%')


          Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.Center }) {
            Text('当前温度(华氏度)')
            Text(String(this.wendu2) + '℉')
              .fontSize(25)
              .fontWeight(FontWeight.Bold)
              .margin({ top: 5 })
              .textAlign(TextAlign.Center)
          }
          .width('100%')
          .height('20%')


          Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.Center }) {
            Text('当前湿度')
            Text(String(this.shidu1) + '°%RH')
              .fontSize(25)
              .fontWeight(FontWeight.Bold)
              .margin({ top: 5 })
              .textAlign(TextAlign.Center)
          }
          .width('100%')
          .height('20%')
        }

        Row() {
          Text('获取温湿度')
            .fontSize(18)
            .margin({ right: -15 })
          Toggle({ type: ToggleType.Switch, isOn: this.isOn })
            .selectedColor('#007DFF')
            .switchPointColor('#FFFFFF')
            .onChange((isOn: boolean) => {
              this.isOn = isOn
              if (isOn) {
                this.wendu1 = this.randomDouble(29, 30)
                this.wendu2 = this.transValue(this.wendu1 * (9 / 5) + 32)
                this.shidu1 = this.randomDouble(69, 70)
              } else {
                this.wendu1 = 0, this.wendu2 = 0, this.shidu1 = 0
              }
            })
            .width('50%')
            .height('10%')
        }
      }
      .width('40%')
      .height('100%')
      .justifyContent(FlexAlign.Center)
      .alignItems(HorizontalAlign.Center)

    }
    .width('100%')
    .height('100%')
    .backgroundImage($r('app.media.wenshidu_card_bg'))
    .backdropBlur(30)
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