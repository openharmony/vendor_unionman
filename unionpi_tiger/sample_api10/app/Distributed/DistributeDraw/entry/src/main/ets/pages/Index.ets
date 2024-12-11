/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
 */

import common from '@ohos.app.ability.common';
import deviceManager from '@ohos.distributedHardware.deviceManager';
import CommonConstants from '../common/constants/CommonConstants';
import Position from '../viewmodel/Position';
import remoteDeviceModel from '../common/utils/RemoteDeviceUtil';
import DeviceListDialogComponent from '../view/CustomDialogComponent';
import Logger from '../common/utils/Logger';
import DistributedObjectModel from '../viewmodel/DistributedObjectModel';
import MediaUtils from '../common/utils/MediaUtils';

let storage = LocalStorage.getShared();

@Entry(storage)
@Component
struct Index {
  @StorageLink('deviceList') deviceList: deviceManager.DeviceInfo[] = [];
  private sessionId: string = '';
  @StorageLink('sessionId') globalSessionId: string = '';
  @StorageLink('objectModel') globalObject: DistributedObjectModel = new DistributedObjectModel();
  @State isOnline: boolean = false;
  @LocalStorageProp('positionList') positionList: Position[] = [];
  @LocalStorageProp('updateCanvas') @Watch('updateCanvas') update: boolean = false;
  private settings: RenderingContextSettings = new RenderingContextSettings(true);
  private canvasContext: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings);
  @LocalStorageProp('strokeStyle') strokeStyle: string = '#000000';
  // private kvStoreModel: KvStoreModel = new KvStoreModel();
  private context: common.UIAbilityContext = getContext(this) as common.UIAbilityContext;
  private mediaUtil: MediaUtils = new MediaUtils(this.context)
  private dialogController: CustomDialogController = new CustomDialogController({
    builder: DeviceListDialogComponent({
      startAbility: this.startAbilityContinuation,
      share: this.share,
      canvasContext: this.canvasContext,
      deviceList: this.deviceList,
      sessionId: this.sessionId,
      positionList: this.positionList,
      strokeStyle: this.strokeStyle,
      cancel: this.onCancel
    }),
    alignment: DialogAlignment.Bottom,
    cancel: this.onCancel,
    autoCancel: true,
    offset: {
      dx: $r('app.float.dialog_offset_dx'),
      dy: $r('app.float.dialog_offset_dy')
    }
  });

  aboutToAppear() {
    Logger.info('Index', 'aboutToAppear begin');
    // this.createKVStore();
  }

  aboutToDisappear() {
    // this.kvStoreModel.removeDataChangeListener();
  }

  /**
   * Callback for closing a pop-up window.
   */
  onCancel(): void {
    remoteDeviceModel.stopDeviceDiscovery();
  }

  /**
   * Update canvas.
   */
  updateCanvas(): void {
    this.redraw();
  }

  @Builder
  CanvasColorsMenu() {
    Flex({
      direction: FlexDirection.Column,
      wrap: FlexWrap.Wrap,
      justifyContent: FlexAlign.SpaceEvenly,
      alignItems: ItemAlign.Center,
      alignContent: FlexAlign.Start
    }) {
      ForEach(CommonConstants.CANVAS_COLORS, (item: string, index: number) => {
        Button({ type: ButtonType.Circle })
          .width($r('app.float.ic_toolbar_btn_width'))
          .height($r('app.float.ic_toolbar_btn_height'))
          .border(item === this.strokeStyle ? {
            width: 6,
            color: item,
            style: BorderStyle.Solid
          } : {})
          .backgroundColor(item === this.strokeStyle ? (item === "#FFFFFF" ? Color.Black : Color.White) : item)
          .margin(3)
          .onClick(() => {
            this.strokeStyle = item
            this.globalObject.update(this.positionList, false, this.strokeStyle)
          })
      })
    }
    .constraintSize({
      maxHeight: 200
    })
  }

  build() {
    Stack({ alignContent: Alignment.Top }) {
      Row() {
        Canvas(this.canvasContext)
          .width(CommonConstants.FULL_PERCENT)
          .height(CommonConstants.FULL_PERCENT)
          .backgroundColor($r('app.color.start_window_background'))
          .onReady(() => {
            this.redraw();
          })
      }
      .onTouch((event: TouchEvent) => {
        this.onTouchEvent(event);
      })
      .width(CommonConstants.FULL_PERCENT)
      .layoutWeight(CommonConstants.NUMBER_ONE)

      Row() {
        Button({ type: ButtonType.Circle }) {
          Image($r('app.media.chevron_left'))
            .width(18)
            .height(28)
            .margin({ bottom: 8, left: 9 })
            .fillColor(Color.White)
        }
        .width(40)
        .height(40)
        .backgroundColor("#B2000000")
        .margin({ left: CommonConstants.ICON_MARGIN_LEFT })
        .align(Alignment.Start)
        .onClick(() => {
          this.context.terminateSelf()
        })

        Blank()

        Row() {
          Button({ type: ButtonType.Circle }) {
            Image($r('app.media.undo'))
              .width($r('app.float.ic_toolbar_btn_width'))
              .height($r('app.float.ic_toolbar_btn_height'))
              .fillColor(Color.White)
          }
          .width(40)
          .height(40)
          .backgroundColor(Color.Transparent)
          .onClick(() => {
            this.goBack();
          })

          Button({ type: ButtonType.Circle }) {
            Image($r('app.media.clean'))
              .width($r('app.float.ic_toolbar_btn_width'))
              .height($r('app.float.ic_toolbar_btn_height'))
              .fillColor(Color.White)
          }
          .width(40)
          .height(40)
          .backgroundColor(Color.Transparent)
          .onClick(() => {
            this.positionList = []
            this.globalObject.clear()
            let height = this.canvasContext.height
            let width = this.canvasContext.width
            this.canvasContext.clearRect(0, 0, width, height)
          })

          Button({ type: ButtonType.Circle }) {
            Image($r('app.media.save'))
              .width($r('app.float.ic_toolbar_btn_width'))
              .height($r('app.float.ic_toolbar_btn_height'))
              .fillColor(Color.White)
          }
          .width(40)
          .height(40)
          .backgroundColor(Color.Transparent)
          .onClick(() => {
            let pixelMap: PixelMap = this.canvasContext.getPixelMap(0, 0, this.canvasContext.width, this.canvasContext.height)
            this.mediaUtil.savePicture(pixelMap)
          })

          Button({ type: ButtonType.Circle }) {
            Row()
              .clip(new Circle({ width: 24, height: 24 }))
              .width($r('app.float.ic_toolbar_btn_width'))
              .height($r('app.float.ic_toolbar_btn_height'))
              .backgroundColor(this.strokeStyle)
          }
          .width(40)
          .height(40)
          .backgroundColor(Color.Transparent)
          .bindMenu(this.CanvasColorsMenu)

          Button({ type: ButtonType.Circle }) {
            Image($r('app.media.share'))
              .width($r('app.float.ic_toolbar_btn_width'))
              .height($r('app.float.ic_toolbar_btn_height'))
              .fillColor(Color.White)
          }
          .width(40)
          .height(40)
          .backgroundColor(Color.Transparent)
          .onClick(() => {
            this.onContinueAbilityClick();
          })
        }
        .width(200)
        .height(40)
        .borderRadius(25)
        .backgroundColor("#B2000000")
        .margin({ right: CommonConstants.ICON_MARGIN_LEFT })
        .justifyContent(FlexAlign.SpaceEvenly)
        .backdropBlur(1)
      }
      .width(CommonConstants.FULL_PERCENT)
      .height(CommonConstants.TITLE_HEIGHT)
    }
    .height(CommonConstants.FULL_PERCENT)
    .width(CommonConstants.FULL_PERCENT)
  }

  /**
   * Create a distributed key-value database.
   */
  // createKVStore(): void {
  //   this.kvStoreModel.createKvStore(this.context, (data: distributedKVStore.ChangeNotification) => {
  //     this.positionList = [];
  //     let entries: distributedKVStore.Entry[] = data.insertEntries.length > 0 ? data.insertEntries : data.updateEntries;
  //     entries.forEach((entry: distributedKVStore.Entry) => {
  //       if (CommonConstants.CHANGE_POSITION === entry.key) {
  //         this.positionList = JSON.parse((entry.value.value) as string);
  //         this.redraw();
  //       }
  //     });
  //   });
  // }

  onPageShow(): void {
    Logger.info('Index', `this.sessionId = ${this.sessionId}`);
    Logger.info('Index', `globalSessionId = ${this.globalSessionId}`);
    if (this.sessionId !== this.globalSessionId) {
      this.sessionId = this.globalSessionId;
      this.share();
    }
    this.redraw();
  }

  share() {
    Logger.info('Index', `sessionId = ${this.sessionId}`);
    this.globalObject.update(this.positionList, false, this.strokeStyle)
    this.globalObject.setChangeCallback(() => {
      let exit: boolean = this.globalObject.distributedObject.exit;
      if (exit) {
        let context = getContext(this) as common.UIAbilityContext
        Logger.info('Index', `EXIT ${exit}`)
        context.terminateSelf((error) => {
          Logger.error('Index', `terminateSelf finished, error= ${error}`)
        })
      }
      this.positionList = this.globalObject.distributedObject.positionList;
      this.strokeStyle = this.globalObject.distributedObject.strokeStyle;
      // this.redraw();
      if (this.canvasContext) {
        this.canvasContext.clearRect(0, 0, this.canvasContext.width, this.canvasContext.height);
        this.positionList.forEach((position) => {
          Logger.info('Index', `redraw position=${JSON.stringify(position)}`);
          if (position.isFirstPosition) {
            this.canvasContext.beginPath();
            this.canvasContext.strokeStyle = position.strokeStyle;
            this.canvasContext.lineWidth = CommonConstants.CANVAS_LINE_WIDTH;
            this.canvasContext.lineJoin = CommonConstants.CANVAS_LINE_JOIN;
            this.canvasContext.moveTo(position.positionX, position.positionY);
            this.canvasContext.lineTo(position.positionX - 1, position.positionY - 1);
            this.canvasContext.stroke();
          } else {
            this.canvasContext.lineTo(position.positionX, position.positionY);
            this.canvasContext.stroke();
          }
        });
      }
    })
    this.globalObject.setStatusCallback((session, networkId, status) => {
      Logger.info('Index', `StatusCallback,${status}`);
      if (status === 'online') {
        this.isOnline = true;
      } else {
        this.isOnline = false;
      }
    })
    this.globalObject.distributedObject.setSessionId(this.sessionId);
    AppStorage.SetOrCreate('objectModel', this.globalObject);
  }

  /**
   * Redraw the track.
   */
  redraw(): void {
    this.canvasContext.clearRect(0, 0, this.canvasContext.width, this.canvasContext.height);
    // 绘制白色背景
    this.canvasContext.fillStyle = '#FFFFFF';
    this.canvasContext.fillRect(0, 0, this.canvasContext.width, this.canvasContext.height);
    this.positionList.forEach((position) => {
      Logger.info('Index', `redraw position=${JSON.stringify(position)}`);
      if (position.isFirstPosition) {
        this.canvasContext.beginPath();
        this.canvasContext.strokeStyle = position.strokeStyle;
        this.canvasContext.lineWidth = CommonConstants.CANVAS_LINE_WIDTH;
        this.canvasContext.lineJoin = CommonConstants.CANVAS_LINE_JOIN;
        this.canvasContext.moveTo(position.positionX, position.positionY);
      } else {
        this.canvasContext.lineTo(position.positionX, position.positionY);
        if (position.isEndPosition) {
          this.canvasContext.stroke();
        }
      }
    });
  }

  /**
   * Retract the previous stroke.
   */
  goBack(): void {
    if (this.positionList.length === 0) {
      return;
    }

    for (let i: number = this.positionList.length - 1; i >= 0; i--) {
      let position: Position | undefined = this.positionList.pop();
      if (position !== undefined && position.isFirstPosition) {
        break;
      }
    }
    this.redraw();
    this.globalObject.update(this.positionList, false, this.strokeStyle)
    // this.kvStoreModel.put(CommonConstants.CHANGE_POSITION, JSON.stringify(this.positionList));
  }

  /**
   * Draw events.
   *
   * @param event Touch event.
   */
  onTouchEvent(event: TouchEvent): void {
    let positionX: number = event.touches[0].x;
    let positionY: number = event.touches[0].y;
    switch (event.type) {
      case TouchType.Down: {
        this.canvasContext.beginPath();
        this.canvasContext.strokeStyle = this.strokeStyle;
        this.canvasContext.lineWidth = CommonConstants.CANVAS_LINE_WIDTH;
        this.canvasContext.lineJoin = CommonConstants.CANVAS_LINE_JOIN;
        this.canvasContext.moveTo(positionX, positionY);
        this.canvasContext.lineTo(positionX - 1, positionY - 1);
        this.canvasContext.stroke();
        this.pushData(true, false, positionX, positionY);
        break;
      }
      case TouchType.Move: {
        this.canvasContext.lineTo(positionX, positionY);
        this.canvasContext.stroke();
        this.pushData(false, false, positionX, positionY);
        break;
      }
      case TouchType.Up: {
        this.canvasContext.lineTo(positionX, positionY);
        this.canvasContext.stroke();
        this.pushData(false, true, positionX, positionY);
        break;
      }
      default: {
        break;
      }
    }
  }

  /**
   * Add position to position list.
   *
   * @param isFirstPosition Indicates whether the position is the start position of the track.
   * @param isEndPosition Indicates whether the position is the end position of the track.
   * @param positionX Horizontal coordinate information.
   * @param positionY Indicates the vertical coordinate information.
   */
  pushData(isFirstPosition: boolean, isEndPosition: boolean, positionX: number, positionY: number): void {
    let position = new Position(isFirstPosition, isEndPosition, positionX, positionY, this.strokeStyle);
    this.positionList.push(position);
    this.globalObject.add(isFirstPosition, isEndPosition, positionX, positionY, this.strokeStyle)
    // if (position.isEndPosition) {
    //   this.kvStoreModel.put(CommonConstants.CHANGE_POSITION, JSON.stringify(this.positionList));
    // }
  }

  /**
   * Click the transfer button to display nearby devices and open the pop-up window.
   */
  onContinueAbilityClick(): void {
    remoteDeviceModel.startDeviceDiscovery();
    this.dialogController.open();
  }

  /**
   * Select a device from the list in the pop-up window.
   * @param context Ability context.
   * @param device Device information.
   * @param positionList Position list.
   */
  startAbilityContinuation(
    context: common.UIAbilityContext,
    devices: deviceManager.DeviceInfo[],
    sessionId: string,
    positionList: Position[],
    strokeStyle: string
  ): void {
    remoteDeviceModel.authenticateDevice(context, devices, sessionId, positionList, strokeStyle);
    remoteDeviceModel.stopDeviceDiscovery();
  }
}