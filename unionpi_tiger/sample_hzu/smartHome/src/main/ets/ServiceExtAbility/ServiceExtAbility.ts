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

import ServiceExtensionAbility from '@ohos.app.ability.ServiceExtensionAbility';

const TAG: string = "[ServiceDemo]";

export default class ServiceExtAbility extends ServiceExtensionAbility {
  onCreate(want) {
    console.info(TAG, `onCreate, want: ${want.abilityName}`);
  }

  onRequest(want, startId) {
    console.info(TAG, `onRequest, want: ${want.abilityName}`);
  }

  onConnect(want) {
    console.info(TAG, `onConnect, want: ${want.abilityName}`);
    return null;
  }

  onDisconnect(want) {
    console.info(TAG, `onDisconnect, want: ${want.abilityName}`);
  }

  onDestroy() {
    console.info(TAG, `onDestroy`);
  }
}