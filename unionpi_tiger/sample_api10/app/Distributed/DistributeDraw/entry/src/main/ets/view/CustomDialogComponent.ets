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
import prompt from '@ohos.promptAction';
import CommonConstants from '../common/constants/CommonConstants';
import Logger from '../common/utils/Logger';
import DistributedObjectModel from '../viewmodel/DistributedObjectModel';
import Position from '../viewmodel/Position';

@Extend(Text)
function ButtonTextStyle() {
  .fontColor($r('app.color.button_text_color'))
  .fontSize($r('app.float.button_font_size'))
  .fontWeight(CommonConstants.FONT_WEIGHT_500)
}

@CustomDialog
export default struct DeviceListDialogComponent {
  @State selectedIndex: Array<number> = [];
  private sessionId: string = '';
  @StorageLink('objectModel') globalObject: DistributedObjectModel = new DistributedObjectModel();
  @Link positionList: Position[];
  @Link strokeStyle: string;
  private canvasContext: CanvasRenderingContext2D | undefined = undefined;
  @Link deviceList: deviceManager.DeviceInfo[];
  controller?: CustomDialogController;
  startAbility: (context: common.UIAbilityContext, devices: deviceManager.DeviceInfo[], sessionId: string, positionList: Position[], strokeStyle: string) => void
    = () => {
  };
  share: () => void
    = () => {
  };
  cancel: () => void = () => {
  };

  build() {
    Column() {
      Row() {
        Text($r('app.string.select_device'))
          .fontSize($r('app.float.dialog_title_font_size'))
          .width(CommonConstants.FULL_PERCENT)
          .textAlign(TextAlign.Start)
          .fontColor(Color.Black)
          .fontWeight(FontWeight.Bold)
      }
      .padding({
        left: $r('app.float.dialog_title_padding_left'),
        right: $r('app.float.dialog_title_padding_left'),
        top: $r('app.float.dialog_title_padding_top')
      })
      .height($r('app.float.dialog_title_height'))

      List() {
        ForEach(this.deviceList, (item: deviceManager.DeviceInfo, index: number) => {
          ListItem() {
            Column() {
              Row() {
                Row() {
                  Row() {
                    Image(this.getDeviceTypeIcon(item.deviceType))
                      .opacity($r('app.float.device_icon_opacity'))
                  }
                  .width($r('app.float.device_icon_width'))
                  .aspectRatio(CommonConstants.NUMBER_ONE)

                  Text(item.deviceName)
                    .fontSize($r('app.float.device_name_font_size'))
                    .width(CommonConstants.DEVICE_NAME_WIDTH)
                    .fontColor(Color.Black)
                    .margin({ left: $r('app.float.device_name_margin_left') })
                    .maxLines(CommonConstants.NUMBER_ONE)
                    .textOverflow({ overflow: TextOverflow.Ellipsis })
                    .textAlign(TextAlign.Start)
                }
                .justifyContent(FlexAlign.Start)

                if (this.selectedIndex.includes(index)) {
                  Image($r('app.media.ic_checked'))
                    .width(CommonConstants.SELECT_ICON_WIDTH)
                    .objectFit(ImageFit.Contain)
                } else {
                  Image($r('app.media.ic_uncheck'))
                    .width(CommonConstants.SELECT_ICON_WIDTH)
                    .objectFit(ImageFit.Contain)
                    .opacity($r('app.float.icon_uncheck_opacity'))
                }
              }
              .height($r('app.float.device_info_height'))
              .onClick(() => {
                if (this.selectedIndex.includes(index)) {
                  this.selectedIndex = this.selectedIndex.filter(item => item !== index)
                } else {
                  this.selectedIndex.push(index);
                }
                console.error("selectedIndex: " + JSON.stringify(this.selectedIndex))
              })
              .padding({
                left: $r('app.float.device_info_padding'),
                right: $r('app.float.device_info_padding')
              })
              .width(CommonConstants.FULL_PERCENT)
              .justifyContent(FlexAlign.SpaceBetween)

              if (index !== this.deviceList.length - 1) {
                Row() {
                  Divider()
                    .width(CommonConstants.FULL_PERCENT)
                    .height($r('app.float.device_info_divider_height'))
                    .opacity($r('app.float.device_info_divider_opacity'))
                }
                .padding({
                  left: $r('app.float.device_info_divider_padding_left'),
                  right: $r('app.float.device_info_divider_padding_right')
                })
                .width(CommonConstants.FULL_PERCENT)
              }
            }
          }
        }, (item: deviceManager.DeviceInfo) => item.deviceId)
      }
      .width(CommonConstants.FULL_PERCENT)
      .constraintSize({ maxHeight: 168 })

      Row() {
        Column() {
          Text($r('app.string.cancel'))
            .ButtonTextStyle()
        }
        .layoutWeight(CommonConstants.NUMBER_ONE)
        .justifyContent(FlexAlign.Center)
        .height($r('app.float.button_line_height'))
        .onClick(() => {
          if (this.controller !== undefined) {
            this.controller.close();
          }
          this.cancel();
        })

        Divider()
          .vertical(true)
          .height($r('app.float.button_line_divider_height'))
          .color($r('app.color.button_line_divider_color'))
          .width($r('app.float.button_line_divider_width'))

        Column() {
          Text($r('app.string.confirm'))
            .ButtonTextStyle()
        }
        .layoutWeight(CommonConstants.NUMBER_ONE)
        .justifyContent(FlexAlign.Center)
        .height($r('app.float.button_line_height'))
        .onClick(() => {
          if (this.selectedIndex.length === 0) {
            prompt.showToast({
              message: $r('app.string.please_select_device')
            });
          } else if (this.selectedIndex.length === 1 && this.selectedIndex[0] === 0) {
            Logger.info('DeviceListDialogComponent', `stop ability`)
            this.globalObject.update(this.positionList, true, this.strokeStyle)
            this.deviceList = []
            if (this.controller !== undefined) {
              this.controller.close()
            }
            return
          } else {
            if (this.controller !== undefined) {
              this.controller.close();
            }
            let selectedDeviceList: deviceManager.DeviceInfo[] = []
            for (let index = 0; index < this.selectedIndex.length; index++) {
              selectedDeviceList.push(this.deviceList[this.selectedIndex[index]])
            }
            this.globalObject = new DistributedObjectModel();
            this.sessionId = this.globalObject.genSessionId();
            AppStorage.SetOrCreate('sessionId', this.sessionId);
            this.share()
            this.startAbility(getContext(this) as common.UIAbilityContext, selectedDeviceList,
              this.sessionId, this.positionList, this.strokeStyle);
          }
        })
      }
      .backgroundColor(Color.White)
      .border({
        color: Color.White,
        radius: $r('app.float.button_line_radius')
      })
      .padding($r('app.float.button_line_padding'))
      .margin({ top: $r("app.float.button_line_margin_top") })
    }
  }

  /**
   * Obtains the device type icon.
   *
   * @param deviceType Device type.
   * @returns Icon resource.
   */
  getDeviceTypeIcon(deviceType: deviceManager.DeviceType): Resource {
    if ((deviceType === deviceManager.DeviceType.PHONE) || (deviceType === deviceManager.DeviceType.UNKNOWN_TYPE)) {
      return $r('app.media.ic_public_devices_phone');
    } else if (deviceType === deviceManager.DeviceType.TV) {
      return $r('app.media.ic_smartscreen');
    } else {
      return $r('app.media.ic_smartscreen');
    }
  }
}