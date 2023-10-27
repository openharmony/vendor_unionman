/*
 * Copyright 2023 Unionman Technology Co., Ltd.
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
import router from '@ohos.router';
import abilityAccessCtrl from '@ohos.abilityAccessCtrl';
import promptAction from '@ohos.promptAction';
import deviceManager from '@ohos.distributedHardware.deviceManager';
import { DeviceDialog } from '../common/DeviceDialog';
import { RemoteDeviceModel } from '../model/RemoteDeviceModel';

const DATA_CHANGE = 'dataChange';
const EXIT = 'exit';
const ONE_THOUSAND: number = 1000;

@Component
export struct TitleBar {
  @Prop title: string
  @Prop isLand: boolean
  @State selectedIndex: number = 0
  @StorageLink('deviceLists') deviceLists: Array<deviceManager.DeviceInfo> = []
  private startAbilityCallBack: (key) => void
  private remoteDeviceModel: RemoteDeviceModel = null
  private dialogController: CustomDialogController = null

  aboutToAppear() {
    AppStorage.SetOrCreate('deviceLists', this.deviceLists)
  }

  clearSelectState() {
    this.deviceLists = []
    if (this.dialogController) {
      this.dialogController.close()
      this.dialogController = null
    }
  }

  selectDevice() {
    if (this.remoteDeviceModel === null || this.remoteDeviceModel.discoverLists.length <= 0 || this.remoteDeviceModel.deviceLists.includes(this.deviceLists[this.selectedIndex])) {
      this.startAbility(this.deviceLists[this.selectedIndex].deviceId)
      this.clearSelectState()
      return
    }
    this.remoteDeviceModel.authenticateDevice(this.deviceLists[this.selectedIndex], (device: deviceManager.DeviceInfo) => {
      this.startAbility(device.deviceId)
    })
    this.clearSelectState()
  }

  selectedIndexChange = (index: number) => {
    this.selectedIndex = index
    if (this.selectedIndex === 0) {
      this.startAbilityCallBack(EXIT)
      globalThis.isDistributed = false
      if (this.dialogController !== null) {
        this.dialogController.close()
      }
      this.deviceLists = []
      return
    }
    this.selectDevice()
  }

  async startAbility(deviceId) {
    await globalThis.context.startAbility({
      bundleName: 'com.unionman.distributedvideoplayer',
      abilityName: 'EntryAbility',
      deviceId: deviceId,
      parameters: {
        isStage: 'Stage',
        deviceID: "remote"
      }
    }).then((data) => {
      promptAction.showToast({ message: "流转成功" })
      globalThis.isDistributed = true
      // 等待对端KvStore启动再同步数据
      setTimeout(() => {
        this.startAbilityCallBack(DATA_CHANGE)
      }, 2000)
    }).catch((err) => {
    });
  }

  showDialog() {
    this.remoteDeviceModel.registerDeviceListCallback(() => {
      this.deviceLists = []
      this.deviceLists.push({
        deviceId: '0',
        deviceName: '本机(结束协同)',
        deviceType: deviceManager.DeviceType.UNKNOWN_TYPE,
        networkId: '',
        range: ONE_THOUSAND
      })
      let deviceTempList = this.remoteDeviceModel.deviceLists.concat(this.remoteDeviceModel.discoverLists)
      for (let i = 0; i < deviceTempList.length; i++) {
        this.deviceLists.push(deviceTempList[i])
        AppStorage.Set('deviceLists', this.deviceLists)
      }
      if (this.dialogController !== null) {
        this.dialogController.close()
        this.dialogController = null
      }
      this.dialogController = new CustomDialogController({
        builder: DeviceDialog({
          cancel: () => {
            this.clearSelectState()
          },
          selectedIndex: this.selectedIndex,
          selectedIndexChange: this.selectedIndexChange
        }),
        cancel: () => {
          this.clearSelectState()
        },
        autoCancel: true,
        customStyle: true,
        alignment: this.isLand ? DialogAlignment.Center : DialogAlignment.Bottom
      })
      this.dialogController.open()
    })
  }

  build() {
    Row() {
      Image($r("app.media.ic_back"))
        .height('60%')
        .margin({ right: 4 })
        .width(this.isLand ? '6%' : '8%')
        .objectFit(ImageFit.Contain)
        .onClick(() => {
          globalThis.context.terminateSelf()
        })

      Text(this.title)
        .fontColor(Color.White)
        .fontSize(22)
        .maxLines(1)

      Blank()

      Image($r("app.media.ic_hop"))
        .height('60%')
        .width(this.isLand ? '6%' : '8%')
        .margin({ right: 8 })
        .objectFit(ImageFit.Contain)
        .onClick(() => {
          this.showDialog()
        })
    }
    .width('100%')
    .height(this.isLand ? '10%' : '8%')
    .constraintSize({ minHeight: 50 })
    .alignItems(VerticalAlign.Center)
    .padding({ left: 10, right: 10 })
    .linearGradient({
      direction: GradientDirection.Bottom,
      colors: [['#CC000000', 0.0], ['#33000000', 0.66], ['#00000000', 0.99]]
    })
  }
}
