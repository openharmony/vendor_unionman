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

import deviceManager from '@ohos.distributedHardware.deviceManager';

export interface DeviceInfoInterface {
  subscribeId: number,
  device: deviceManager.DeviceInfo
}

export interface AuthExtraInfoInterface {
  targetPkgName: string,
  appName: string,
  appDescription: string,
  business: string
}

/**
 * Common constants for all features.
 */
export default class CommonConstants {
  /**
   * KvStore key.
   */
  static readonly CHANGE_POSITION: string = 'change_position';
  /**
   * KvStore id.
   */
  static readonly KVSTORE_ID: string = 'draw_board_kvstore';
  /**
   * One hundred percent.
   */
  static readonly FULL_PERCENT: string = '100%';
  /**
   * Icon margin left.
   */
  static readonly ICON_MARGIN_LEFT: string = '6.7%';
  /**
   * Title height.
   */
  static readonly TITLE_HEIGHT: string = '10%';
  /**
   * Number one.
   */
  static readonly NUMBER_ONE: number = 1;
  /**
   * Canvas line width.
   */
  static readonly CANVAS_LINE_WIDTH: number = 4;
  /**
   * Canvas line join.
   */
  static readonly CANVAS_LINE_JOIN: 'bevel' | 'miter' | 'round' = 'miter';
  /**
   * Localhost name.
   */
  static readonly LOCALHOST_NAME: string = '本机';
  /**
   * Font weight.
   */
  static readonly FONT_WEIGHT_500: number = 500;
  /**
   * Invalid Index.
   */
  static readonly INVALID_INDEX: number = -1;
  /**
   * Device name width.
   */
  static readonly DEVICE_NAME_WIDTH: string = '78%';
  /**
   * Select icon width.
   */
  static readonly SELECT_ICON_WIDTH: string = '8%';
  /**
   * Subscribe ID range.
   */
  static readonly SUBSCRIBE_ID_RANGE: number = 65536;
  /**
   * Ability name.
   */
  static readonly ABILITY_NAME: string = 'EntryAbility';
  /**
   * Subscribe mode.
   */
  static readonly SUBSCRIBE_MODE: number = 0xAA;
  /**
   * Subscribe medium.
   */
  static readonly SUBSCRIBE_MEDIUM: number = 0;
  /**
   * Subscribe freq.
   */
  static readonly SUBSCRIBE_FREQ: number = 2;
  /**
   * Subscribe capability.
   */
  static readonly SUBSCRIBE_CAPABILITY: number = 0;
  /**
   * Auth type.
   */
  static readonly AUTH_TYPE: number = 1;
  /**
   * App description.
   */
  static readonly APP_DESCRIPTION: string = 'Distribute Draw Ability';
  /**
   * Business type.
   */
  static readonly BUSINESS_TYPE: string = '0';
  /**
   * Bundle name.
   */
  static readonly BUNDLE_NAME: string = 'com.example.distributedraw';
  /**
   * Canvas colors.
   */
  static readonly CANVAS_COLORS: Array<string> = [
  // 第一列：黑色（从深到浅）
    '#000000', '#111111', '#222222', '#333333', '#444444', '#555555',

    // 第二列：红色（从深到浅）
    '#330000', '#550000', '#770000', '#990000', '#BB0000', '#DD0000',

    // 第三列：绿色（从深到浅）
    '#003300', '#005500', '#007700', '#009900', '#00BB00', '#00DD00',

    // 第四列：蓝色（从深到浅，避开第一列）
    '#001133', '#002266', '#003399', '#0044CC', '#0055FF', '#3366FF',

    // 第五列：黄色（从深到浅）
    '#333300', '#555500', '#777700', '#999900', '#BBBB00', '#DDDD00',

    // 第六列：紫色（从深到浅）
    '#330033', '#550055', '#770077', '#990099', '#BB00BB', '#DD00DD',

    // 第七列：青色（从深到浅）
    '#003333', '#005555', '#007777', '#009999', '#00BBBB', '#00DDDD',

    // 第八列：灰色（从深到浅，避开第一列部分值）
    '#666666', '#888888', '#AAAAAA', '#CCCCCC', '#DDDDDD', '#FFFFFF'
  ];
}