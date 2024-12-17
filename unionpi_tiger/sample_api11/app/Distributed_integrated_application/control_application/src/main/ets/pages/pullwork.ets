# Copyright (c) 2022 Unionman Technology Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import abilityAccessCtrl, { Permissions } from '@ohos.abilityAccessCtrl';
import common from '@ohos.app.ability.common';
import { BusinessError } from '@ohos.base';
import bundleManager from '@ohos.bundle.bundleManager';
import deviceManager from '@ohos.distributedDeviceManager';
import { ErrorCode } from '@ohos.ability.errorCode';
import Want from '@ohos.app.ability.Want';

export async function RequestPermission(bundleName: string) {
  console.info('RequestPermission begin');

  // 动态申请权限的列表
  let permissionList: Array<Permissions> = ['ohos.permission.DISTRIBUTED_DATASYNC'];

  let bundleFlag = 0;
  let userId = 100;

  try {
    // 调用 bundleManager 获取远端应用的信息
    let appInfo = await bundleManager.getApplicationInfo(bundleName, bundleFlag, userId);
    if (!appInfo || !appInfo.accessTokenId) {
      console.error('App info or accessTokenId is missing for bundle: ' + bundleName);
      return;
    }

    let tokenId = appInfo.accessTokenId;
    let atManager: abilityAccessCtrl.AtManager = abilityAccessCtrl.createAtManager();

    // 检查当前权限是否已授权
    let requestPermissions: Array<Permissions> = [];
    for (let permission of permissionList) {
      let result = await atManager.verifyAccessToken(tokenId, permission);
      console.info(`verifyAccessToken result for ${permission}: ${JSON.stringify(result)}`);

      // 如果没有授权，则将权限添加到请求列表
      if (result !== abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED) {
        requestPermissions.push(permission);
      }
    }

    console.info('Requesting permissions: ' + JSON.stringify(requestPermissions));

    // 如果没有需要请求的权限，则不需要继续
    if (requestPermissions.length === 0) {
      console.info('All required permissions are already granted');
      return;
    }

    // 获取上下文并请求权限
    let context = getContext() as common.UIAbilityContext;
    if (!context) {
      console.error('Failed to get context');
      return;
    }

    // 请求权限
    let result = await atManager.requestPermissionsFromUser(context, requestPermissions);
    console.info('Permissions request result: ' + JSON.stringify(result));
    console.info('Requested permissions: ' + result.permissions);
    console.info('Authorization results: ' + result.authResults);

    // 处理授权结果
    for (let i = 0; i < result.permissions.length; i++) {
      let permission = result.permissions[i];
      let authResult = result.authResults[i];
      if (authResult === abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED) {
        console.info(`Permission granted for: ${permission}`);
      } else {
        console.warn(`Permission denied for: ${permission}`);
      }
    }

  } catch (err) {
    console.error('Error occurred during permission request: ' + err.message);
    console.error(err);
  }

  console.info('RequestPermission end');
}


export function CreateWork(localName: string, remoteName: string, abilityName: string) {
  let devManager = deviceManager.createDeviceManager(localName);
  let devList = devManager.getAvailableDeviceListSync();
  let want: Want = {
    deviceId: devList[0].deviceId,
    bundleName: remoteName,
    abilityName: abilityName
  }
  let context = getContext() as common.UIAbilityContext;
  context.startAbility(want, (error: BusinessError) => {
    if (error.code) {
      console.info('startAbility failed, error.code: ' + JSON.stringify(error.code) +
        ' error.message: ' + JSON.stringify(error.message));
      return;
    } else {
      console.info('startAbility succeed')
    }
  })
}