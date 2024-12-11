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


import distributedDataObject from '@ohos.data.distributedDataObject';
import deviceManager from '@ohos.distributedDeviceManager';
import { Hero } from '../object/hero'
import common from '@ohos.app.ability.common';
import Want from '@ohos.app.ability.Want';
import { BusinessError } from '@ohos.base';

let hero = new Hero()
let context = getContext(this) as common.UIAbilityContext


export let globalObject = distributedDataObject.create(context, {
  name: undefined,
  status: 2,
  hero: undefined,
  generate: undefined,
  generateBig: undefined,
  mode: undefined,
})

export function GetDevice(): deviceManager.DeviceBasicInfo[] {
  const myDeviceManager = deviceManager.createDeviceManager('com.example.myDevice');
  const deviceList = myDeviceManager.getAvailableDeviceListSync()
  console.info(JSON.stringify(deviceList))
  return deviceList
}

export function ActiveApp(deviceId: string): Array<number> {

  let Coordinate: Array<number> = []
  let sessionId = distributedDataObject.genSessionId()
  let want: Want = {
    deviceId: deviceId,
    bundleName: 'com.example.myDevice',
    abilityName: 'EntryAbility',
    parameters: {
      info: 1
    }
  }

  context.startAbility(want, (error: BusinessError) => {
    if (error.code) {
      // 处理业务逻辑错误
      console.info('startAbility failed, error.code: ' + JSON.stringify(error.code) +
        ' error.message: ' + JSON.stringify(error.message));
      return;
    }
    console.info('startAbility succeed')
  });

  globalObject.setSessionId(sessionId)
  globalObject.on('change', (sessionId: string, changeData: string[]) => {
    console.info(`change: ${sessionId}`);
    if (changeData !== null && changeData !== undefined) {
      changeData.forEach(element => {
        console.info(`The element ${globalObject[element]} changed.`);
      });
    }
  });
  return Coordinate
}

