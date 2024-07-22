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
import { ConnectDeviceDialog } from '../dialog/ConnectDeviceDialog'
import { Fandialog } from '../dialog/Fandialog'
import { LedDialog } from '../dialog/LedDialog'
import { TimeDialog } from '../dialog/TimeDialog'
import { WenShiduDialog } from '../dialog/WenShiduDialog'
import { InitDeviceDto } from '../interface'
import { Card } from './Card'
import { DeviceCard } from './DeviceCard'
import { WeatherCard } from './WeatherCard'


@Entry
@Component
export struct Home {
  @State cardWeight: string = '100%'
  @State cardHeight: string = '100%'
  @State text: string = 'drag'

  changeIndex(index1: number, index2: number) { //交换数组位置
  }

  @Builder pixelMapBuilder() { //拖拽过程样式
    Column() {
      Text(this.text)
        .fontSize(16)
        .backgroundColor(0xF9CF93)
        .width(80)
        .height(80)
        .textAlign(TextAlign.Center)
    }
  }

  @State timeDialogEnable: boolean = false
  @State ledDialogEnable: boolean = false
  @State fanDialogEnable: boolean = false
  @State wenshiduDialogEnable: boolean = false
  @State deviceDialogEnable: boolean = false
  @State @Watch('watchDialog') maskVisable: boolean = false
  @State @Watch('watchIsON1') isOn1: boolean = false
  @State @Watch('watchIsON2') isOn2: boolean = false
  @State scoleX: number = 0;
  @State scoleY: number = 0;
  @State deviceScoleX: number = 0;
  @State deviceScoleY: number = 0;
  @State @Watch('watchLedIsOn') ledIsOn: boolean = false
  @State fanIsOn: boolean = false
  @State @Watch('watchWenshiduIsOn') wenshiduIsOn: boolean = false
  @State ledIcon: Resource = $r('app.media.led_noicon')
  @State wenshiduIcon: Resource = $r('app.media.wenshidu_sm_icon')
  @State fanIcon: Resource = $r('app.media.fan_sm_bg')
  @State ledChangeIconColor: string = ''
  @State fanChangeIconColor: string = '#d6e2f0'
  @State wenshiduChangeIconColor: string = '#d6e2f0'

  watchLedIsOn() {
    if (this.ledIsOn) {
      this.ledIcon = $r('app.media.led_icon')
    } else {
      this.ledIcon = $r('app.media.led_noicon')
    }
  }

  watchWenshiduIsOn() {
    if (this.wenshiduIsOn) {
      this.wenshiduChangeIconColor = '#4ad3ff'
    } else {
      this.wenshiduChangeIconColor = '#d6e2f0'
    }
  }

  @State deviceInfo1: InitDeviceDto = {
    deviceName: '无',
    device: [],
    deviceNum: 0,
    ip: '',
    signal: ''
  }

  watchIsON1() {
    if (this.isOn1) {
      hilog.debug(0x0000, 'Test Tag', JSON.stringify(this.deviceInfo1))
      this.maskVisable = true
      this.deviceDialogEnable = true
      this.deviceScoleX = 1
      this.deviceScoleY = 1
    }
  }

  watchIsON2() {
    if (this.isOn2) {
      this.maskVisable = true
      this.deviceDialogEnable = true
      this.deviceScoleX = 1
      this.deviceScoleY = 1
    }
  }

  watchDialog() {
    if (!this.maskVisable) {
      this.ledDialogEnable = false
      this.fanDialogEnable = false
      this.wenshiduDialogEnable = false
      this.deviceDialogEnable = false
      this.timeDialogEnable = false
      this.isOn1 = false
      this.isOn2 = false
    }
  }

  build() {
    Column() {
      // 黑布遮罩层
      Column() {
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#99222222')
      .position({ x: 0, y: 0 })
      .zIndex(10)
      .visibility((this.maskVisable ? Visibility.Visible : Visibility.None))
      .onClick((e: ClickEvent) => {
        this.maskVisable = !this.maskVisable
        this.scoleX = 0
        this.scoleY = 0
        this.deviceScoleX = 0
        this.deviceScoleY = 0
      })

      TimeDialog()
        .position({
          x: '50%',
          y: '50%'
        })
        .translate({
          x: '-50%',
          y: '-50%'
        })
        .width('50%')
        .height('70%')
        .zIndex(100)
        .scale({
          x: this.scoleX,
          y: this.scoleY
        })
        .animation({
          duration: 500,
          delay: 100,
          curve: Curve.Friction,
          playMode: PlayMode.Alternate,
        })
        .visibility((this.timeDialogEnable ? Visibility.Visible : Visibility.None))


      // led弹窗
      LedDialog({
        ledIsOn: $ledIsOn
      })
        .position({
          x: '50%',
          y: '50%'
        })
        .translate({
          x: '-50%',
          y: '-50%'
        })
        .width('50%')
        .height('70%')
        .zIndex(100)
        .scale({
          x: this.scoleX,
          y: this.scoleY
        })
        .animation({
          duration: 500,
          delay: 100,
          curve: Curve.Friction,
          playMode: PlayMode.Alternate,
        })
        .visibility((this.ledDialogEnable ? Visibility.Visible : Visibility.None))


      // fan弹窗
      Fandialog()
        .position({
          x: '50%',
          y: '50%'
        })
        .translate({
          x: '-50%',
          y: '-50%'
        })
        .width('50%')
        .height('70%')
        .zIndex(100)
        .scale({
          x: this.scoleX,
          y: this.scoleY
        })
        .animation({
          duration: 500,
          delay: 100,
          curve: Curve.Friction,
          playMode: PlayMode.Alternate,
        })
        .visibility((this.fanDialogEnable ? Visibility.Visible : Visibility.None))

      // wenshidu弹窗
      WenShiduDialog({
        isOn: $wenshiduIsOn
      })
        .position({
          x: '50%',
          y: '50%'
        })
        .translate({
          x: '-50%',
          y: '-50%'
        })
        .width('50%')
        .height('70%')
        .zIndex(100)
        .scale({
          x: this.scoleX,
          y: this.scoleY
        })
        .animation({
          duration: 500,
          delay: 100,
          curve: Curve.Friction,
          playMode: PlayMode.Alternate,
        })
        .visibility((this.wenshiduDialogEnable ? Visibility.Visible : Visibility.None))

      // device弹窗
      ConnectDeviceDialog({
        deviceInfo: $deviceInfo1
      })
        .position({
          x: '50%',
          y: '50%'
        })
        .translate({
          x: '-50%',
          y: '-50%'
        })
        .width('50%')
        .height('70%')
        .zIndex(100)
        .scale({
          x: this.deviceScoleX,
          y: this.deviceScoleY
        })
        .animation({
          duration: 500,
          delay: 100,
          curve: Curve.Friction,
          playMode: PlayMode.Alternate,
        })
        .visibility((this.deviceDialogEnable ? Visibility.Visible : Visibility.None))


      Grid() {
        GridItem() {
          WeatherCard()
            .width(this.cardWeight)
            .height(this.cardHeight)
            .onClick((e: ClickEvent) => {
              this.timeDialogEnable = true
              this.maskVisable = true
              this.scoleX = 1
              this.scoleY = 1
            })
        }
        .columnStart(1)
        .columnEnd(1)


        GridItem() {
          Card({
            cardBg: $r('app.media.led_bg'),
            deviceName: '电灯泡',
            isOn: $ledIsOn,
            iconUrl: $ledIcon,
            isChangeIconColor: $ledChangeIconColor
          })
            .width(this.cardWeight)
            .height(this.cardHeight)
            .onClick((e: ClickEvent) => {
              this.ledDialogEnable = true
              this.maskVisable = true
              this.scoleX = 1
              this.scoleY = 1
            })
        }


        GridItem() {
          DeviceCard({
            isOn1: $isOn1,
            isOn2: $isOn2,
            deviceInfo1: $deviceInfo1
          })
            .width(this.cardWeight)
            .height(this.cardHeight)

        }
        .rowStart(1)
        .rowEnd(2)


        GridItem() {
          Card({
            cardBg: $r('app.media.fan_bg'),
            isOn: $fanIsOn,
            iconUrl: $fanIcon,
            isChangeIconColor: $fanChangeIconColor
          })
            .width(this.cardWeight)
            .height(this.cardHeight)
            .onClick((e: ClickEvent) => {
              this.fanDialogEnable = true
              this.maskVisable = true
              this.scoleX = 1
              this.scoleY = 1
            })
        }

        GridItem() {
          Card(
            {
              isOn: $wenshiduIsOn,
              iconUrl: $wenshiduIcon,
              deviceName: '温湿度传感器',
              cardBg: $r('app.media.wenshidu_bg'),
              isChangeIconColor: $wenshiduChangeIconColor
            }
          )
            .width(this.cardWeight)
            .height(this.cardHeight)
            .onClick((e: ClickEvent) => {
              this.wenshiduDialogEnable = true
              this.maskVisable = true
              this.scoleX = 1
              this.scoleY = 1
            })
        }

      }
      .rowsTemplate('1fr 1fr')
      .columnsTemplate('1fr 1fr 2fr')
      .rowsGap(28)
      .columnsGap(28)
      .width('100%')
      .height('100%')
      .padding(25)
      .editMode(true)
      .onItemDragStart((event: ItemDragInfo, itemIndex: number) => { //第一次拖拽此事件绑定的组件时，触发回调。
        console.log(String(event.x), String(event.y))
        return this.pixelMapBuilder() //设置拖拽过程中显示的图片。
      })
    }
    .width('100%')
    .height('100%')
  }
}