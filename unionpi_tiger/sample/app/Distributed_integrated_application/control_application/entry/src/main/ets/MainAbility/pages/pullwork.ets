/* Copyright 2022 Unionman Technology Co., Ltd.
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
// @ts-nocheck
import featureAbility from '@ohos.ability.featureAbility'
import deviceManager from '@ohos.distributedHardware.deviceManager';
import abilityAccessCtrl from "@ohos.abilityAccessCtrl";
import bundle from '@ohos.bundle';


export async function RequestPermission(bundle_Name: string) {
  console.info('xxxx RequestPermission begin');
  // 动态申请权限的列表
  let array: Array<string> = ["ohos.permission.DISTRIBUTED_DATASYNC"];
  let bundleFlag = 0;
  let tokenID = undefined;
  let userID = 100;
  // 调用bundle函数得到远端APP的信息,bundle_name为本地的bundle_name
  let appInfo = await bundle.getApplicationInfo(bundle_Name, bundleFlag, userID);
  // 通过appInfo得到要访问远程设备的令牌ID（安全标识）
  tokenID = appInfo.accessTokenId;
  // 创建AtManager
  let atManager = abilityAccessCtrl.createAtManager();
  let requestPermissions: Array<string> = [];
  // 通过循环来判断每一个权限是否通过用户的授权
  for (let i = 0;i < array.length; i++) {
    // 通过指定的tokenId来看是否有访问权限
    let result = await atManager.verifyAccessToken(tokenID, array[i]);
    console.info("xxxx verifyAccessToken result:" + JSON.stringify(result));
    if (result != abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED) {
      requestPermissions.push(array[i]);
    }
  }
  console.info("xxxx requestPermissions:" + JSON.stringify(requestPermissions));
  if (requestPermissions.length == 0 || requestPermissions == []) {
    return;
  }
  // 得到上下文
  let context = featureAbility.getContext();
  context.requestPermissionsFromUser(requestPermissions, 1, (data) => {
    console.info("xxxx data:" + JSON.stringify(data));
    console.info("xxxx data requestCode:" + data.requestCode);
    console.info("xxxx data permissions:" + data.permissions);
    console.info("xxxx data authResults:" + data.authResults);
  });
  console.info('xxxx RequestPermission end');
}

export function CreatePullWork(local_name: string, remote_name: string, ability_name: string) {
  deviceManager.createDeviceManager(local_name, (err, value) => {
    if (!err) {
      let devManager = value;
      let deviceList = devManager.getTrustedDeviceListSync();
      featureAbility.startAbility({
        want:
        {
          deviceId: deviceList[0].deviceId,
          bundleName: remote_name,
          abilityName: ability_name
        }
      }).then((data) => {
        console.info('xxxx Operation success. Data:' + JSON.stringify(data));
      }).catch((error) => {
        console.info('xxxx Operation failed. Cause:' + JSON.stringify(error));
      })
    }
  })
}