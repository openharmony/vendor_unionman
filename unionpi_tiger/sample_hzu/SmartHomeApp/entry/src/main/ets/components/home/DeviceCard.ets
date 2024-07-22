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

import http from '@ohos.net.http'
import hilog from '@ohos.hilog'
import { runRequest } from '../../utils/Task'
import { DeviceTabs } from './DeviceTabs'
import { ResponseResult } from '../../utils/request'
import { InitDeviceDto } from '../interface'

@Component
struct NotFoundCard {
  build() {
    Column() {
      Row() {
        Image($r('app.media.lixian'))
          .objectFit(ImageFit.Contain)
          .width(70)
          .margin({ right: 10 })

        Text('未连接到设备')
          .fontSize(30)
          .fontWeight(700)
          .fontColor('#353535')
      }
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
  }
}


@Component
export struct DeviceCard {
  @State totalConnectNum: number = 0
  @Link isOn1: boolean
  @Link isOn2: boolean
  @State initReqTask: number = 0
  @Link deviceInfo1: InitDeviceDto
  @State imgRotate: number = 0
  @State textActive: boolean = false
  @State intervalSeqs: number[] = []
  initCallback = (err: Error, res: http.HttpResponse) => {
    if (err) {
      return
    }
    const data: ResponseResult = JSON.parse(res.result.toString()) as ResponseResult
    if (data.code == 200) {
      hilog.debug(0x0000, 'Test Tag', JSON.stringify(data.result))
      this.deviceInfo1 = data.result as InitDeviceDto
      this.totalConnectNum = 1
      this.intervalSeqs.forEach((intervalSeq: number) => clearInterval(intervalSeq))
      hilog.debug(0x0000, 'Test Tag', '清除轮询器: ' + this.intervalSeqs.toString())
    }
    hilog.debug(0x0000, 'Test Tag', JSON.stringify(data.msg))
  }

  aboutToAppear() {
    const timer = runRequest({
      url: '/device/init?key=testtopic/1',
      options: {
        method: http.RequestMethod.GET
      },
      callback: this.initCallback
    })
    this.intervalSeqs.push(timer)
  }

  build() {
    Column() {
      Row() {
        // 连接数

        Row() {
          Flex(
            {
              direction: FlexDirection.Column,
              justifyContent: FlexAlign.SpaceAround,
              alignItems: ItemAlign.Center
            }
          ) {
            Image($r('app.media.lianjieshu'))
              .objectFit(ImageFit.Contain)
              .fillColor('#3adbc1')
              .width(50)
              .height(50)

            Row() {
              Row()
                .width(12)
                .height(12)
                .borderRadius(12)
                .backgroundColor(this.totalConnectNum == 0 ? '#ffffff' : '#00b173')
                .margin({ right: 5 })
                .borderWidth(this.totalConnectNum == 0 ? 3 : 0)
                .borderStyle(BorderStyle.Solid)
                .borderColor('#aaa')
              Text('在线连接数')
                .fontColor($r('app.color.icon_fuzhu_color'))
                .fontSize(14)
            }

            Text(String(this.totalConnectNum))
              .fontSize(30)
              .fontWeight(750)
              .fontColor('#222')
          }

          Row().height('75%').width(2).backgroundColor('#ddd').margin({ left: 10 })
        }

        // 搜索附近设备
        Row() {
          Flex(
            {
              direction: FlexDirection.Column,
              justifyContent: FlexAlign.Center,
              alignItems: ItemAlign.Center
            }
          ) {
            Image($r('app.media.leida'))
              .objectFit(ImageFit.Contain)
              .fillColor('#ff3eeed1')
              .rotate({ angle: this.imgRotate })
              .animation({
                delay: 500,
                duration: 800,
                playMode: PlayMode.Normal,
                curve: Curve.FastOutLinearIn,
                iterations: 6,
                onFinish: () => {
                  this.imgRotate = 0
                  this.textActive = false
                }
              })

            Text('搜索附近设备')
              .fontColor(this.textActive ? $r('app.color.app_header_color')
                                         : $r('app.color.icon_fuzhu_color'))
              .fontSize(15)
              .textAlign(TextAlign.Center)
              .onClick((e: ClickEvent) => {
                this.textActive = true
                this.imgRotate = 360
                console.log('旋转' + this.imgRotate.toString())
                const initCallback = (err: Error, res: http.HttpResponse) => {
                  if (err) {
                    return
                  }
                  const data: ResponseResult = JSON.parse(res.result.toString()) as ResponseResult
                  if (data.code == 200) {
                    hilog.debug(0x0000, 'Test Tag', JSON.stringify(data.result))
                    this.deviceInfo1 = data.result as InitDeviceDto
                    clearInterval(this.initReqTask)
                    hilog.debug(0x0000, 'Test Tag', '清楚轮询器')
                  }
                  hilog.debug(0x0000, 'Test Tag', JSON.stringify(data.msg))
                }

                const timer = runRequest({
                  url: '/device/init?key=testtopic/1',
                  options: {
                    method: http.RequestMethod.GET
                  },
                  callback: initCallback
                })

                this.initReqTask = timer
              })
          }
        }
        .width('60%')
        .height('100%')
        .margin({ bottom: 30 })

      }
      .justifyContent(FlexAlign.SpaceAround)
      .width('100%')
      .height('30%')
      .backgroundColor('#99ffffff')
      .borderRadius(15)
      .padding(10)


      Column() {
        if (this.totalConnectNum == 0) {
          NotFoundCard()
        } else {
          DeviceTabs({
            isOn1: $isOn1,
            isOn2: $isOn2,
            deviceInfo1: $deviceInfo1
          })
        }
      }
      .width('100%')
      .height('65%')
      .margin({ top: 15
      })
      .backgroundColor('#99ffffff')
      .backdropBlur(20)
      .backgroundBlurStyle(BlurStyle.Thin)
      .borderRadius(15)
      .shadow({
        radius: 15,
        color: '#aa949393',
        offsetY: 10,
        offsetX: 10
      })

    }
    .backgroundImage($r('app.media.device_bg'))
    .backgroundImageSize({
      width: '100%',
      height: '100%'
    })
    .width('100%')
    .height('100%')
    .padding(15)
    .backdropBlur(30)
    .borderRadius(25)
    .shadow({
      radius: 10,
      color: '#89949393',
      offsetY: 15,
      offsetX: 15
    })
  }
}