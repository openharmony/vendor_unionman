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
import itemCategory from '../Viewmodel/itemCategory';
import router from '@ohos.router';
import promptAction from '@ohos.promptAction';
import deviceManager from '@ohos.distributedHardware.deviceManager';
import common from '@ohos.app.ability.common';
import { BusinessError } from '@ohos.base';
import Logger from '../model/Logger';
import MediaUtils from '../utils/MediaUtils';
import { DeviceDialog } from '../common/DeviceDialog';
import { RemoteDeviceModel } from '../model/RemoteDeviceModel';
import { GlobalThis } from '../utils/globalThis';
const TAG = 'TitleBar';
const DATA_CHANGE = 'dataChange';
const EXIT = 'exit';
const ONE_THOUSAND: number = 1000;
@Entry
@Component
struct videolist {
  title: string
  isLand: boolean
  @State selectedIndex: number = 0
  @StorageLink('deviceLists') deviceLists: Array<deviceManager.DeviceInfo> = []
  @State screenshotUrl: PixelMap | null = null
  private startAbilityCallBack: (key: string) => void = () => {
  }
  private remoteDeviceModel: RemoteDeviceModel = new RemoteDeviceModel()
  private dialogController: CustomDialogController | null = null
  private mediaUtil: MediaUtils = new MediaUtils()
  @State itemCategories: itemCategory[] = [
    new itemCategory(1, '视频1', $rawfile('cloud.mp4'), $rawfile('Cloud.jpg')),
    new itemCategory(2, '视频2', $rawfile('desert.mp4'), $rawfile('desert.jpg')),
    new itemCategory(3, '视频3', $rawfile('flower.mp4'), $rawfile('flowers.jpg')),
    new itemCategory(4, '视频4', $rawfile('forest.mp4'), $rawfile('forest.jpg')),
    new itemCategory(5, '视频5', $rawfile('duck.mp4'), $rawfile('duck.png')),
  ];
  @State index: number = 2
  @State space: number = 8
  @State arrowPosition: BarPosition = BarPosition.End
  @State currentProgressRate: PlaybackSpeed = PlaybackSpeed.Speed_Forward_1_00_X; //视频播放倍速
  @State isAutoPlay: boolean = false; //是否自动播放
  @State showControls: boolean = true; //显示视频控件
  @State muted: boolean = false; //是否静音
  @State autoPlay: boolean = false; //是否自动播放
  @State currentProgressRates: PlaybackSpeed[] = this.itemCategories.map(() => PlaybackSpeed.Speed_Forward_1_00_X);
  aboutToAppear() {
    AppStorage.SetOrCreate('deviceLists', this.deviceLists)
  }
  clearSelectState() {
    this.deviceLists = []
    if (this.dialogController) {
      this.dialogController.close()
    }
  }
  selectDevice() {
    Logger.info(TAG, `start ability device:${JSON.stringify(this.deviceLists[this.selectedIndex])}`)
    if (this.remoteDeviceModel === null ||
      this.remoteDeviceModel.discoverLists.length <= 0 || this.remoteDeviceModel.deviceLists.includes
    (this.deviceLists[this.selectedIndex])) {
      this.startAbility(this.deviceLists[this.selectedIndex].deviceId)
      this.clearSelectState()
      return
    }
    Logger.info(TAG, 'start ability, needAuth')
    this.remoteDeviceModel.authenticateDevice
    (this.deviceLists[this.selectedIndex], (device: deviceManager.DeviceInfo) => {
      Logger.info(TAG, 'auth and online finished' + JSON.stringify(device))
      this.startAbility(device.deviceId)
    })
    Logger.info(TAG, 'start ability2 ......')
    this.clearSelectState()
  }

  selectedIndexChange = (index: number) => {
    Logger.info(TAG, 'selectedIndexChange')
    this.selectedIndex = index
    if (this.selectedIndex === 0) {
      Logger.info(TAG, 'stop ability')
      this.startAbilityCallBack(EXIT)
      GlobalThis.getInstance().setObject('isDistributed', false)
      if (this.dialogController !== null) {
        this.dialogController.close()
      }
      this.deviceLists = []
      return
    }
    this.selectDevice()
  }
  async startAbility(deviceId: string | undefined) {
    Logger.info(TAG, `startAbility deviceId: ${deviceId}`)
    let context = GlobalThis.getInstance().getObject('context') as common.UIAbilityContext
    await context.startAbility({
      bundleName: 'com.unionman.distributedvideoplayer',
      abilityName: 'EntryAbility',
      deviceId: deviceId,
      parameters: {
        isStage: 'Stage',
        deviceID: 'remote'
      }
    }).then((data) => {
      Logger.info(TAG, `start ability finished: ${JSON.stringify(data)}`)
      promptAction.showToast({ message: '流转成功' })
      GlobalThis.getInstance().setObject('isDistributed', true)
      setTimeout(() => {
        this.startAbilityCallBack(DATA_CHANGE)
      }, 2000)
    }).catch((err: BusinessError) => {
      Logger.error(TAG, `start ability err: ${JSON.stringify(err)}`);
    });
  }
  showDialog() {
    this.remoteDeviceModel.registerDeviceListCallback(() => {
      Logger.info(TAG, 'registerDeviceListCallback, callback entered')
      this.deviceLists = []
      this.deviceLists.push({
        deviceId: '0',
        deviceName: '本机(结束协同)',
        deviceType: deviceManager.DeviceType.UNKNOWN_TYPE,
        networkId: '',
        range: ONE_THOUSAND,
        authForm: -1
      })
      let deviceTempList = this.remoteDeviceModel.deviceLists.concat(this.remoteDeviceModel.discoverLists)
      Logger.info(TAG, `this.remoteDeviceModel.deviceTempList.length${deviceTempList.length}`)
      for (let i = 0; i < deviceTempList.length; i++) {
        Logger.info(TAG, `device ${i}/${deviceTempList.length} deviceId= ${deviceTempList[i].deviceId}, deviceName= ${deviceTempList[i].deviceName}, deviceType= ${deviceTempList[i].deviceType}`)
        this.deviceLists.push(deviceTempList[i])
        AppStorage.Set('deviceLists', this.deviceLists)
        Logger.info(TAG, 'deviceLists push end')
      }
      Logger.info(TAG, 'CustomDialogController start')
      if (this.dialogController !== null) {
        this.dialogController.close()
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
        autoCancel: true, customStyle: true,
        alignment: this.isLand ? DialogAlignment.Center : DialogAlignment.Bottom
      })
      this.dialogController.open()
      Logger.info(TAG, 'CustomDialogController end')
    })
  }

  build() {
    Column() {
      Row() {
        Image($r('app.media.ic_back')).height('6%').margin({ right: 1 })
          .objectFit(ImageFit.Contain)
          .onClick(() => {
            let context = GlobalThis.getInstance().getObject('context') as common.UIAbilityContext
            context.terminateSelf()
          })
        Blank()
        Image($r('app.media.ic_hop')).id('hop').height('6%').margin({ right: 8 })
          .objectFit(ImageFit.Contain)
          .onClick(() => {this.showDialog()})
        Image($r('app.media.ic_settings')).id('settings').height('6%').objectFit(ImageFit.Contain)
          .onClick(() => {
            router.pushUrl({
              url: 'pages/Settings'
            })
          })
      }
      .width('100%').constraintSize({ minHeight: 50 })
      .alignItems(VerticalAlign.Center).padding({ left: 10, right: 10 })
      .linearGradient({
        direction: GradientDirection.Bottom,
       colors: [['#CC000000', 0.0], ['#CC000000', 0.66], ['#CC000000', 0.99]]
      })
      Grid() {
        ForEach(this.itemCategories, (item: itemCategory, index: number) => {
          GridItem() {
            Column() {
              Video({
                previewUri: item.image,
                src: item.video,
                currentProgressRate: this.currentProgressRates[index],
              })
                .width('100%').height('90%')
                .backgroundBlurStyle(BlurStyle.Thin, {
                  colorMode: ThemeColorMode.LIGHT,
                  adaptiveColor: AdaptiveColor.DEFAULT,
                  scale: 1.0
                }).borderRadius(18).padding(10).objectFit(ImageFit.Contain)
                .autoPlay(this.isAutoPlay).controls(this.showControls)
                .onStart(() => {console.info('onStart')}).onPause(() => {
                  console.info('onPause')
              })
                .onFinish(() => {
                  console.info('onFinish')
                }).onError(() => {
                  console.info('播放错误')
              })
                .onPrepared((e) => {
                  console.info('onPrepared is ' + e.duration)
                }
                )
                .onSeeking((e) => {
                  console.info('onSeeking is ' + e.time)
                })
                .onSeeked((e) => {
                  console.info('onSeeked is ' + e.time)
                })
                .onUpdate((e) => {
                  console.info('onUpdate is ' + e.time)
                })
              Row() {
                Select([
                  { value: '0.75x' },
                  { value: '1x' },
                  { value: '2x' }
                ])
                  .font({ size: 16, weight: 500 }).fontColor('#182431')
                  .selectedOptionFont({ size: 16, weight: 400 })
                  .optionFont({ size: 16, weight: 400 }).space(this.space)
                  .selected(this.currentProgressRates[index] === PlaybackSpeed.Speed_Forward_0_75_X ? 0 :
                    this.currentProgressRates[index] === PlaybackSpeed.Speed_Forward_1_00_X ? 1 :
                      this.currentProgressRates[index] === PlaybackSpeed.Speed_Forward_2_00_X ? 2 : 1) // 默认选中1x
                  .onSelect((selectedIndex: number, value?: string) => {
                    console.info('Select:' + selectedIndex);
                    if (value) {
                      let playbackSpeed: PlaybackSpeed;
                      switch (value) {
                        case '0.75x': playbackSpeed = PlaybackSpeed.Speed_Forward_0_75_X; break;
                        case '1x': playbackSpeed = PlaybackSpeed.Speed_Forward_1_00_X; break;
                        case '2x': playbackSpeed = PlaybackSpeed.Speed_Forward_2_00_X; break;
                        default: playbackSpeed = PlaybackSpeed.Speed_Forward_1_00_X; // 默认速度
                      }
                      this.currentProgressRates[index] = playbackSpeed;
                    }
                  })
              }.width('100%')
            }.width('100%').height('100%').borderRadius(18).padding(12)
          }.height('83%').width('100%')
        })
      } .columnsTemplate('1fr 1fr').columnsGap(10).rowsGap(10).width('100%')
      .height('100%').backgroundImage($r('app.media.bj'))
      .backgroundImageSize(ImageSize.Cover)
    }.width('100%').height('100%')
  }
}