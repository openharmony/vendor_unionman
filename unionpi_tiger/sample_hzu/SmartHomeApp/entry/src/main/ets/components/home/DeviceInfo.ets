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

import hilog from '@ohos.hilog'
import emitter from '@ohos.events.emitter'
import { InitDeviceDto } from '../interface'

@Component
export struct DeviceInfo {
  @Link deviceInfo: InitDeviceDto
  @State name: string = '润和3861'
  @State signal: string = '强'
  @State ip: string = '192.168.248.12'
  @State connectTime: string = '2024-06-05 21:34'
  @State exteriorNum: number = 2
  @Link isON: boolean

  build() {
    Row() {
      Row() {
        Image($r('app.media.hi3861_icon'))
          .objectFit(ImageFit.Contain)
          .fillColor('#1e48ed')
      }
      .width('40%')
      .height('100%')
      .padding(10)

      Column({ space: 8 }) {
        Text('设备名称: ' + this.deviceInfo.deviceName)
        Text('连接信号强度: ' + this.deviceInfo.signal)
        Text('设备IP: ' + this.deviceInfo.ip)
        Text('连接时间: ' + this.deviceInfo.connectTime)
        Text('设备外设数量: ' + String(this.deviceInfo.device.length))
        Button('查看外设详情', { type: ButtonType.Capsule })
          .translate({ x: '8%' })
          .onClick((e: ClickEvent) => {
            this.isON = true
            hilog.debug(0x0000, 'Test Tag', 'click event')
          })
      }
      .width('60%')
      .height('100%')
      .justifyContent(FlexAlign.Center)
      .alignItems(HorizontalAlign.Start)
      .translate({
        x: '8%'
      })
    }
    .width('100%')
    .height('100%')
    .borderWidth({
      top: 2
    })
    .borderStyle(BorderStyle.Solid)
    .borderColor('#ff1af1a0')
  }
}