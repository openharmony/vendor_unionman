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


import common from '@ohos.app.ability.common';
import { BusinessError } from '@ohos.base';
import { PlaneGame } from './flygame'
import abilityAccessCtrl, { Permissions } from '@ohos.abilityAccessCtrl';

const permissionList: Array<Permissions> = [
  'ohos.permission.DISTRIBUTED_DATASYNC'
]
let atManager: abilityAccessCtrl.AtManager = abilityAccessCtrl.createAtManager();
let context = getContext(this) as common.UIAbilityContext

function requestPermission() {
  atManager.requestPermissionsFromUser(context, permissionList).then((data) => {
    console.info('data:' + JSON.stringify(data));
    console.info('data permissions:' + data.permissions);
    console.info('data authResults:' + data.authResults);
  }).catch((err: BusinessError) => {
    console.log(`NormalCapturer:createAudioCapturer err code:` + err.code + 'err msg:' + err.message);
    console.log(`NormalCapturer:createAudioCapturer err=${JSON.stringify(err)}`);
  });
}

@Entry
@Component
struct Index {
  @State text: string = '';
  @State eventType: string = ''

  async aboutToAppear() {
    await requestPermission()
  }

  build() {
    Row() {
      Column() {
        PlaneGame()
      }
    }
  }
}