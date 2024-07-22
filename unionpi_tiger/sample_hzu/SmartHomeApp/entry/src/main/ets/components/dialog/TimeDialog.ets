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

import systemDateTime from '@ohos.systemDateTime'
import hilog from '@ohos.hilog'
import dayjs from 'dayjs'
import util from '@ohos.util'

@Component
export struct TimeDialog {
  aboutToAppear() {
    setInterval(() => {
      this.timeMoudle()
    }, 1000)
  }

  async timeMoudle() {
    await systemDateTime.getCurrentTime(false, (err: Error, time: number) => {
      if (err) {
        hilog.debug(0x0000, 'Test Tag', err.message)
        return
      }
      const date = new Date(time)
      const dateStr: string = dayjs(date).format('HH-mm').toString()
      const dateSplit: string[] = dateStr.split('-')
      this.hourStr = dateSplit[0]
      this.minuteStr = dateSplit[1]
    })
  }

  @State wendu: number = 30
  @State dateStr: string = '2024-06-27'
  @State hourStr: string = '19'
  @State minuteStr: string = '40'
  @State dayStr: string = '星期四'

  build() {
    Column() {
      Row() {
        Image($r('app.media.sun'))
          .objectFit(ImageFit.Contain)
          .width(80)
          .margin({ right: 20 })
        Text(this.wendu.toString() + '℃')
          .fontSize(35)
          .fontWeight(650)
          .fontColor(Color.White)
      }
      .height('20%')
      .width('100%')
      .margin({ left: 20, top: 10 })

      Column() {
        Text(this.dateStr)
          .fontSize(35)
          .fontColor(Color.White)
          .fontWeight(650)

        Row() {
          Text(this.hourStr)
            .fontSize(65)
            .fontColor(Color.White)
            .fontWeight(800)

          Text(':').fontSize(45)
            .fontColor(Color.White)
            .fontWeight(800)
            .margin({ left: 8, right: 8 })

          Text(this.minuteStr)
            .fontSize(65)
            .fontColor(Color.White)
            .fontWeight(800)

        }
        .margin({ top: 10 })

        Text(this.dayStr)
          .fontSize(45)
          .fontColor(Color.White)
          .fontWeight(650)
          .margin({ top: 10 })
      }
      .height('70%')
      .width('100%')
      .margin({ top: 20 })
    }
    .width('100%')
    .height('100%')

    .backgroundImage($r('app.media.time_bg'))
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