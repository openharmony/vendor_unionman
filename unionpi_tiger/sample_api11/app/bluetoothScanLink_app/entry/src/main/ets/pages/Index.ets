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
import emitter from '@ohos.events.emitter';
import Bluetooth from '../common/core/Bluetooth';
import Logger from '../common/core/Logger';
import SignDevice, { BlueToothEvent, DeviceText } from '../common/core/SignDevice';
let logger = new Logger()
let blueTooth = new Bluetooth()
let signDevice = new SignDevice(blueTooth)
@Entry
@Component
struct Index {
  @State message: string = '等待体征测试';
  aboutToAppear() {
    emitter.on(BlueToothEvent, this.onSignData)
    signDevice.startScan()
  }

  onSignData(data: emitter.EventData) {
    this.message = `${DeviceText[data?.data?.deviceType]}测试数据：${data?.data?.value}`
  }
  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
      }
      .width('100%')
    }
    .height('100%')
  }
}