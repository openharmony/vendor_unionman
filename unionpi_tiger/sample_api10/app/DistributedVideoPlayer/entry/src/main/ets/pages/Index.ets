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

import mediaQuery from '@ohos.mediaquery';
import preferences from '@ohos.data.preferences';
import common from '@ohos.app.ability.common';
import { BusinessError } from '@ohos.base';
import MediaDataSource from '../common/BasicDataSource';
import { TitleBar } from '../common/TitleBar';
import { toTime } from '../utils/utils';
import { RemoteDeviceModel } from '../model/RemoteDeviceModel';
import { KvStoreModel } from '../model/KvStoreModel';
import Logger from '../model/Logger';
import MediaUtils from '../utils/MediaUtils';
import AVPlayerUtils from '../utils/AVPlayerUtils';
import DistributedDataModel from '../model/DistributedDataModel';
import { GlobalThis } from '../utils/globalThis';
import photoAccessHelper from '@ohos.file.photoAccessHelper';
import image from '@ohos.multimedia.image';
import dataSharePredicates from '@ohos.data.dataSharePredicates';

const TAG: string = "Index";
const EXIT: string = 'exit';
const DATA_CHANGE: string = 'dataChange';
const PlaybackSpeed: string[] = ['0.75X', '1.0X', '1.25X', '1.75X', '2.0X'];
const PREFERENCES_NAME = 'setting';
let preferenceSetting: preferences.Preferences | null = null;

@Component
struct localVideoItem {
  private media: photoAccessHelper.PhotoAsset = {} as photoAccessHelper.PhotoAsset
  @State pixelMap: PixelMap | null = null
  @Link videoIndex: number
  @State index: number = 0
  event: (event?: ClickEvent) => void = () => {
  }

  async aboutToAppear() {
    let size: image.Size = { width: 128, height: 80 }
    this.pixelMap = await this.media.getThumbnail(size)
  }

  build() {
    Row({ space: 10 }) {
      Image(this.pixelMap)
        .height(80)
        .aspectRatio(1.6)
        .objectFit(ImageFit.Cover)
        .borderRadius(10)
      Text(this.media.displayName.replace('.mp4', ''))
        .width(172)
        .fontSize(14)
        .fontColor(this.videoIndex === this.index ? '#E5007DFF' : '#FFFFFF')
    }
    .width(330)
    .height(100)
    .padding(10)
    .borderRadius(20)
    .backgroundColor('#464646')
    .alignItems(VerticalAlign.Center)
    .onClick(this.event)
  }
}

@Entry
@Component
struct Index {
  @State tabsIndex: number = 0
  @State @Watch('resetVideo') videoIndex: number = 0
  @State @Watch('initVideo') title: string = ''
  @State @Watch('setSpeed') SpeedIndex: number = 1
  @State @Watch('dataChange') isPlay: boolean = false
  // 是否显示控制栏
  @State @Watch('showControlBarEvent') controls: number = Visibility.Visible;
  // 是否显示侧边栏
  @State showSideBar: boolean = false
  @State showPlaybackProgress: boolean = false
  @State currentTime: number = 0
  @State duration: number = 0
  @State kvStoreModel: KvStoreModel = new KvStoreModel()
  @State ratio: number = 1.0
  // 是否横屏
  @State isLand: boolean = false
  // 是否正在加载
  @State isLoading: boolean = true;
  // 流转模式
  @StorageLink('continuationMode') continuationMode: preferences.ValueType = 'continuation'
  @StorageLink('loop') @Watch("loopChange") loop: preferences.ValueType = true
  // 控制栏定时器id
  private controlBarTimeoutID: number = -1
  private tabsController: TabsController = new TabsController()
  private remoteDeviceModel: RemoteDeviceModel = new RemoteDeviceModel()
  private mediaDataSource: MediaDataSource = new MediaDataSource([])
  private mediaUtil: MediaUtils = new MediaUtils()
  private mediaList: Array<photoAccessHelper.PhotoAsset> = []
  private mXComponentController: XComponentController = new XComponentController()
  private surfaceId: string = ''
  private avPlayer: AVPlayerUtils = new AVPlayerUtils()
  private fileAsset: photoAccessHelper.PhotoAsset = {} as photoAccessHelper.PhotoAsset
  private fd: number = -1
  private listener = mediaQuery.matchMediaSync('screen and (min-aspect-ratio: 1.5) or (orientation: landscape)')
  onLand = (mediaQueryResult: mediaQuery.MediaQueryResult) => {
    Logger.info(TAG, `onLand: mediaQueryResult.matches= ${mediaQueryResult.matches}`)
    if (mediaQueryResult.matches) {
      this.isLand = true
    } else {
      this.isLand = false
    }
  }
  startX: number = 0
  startY: number = 0
  moveX: number = 0
  moveY: number = 0

  @Builder
  TabBuilder(index: number, text: string) {
    Column() {
      Text(text)
        .fontColor(Color.White)
        .fontSize(24)
        .fontFamily('HarmonyHeiTi')

      Divider()
        .width('75%')
        .strokeWidth(5)
        .color('#007Dff')
        .opacity(this.tabsIndex === index ? 1 : 0)
    }
  }

  async aboutToAppear() {
    this.getVideos()
    // 订阅DATA_CHANGE类型的数据变更通知
    this.kvStoreModel.setOnMessageReceivedListener(DATA_CHANGE, (value: string) => {
      Logger.info(TAG, `DATA_CHANGE ${value}`)
      if (GlobalThis.getInstance().getObject("isDistributed")) {
        if (value.search(EXIT) !== -1) {
          Logger.info(TAG, `EXIT ${EXIT}`)
          let context = GlobalThis.getInstance().getObject("context") as common.UIAbilityContext
          context.terminateSelf((error: BusinessError) => {
            Logger.error(TAG, `terminateSelf finished, error= ${error}`)
          });
        } else {
          let video: DistributedDataModel = JSON.parse(value)
          GlobalThis.getInstance().setObject("dataChange", true)
          GlobalThis.getInstance().setObject("firstHop", video.firstHop)
          this.title = video.title
          this.currentTime = video.currentTime
          if (!video.firstHop) {
            this.setCurrentTime()
          }
          this.SpeedIndex = video.SpeedIndex
          this.isPlay = video.isPlay
          Logger.info(TAG, `title:${this.title},currentTime:${this.currentTime},SpeedIndex:${this.SpeedIndex},isPlay:${this.isPlay},firstHop:${GlobalThis.getInstance()
            .getObject("firstHop")}`)
          GlobalThis.getInstance().setObject("dataChange", false)
        }
      }
    })
    if (AppStorage.Get('isStage') === 'Stage') {
      GlobalThis.getInstance().setObject("isDistributed", true)
    }
    GlobalThis.getInstance().setObject("deviceID", AppStorage.Get('deviceID'))
    this.listener.on('change', this.onLand)
    let context = GlobalThis.getInstance().getObject("context") as common.UIAbilityContext
    preferenceSetting = await preferences.getPreferences(context, PREFERENCES_NAME)
    this.continuationMode = await preferenceSetting.get('continuationMode', 'continuation')
    this.loop = await preferenceSetting.get('loop', true)
  }

  startAbilityCallBack = (key: string) => {
    Logger.info(TAG, `startAbilityCallBack ${key}`)
    if (DATA_CHANGE === key) {
      GlobalThis.getInstance().setObject("deviceID", "local")
      GlobalThis.getInstance().setObject("firstHop", true)
      let video = new DistributedDataModel(this.title, this.currentTime, this.SpeedIndex, this.isPlay, true)
      this.isPlay = false
      this.kvStoreModel.put(DATA_CHANGE, JSON.stringify(video))
      if (this.continuationMode == 'continuation') {
        let context = GlobalThis.getInstance().getObject("context") as common.UIAbilityContext
        // 等待对端获取到数据再退出
        setTimeout(() => {
          context.terminateSelf()
        }, 1500)
      }
    }
    if (EXIT === key) {
      this.kvStoreModel.put(DATA_CHANGE, EXIT)
    }
  }

  async getVideos() {
    let fileList = await this.mediaUtil.getFileAssetsFromType(photoAccessHelper.PhotoType.VIDEO)
    Logger.info(TAG, 'getVideos fileList:' + JSON.stringify(fileList))
    this.mediaList = fileList
    for (let i = 0; i < fileList.length; i++) {
      this.mediaDataSource.pushData(fileList[i])
    }
    Logger.info(TAG, 'getVideos mediaList:' + JSON.stringify(this.mediaList))
    Logger.info(TAG, 'getVideos mediaDataSource:' + JSON.stringify(this.mediaDataSource))
    this.mediaDataSource.notifyDataReload()
    if (!GlobalThis.getInstance().getObject("isDistributed")) {
      this.title = this.mediaList[this.videoIndex].displayName.replace('.mp4', '')
    }
  }

  dataChange() {
    Logger.info(TAG, `dataChange, title = ${this.title}, currentTime = ${this.currentTime}, SpeedIndex = ${this.SpeedIndex}, isPlay = ${this.isPlay}`)
    if (this.isPlay) {
      this.playVideo()
    } else {
      this.avPlayer.pause()
      this.controls = Visibility.Visible
      clearTimeout(this.controlBarTimeoutID)
    }
    this.controls = Visibility.Visible
    this.distributedDataSync()
  }

  distributedDataSync() {
    if (GlobalThis.getInstance().getObject("isDistributed") && !GlobalThis.getInstance()
      .getObject("firstHop") && !GlobalThis.getInstance().getObject("dataChange")) {
      let video = new DistributedDataModel(this.title, this.currentTime, this.SpeedIndex, this.isPlay, false)
      this.kvStoreModel.put(DATA_CHANGE, JSON.stringify(video))
    }
  }

  showControlBarEvent() {
    if (this.controls == Visibility.Visible) {
      this.controlBarTimeoutID = setTimeout(() => {
        this.controls = Visibility.Hidden
      }, 5000)
    }
  }

  async initVideo() {
    Logger.info(TAG, 'initVideo')
    try {
      this.mediaList.forEach((file, index) => {
        if (file.displayName.replace('.mp4', '') === this.title) {
          this.fileAsset = file
          this.videoIndex = index
        }
      })
      let fdPath = await this.prepareVideo()
      await this.avPlayer.initVideoPlayer(fdPath, this.surfaceId)
      // 等待avPlayer初始化完成
      setTimeout(() => {
        if (GlobalThis.getInstance().getObject("firstHop") && GlobalThis.getInstance().getObject("isDistributed")) {
          this.setCurrentTime()
        }
        if (GlobalThis.getInstance().getObject("firstHop") && GlobalThis.getInstance()
          .getObject("deviceID") === "remote") {
          GlobalThis.getInstance().setObject("firstHop", false)
          this.dataChange()
        }
      }, 200)
    } catch (error) {
      Logger.info(TAG, `initVideo error ${JSON.stringify(error)}`)
    }
  }

  async getPhotoValue(key: photoAccessHelper.PhotoKeys) {
    let context = GlobalThis.getInstance().getObject("context") as common.UIAbilityContext;
    let phAccessHelper = photoAccessHelper.getPhotoAccessHelper(context);

    let predicates: dataSharePredicates.DataSharePredicates = new dataSharePredicates.DataSharePredicates();
    predicates.equalTo("title", this.title)
    let fetchOption: photoAccessHelper.FetchOptions = {
      fetchColumns: [key],
      predicates: predicates
    };
    let fetchResult: photoAccessHelper.FetchResult<photoAccessHelper.PhotoAsset> = await phAccessHelper.getAssets(fetchOption);
    let photoAsset: photoAccessHelper.PhotoAsset = await fetchResult.getFirstObject();

    Logger.info(TAG, 'fetchResult success' + JSON.stringify(photoAsset));
    let photoKeys: photoAccessHelper.PhotoKeys = key;
    let photoValue: photoAccessHelper.MemberType = photoAsset.get(photoKeys);

    Logger.info(TAG, 'fetchResult success' + JSON.stringify(key) + JSON.stringify(photoValue));
    return photoValue
  }

  async prepareVideo() {
    Logger.info(TAG, 'prepareVideo')
    try {
      let value = await this.getPhotoValue(photoAccessHelper.PhotoKeys.WIDTH)
      let photoAssetWidth = Number(value)

      value = await this.getPhotoValue(photoAccessHelper.PhotoKeys.HEIGHT)
      let photoAssetHeight = Number(value)

      value = await this.getPhotoValue(photoAccessHelper.PhotoKeys.DURATION)
      let photoAssetDuration = Number(value)

      let photoAsset = this.fileAsset
      this.fd = await photoAsset.open('Rw')
      this.ratio = photoAssetWidth / photoAssetHeight
      this.mXComponentController.setXComponentSurfaceSize({
        surfaceWidth: photoAssetWidth,
        surfaceHeight: photoAssetHeight
      })
      this.surfaceId = this.mXComponentController.getXComponentSurfaceId()
      this.isLoading = false
      this.duration = photoAssetDuration / 1000
    } catch (err) {
      console.error(`prepareVideo release failed. error: ${err.code}, ${err.message}`);
    }
    return 'fd://' + this.fd
  }

  async resetVideo() {
    Logger.info(TAG, 'resetVideo')
    if (GlobalThis.getInstance().getObject("firstHop")) {
      return
    }
    this.SpeedIndex = 1
    this.currentTime = 0
    this.isLoading = true
    await this.fileAsset.close(this.fd)
    this.fileAsset = this.mediaList[this.videoIndex]
    this.title = this.fileAsset.displayName.replace('.mp4', '')
    this.isPlay = false
    this.distributedDataSync()
    let fdPath = await this.prepareVideo()
    await this.avPlayer.reset(fdPath)
  }

  async playVideo() {
    Logger.info(TAG, 'playVideo')
    if (GlobalThis.getInstance().getObject("firstHop")) {
      return
    }
    this.avPlayer.setTimeUpdateCallBackCallBack((time: number) => {
      this.currentTime = time / 1000
    })
    this.avPlayer.play()
    this.isPlay = true
    this.showControlBarEvent()
  }

  aboutToDisappear() {
    this.clearVideoPlayer()
  }

  clearVideoPlayer() {
    Logger.info(TAG, 'clearVideoPlayer')
    if (this.avPlayer) {
      this.avPlayer.stop()
      this.avPlayer.release()
    }
    if (this.fileAsset) {
      this.fileAsset.close(this.fd)
    }
  }

  setCurrentTime() {
    Logger.info(TAG, 'setCurrentTime')
    this.avPlayer.seek(this.currentTime)
    this.isLoading = false
    this.distributedDataSync()
  }

  setSpeed() {
    Logger.info(TAG, 'setSpeed')
    this.avPlayer.setSpeed(this.SpeedIndex)
    this.distributedDataSync()
  }

  loopChange() {
    this.avPlayer.setLoop(this.loop as boolean)
  }

  build() {
    SideBarContainer(SideBarContainerType.Overlay) {
      Tabs({ controller: this.tabsController }) {
        TabContent() {
          Scroll() {
            Column({ space: 10 }) {
              LazyForEach(this.mediaDataSource, (item: photoAccessHelper.PhotoAsset, index: number) => {
                localVideoItem({ media: item, videoIndex: $videoIndex, index: index, event: () => {
                  this.videoIndex = index
                  this.title = item.displayName.replace('.mp4', '')
                } })
              })
            }
            .constraintSize({ minHeight: '100%' })
          }
          .edgeEffect(EdgeEffect.Spring)
          .padding(10)
        }.tabBar(this.TabBuilder(0, '本地视频'))
      }
      .backgroundColor('#7F000000')
      .vertical(false)
      .scrollable(false)

      Stack() {
        XComponent({
          id: 'xComponent',
          type: 'surface',
          controller: this.mXComponentController
        })
          .width('100%')
          .aspectRatio(this.ratio)

        // 控制栏
        Column() {
          TitleBar({
            title: this.title,
            isLand: this.isLand,
            startAbilityCallBack: this.startAbilityCallBack,
            remoteDeviceModel: this.remoteDeviceModel,
          })

          Flex({ direction: FlexDirection.Row, justifyContent: FlexAlign.SpaceEvenly, alignItems: ItemAlign.Center }) {
            Image(this.isPlay ? $r("app.media.ic_pause") : $r("app.media.ic_play"))
              .id("playBtn")
              .width(36)
              .height(36)
              .flexShrink(0)
              .margin({ right: 10 })
              .onClick(() => {
                this.isPlay = !this.isPlay
              })
            Text(toTime(this.currentTime))
              .flexShrink(0)
              .fontColor(Color.White)
              .maxLines(1)
              .textOverflow({ overflow: TextOverflow.None })
            Slider({
              value: this.currentTime,
              min: 0,
              max: this.duration,
              step: 1,
            })
              .id("slider")
              .blockColor(Color.White)
              .trackColor(Color.Gray)
              .selectedColor(Color.White)
              .onChange((value: number, mode: SliderChangeMode) => {
                this.currentTime = value
                this.setCurrentTime()
              })
            Text(toTime(this.duration))
              .flexShrink(0)
              .fontColor(Color.White)
              .maxLines(1)
              .textOverflow({ overflow: TextOverflow.None })
            Select([{ value: PlaybackSpeed[0] },
              { value: PlaybackSpeed[1] },
              { value: PlaybackSpeed[2] },
              { value: PlaybackSpeed[3] },
              { value: PlaybackSpeed[4] }])
              .id("playbackSpeed")
              .flexShrink(0)
              .backgroundColor('#00000000')
              .margin({ left: 10, right: 10 })
              .selected(this.SpeedIndex)
              .value(PlaybackSpeed[this.SpeedIndex])
              .font({ size: 24 })
              .fontColor("#E5007DFF")
              .selectedOptionFont({ size: 16 })
              .selectedOptionFontColor("#E5007DFF")
              .optionFont({ size: 16 })
              .optionFontColor("#7F007DFF")
              .onSelect((index: number, value: string) => {
                Logger.info(TAG, `index ${index}`)
                this.SpeedIndex = index
              })

            Image($r("app.media.ic_public_view_list"))
              .id("viewList")
              .width(36)
              .aspectRatio(1)
              .flexShrink(0)
              .margin({ right: 10 })
              .onClick(() => {
                this.showSideBar = !this.showSideBar
              })
          }
          .width('100%')
          .padding({ left: 10, right: 10 })
          .linearGradient({
            direction: GradientDirection.Top,
            colors: [['#CC000000', 0.0], ['#33000000', 0.66], ['#00000000', 0.99]]
          })
        }
        .width('100%')
        .height('100%')
        .justifyContent(FlexAlign.SpaceBetween)
        .alignItems(HorizontalAlign.Start)
        .visibility(this.controls)

        if (this.showPlaybackProgress) {
          Text(toTime(this.currentTime) + '/' + toTime(this.duration))
            .fontColor(Color.White)
            .padding(12)
            .backgroundColor('#CC000000')
            .borderRadius(8)
        }

        if (this.isLoading) {
          Column() {
            LoadingProgress()
              .height(96)
              .width(96)
              .color(Color.Grey)

            Text("正在加载...")
              .fontColor(Color.White)
              .fontFamily('HarmonyHeiTi')
              .fontSize(14)
          }
        }
      }
      .id("container")
      .width('100%')
      .height('100%')
      .backgroundColor(Color.Black)
      .onClick(() => {
        if (!this.showSideBar) {
          this.controls = (this.controls === Visibility.Hidden) ? Visibility.Visible : Visibility.Hidden
        } else {
          this.showSideBar = false
        }
      })
      .priorityGesture(
        GestureGroup(GestureMode.Exclusive,
          TapGesture({ count: 2 })
            .onAction(() => {
              this.isPlay = !this.isPlay
            }),
          PanGesture({ direction: PanDirection.Left | PanDirection.Right })
            .onActionStart((event: GestureEvent) => {
              Logger.info(TAG, 'PlaybackEvent Pan start')
              this.startX = event.offsetX
            })
            .onActionUpdate((event: GestureEvent) => {
              Logger.info(TAG, 'PlaybackEvent Pan update')
              // 右滑
              if (event.offsetX - this.startX > 0) {
                this.startX = event.offsetX
                this.showPlaybackProgress = true
                if (this.currentTime === this.duration) {
                  return
                } else {
                  this.currentTime += 1
                }
              } // 左滑
              else if (event.offsetX - this.startX < 0) {
                this.startX = event.offsetX
                this.showPlaybackProgress = true
                if (this.currentTime === 0) {
                  return
                } else {
                  this.currentTime -= 1
                }
              }
            })
            .onActionEnd(() => {
              Logger.info(TAG, 'PlaybackEvent Pan end')
              this.showPlaybackProgress = false
              this.isLoading = true
              this.setCurrentTime()
            })
        )
      )
    }
    .showSideBar(this.showSideBar)
    .showControlButton(false)
    .sideBarPosition(SideBarPosition.End)
    .sideBarWidth(350)
    .minSideBarWidth(200)
    .maxSideBarWidth(400)
    .onChange((value: boolean) => {
      Logger.info('status:' + value)
    })
  }
}