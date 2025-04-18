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

import AbilityConstant from '@ohos.app.ability.AbilityConstant';
import UIAbility from '@ohos.app.ability.UIAbility';
import Want from '@ohos.app.ability.Want';
import window from '@ohos.window';
import bundleManager from '@ohos.bundle.bundleManager';
import abilityAccessCtrl, { Permissions } from '@ohos.abilityAccessCtrl';
import common from '@ohos.app.ability.common';
import Logger from '../common/utils/Logger';
import remoteDeviceModel from '../common/utils/RemoteDeviceUtil';
import Position from '../viewmodel/Position';
import DistributedObjectModel from '../viewmodel/DistributedObjectModel';

const permissions: Array<Permissions> = ['ohos.permission.DISTRIBUTED_DATASYNC', "ohos.permission.READ_MEDIA", "ohos.permission.WRITE_IMAGEVIDEO"];

export default class EntryAbility extends UIAbility {
  storage: LocalStorage = new LocalStorage();
  want?: Want;

  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam) {
    this.want = want;
    Logger.info('EntryAbility',
      `onCreate want=${JSON.stringify(this.want)}, launchParam=${JSON.stringify(launchParam)}`);
    remoteDeviceModel.createDeviceManager();
    if (this.want?.parameters?.sessionId) {
      let sessionId = this.want.parameters.sessionId ? this.want.parameters.sessionId : '';
      AppStorage.SetOrCreate('sessionId', sessionId);
      AppStorage.SetOrCreate('objectModel', new DistributedObjectModel());
    }
  }

  onNewWant(want: Want, launchParam: AbilityConstant.LaunchParam) {
    this.want = want;
    Logger.info('EntryAbility',
      `onNewWant want=${JSON.stringify(this.want)}, launchParam=${JSON.stringify(launchParam)}`);

    // if (this.want?.parameters?.positionList) {
    // let positionList: Position[] = JSON.parse((this.want.parameters.positionList) as string);
    // this.storage.setOrCreate('positionList', positionList);
    //   this.storage.setOrCreate('updateCanvas', true);
    // }

    if (this.want?.parameters?.sessionId) {
      AppStorage.SetOrCreate('sessionId', this.want.parameters.sessionId);
      let objectModel = AppStorage.Get('objectModel') as DistributedObjectModel;
      objectModel.off();
      AppStorage.SetOrCreate('objectModel', new DistributedObjectModel());
    }
  }

  onDestroy() {
    Logger.info('EntryAbility', 'onDestroy execute');
    remoteDeviceModel.unregisterDeviceListCallback();
  }

  onWindowStageCreate(windowStage: window.WindowStage) {
    Logger.info('EntryAbility', 'onWindowStageCreate start');

    if (this?.want?.parameters?.positionList) {
      let positionList: Position[] = JSON.parse((this.want.parameters.positionList) as string);
      this.storage.setOrCreate('positionList', positionList);
    }
    if (this?.want?.parameters?.strokeStyle) {
      let strokeStyle: string = this.want.parameters.strokeStyle as string;
      this.storage.setOrCreate('strokeStyle', strokeStyle);
    }

    checkPermissions(this.context);

    try {
      windowStage.loadContent('pages/Index', this.storage, (err) => {
        if (err) {
          Logger.error('EntryAbility',
            `onWindowStageCreate Failed to load the content. code=${err.code}, msg=${JSON.stringify(err.message)}`);
          return;
        }
        Logger.info('EntryAbility', 'onWindowStageCreate Succeeded in loading the content');
      });
    } catch (error) {
      Logger.error('EntryAbility', `onWindowStageCreate failed. err=${JSON.stringify(error)}`);
    }
  }
}

/**
 * Check the permission status.
 */
async function checkPermissions(context: common.UIAbilityContext) {
  let grantStatus: abilityAccessCtrl.GrantStatus = await checkAccessToken();
  if (grantStatus !== abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED) {
    // Applying for the distributed data synchronization permission.
    let atManager: abilityAccessCtrl.AtManager = abilityAccessCtrl.createAtManager();
    atManager.requestPermissionsFromUser(context, permissions).then((data) => {
      let grantStatus: Array<number> = data.authResults;
      let length: number = grantStatus.length;
      for (let i: number = 0; i < length; i++) {
        if (grantStatus[i] === 0) {
          Logger.info('EntryAbility', `checkPermissions request permission ${permissions[i]} success`);
        } else {
          Logger.error('EntryAbility',
            `checkPermissions request fail permission=${permissions[i]}, grantStatus=${grantStatus[i]}`);
          return;
        }
      }
    }).catch((err: Error) => {
      Logger.error('EntryAbility',
        `checkPermissions request permissions failed, err=${JSON.stringify(err)}`);
    })
  }
}

/**
 * Obtains the authorization status.
 *
 * @returns Grant status.
 */
async function checkAccessToken() {
  let atManager = abilityAccessCtrl.createAtManager();
  let tokenId: number = 0;
  try {
    let bundleInfo: bundleManager.BundleInfo = await bundleManager
      .getBundleInfoForSelf(bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION);
    let appInfo: bundleManager.ApplicationInfo = bundleInfo.appInfo;
    tokenId = appInfo.accessTokenId;
  } catch (err) {
    Logger.error('EntryAbility',
      `checkAccessToken Failed to get bundle info for self. err=${JSON.stringify(err)}`);
  }

  let grantStatus: abilityAccessCtrl.GrantStatus = abilityAccessCtrl.GrantStatus.PERMISSION_DENIED;
  try {
    grantStatus = await atManager.checkAccessToken(tokenId, 'ohos.permission.DISTRIBUTED_DATASYNC');
  } catch (err) {
    Logger.error('EntryAbility',
      `checkAccessToken Failed to check access token. err=${JSON.stringify(err)}`);
  }

  return grantStatus;
}