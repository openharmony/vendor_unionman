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

import { InitDeviceDto } from '../interface'
import { DeviceInfo } from './DeviceInfo'

@Component
export struct DeviceTabs {
  @Builder tabBuilder(index: number, name: string) {
    Row() {
      Column() {
        Text(name)
          .fontColor(this.currentIndex === index ? this.selectedFontColor : this.fontColor)
          .fontSize(20)
          .fontWeight(this.currentIndex === index ? 500 : 400)

        Divider()
          .vertical(false)
          .width('20%')
          .color((this.currentIndex === index ? this.selectedFontColor : '#55767677'))
          .strokeWidth(5)
          .margin({ top: 8 })
          .lineCap(LineCapStyle.Round)
      }
      .justifyContent(FlexAlign.Center)
      .width('100%')
      .height('100%')

      Divider()
        .vertical(true)
        .color('#ffe0dbdb')
        .strokeWidth(3)
        .height('50%')
        .lineCap(LineCapStyle.Round)
    }
    .width('100%')
    .height('100%')
    .alignItems(VerticalAlign.Center)

  }

  @State currentIndex: number = 0
  @State fontColor: string = '#182431'
  @State selectedFontColor: string = '#007DFF'
  @Link isOn1: boolean
  @Link isOn2: boolean
  @Link deviceInfo1: InitDeviceDto
  private controller: TabsController = new TabsController()

  build() {
    Column() {
      Tabs({
        barPosition: BarPosition.Start,
        index: this.currentIndex,
        controller: this.controller
      }) {
        TabContent() {
          DeviceInfo(
            {
              isON: $isOn1,
              deviceInfo: $deviceInfo1
            }
          )
        }
        .tabBar(this.tabBuilder(0, '设备1'))

      }
      .onChange((index: number) => {
        this.currentIndex = index
      })
      .scrollable(false)
      .barMode(BarMode.Fixed)
      .barHeight('20%')
      .animationDuration(500)
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Start)
  }
}