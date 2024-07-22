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

import dayjs from 'dayjs';

@Component
export struct WeatherCard {
  @State temperature: number = 30
  @State timeStr: string = '19:40'
  @State day: string = 'Thursday'
  @State month: string = 'June 27th'

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
      const dataSplit: string[] = dateStr.split('-')
      this.timeStr = dataSplit[0] + ':' + dataSplit[1]
    })
  }

  build() {
    Column() {
      Flex({
        direction: FlexDirection.Column,
        justifyContent: FlexAlign.Center,
        alignItems: ItemAlign.Center
      }) {
        // 天气
        Row() {
          // 天气图标
          Row() {
            Image($r('app.media.sun'))
              .objectFit(ImageFit.Contain)
              .width('100%')
              .height('100%')
          }
          .width(60)
          .height(60)
          .margin({
            right: 15
          })
          // 温度
          Text(String(this.temperature) + '℃')
            .fontColor(Color.White)
            .fontSize('30fp')
        }
        // 时间
        Row() {
          Text(this.timeStr)
            .fontSize(55)
            .fontColor(Color.White)
        }

        // 年月日
        Row() {
          Text(this.day + ' ' + this.month)
            .fontSize(20)
            .fontColor(Color.White)
        }
      }

    }
    .backgroundImage($r('app.media.weather_bg'))
    .backgroundImageSize({
      width: '100%',
      height: '100%'
    })
    .width('100%')
    .height('100%')
    .borderRadius(25)
    .shadow({
      radius: 10,
      color: '#89949393',
      offsetY: 15,
      offsetX: 15
    })
  }
}