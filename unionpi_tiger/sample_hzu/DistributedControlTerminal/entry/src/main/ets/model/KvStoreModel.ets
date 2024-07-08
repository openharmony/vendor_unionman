/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

import distributedData from '@ohos.data.distributedKVStore'
import common from '@ohos.app.ability.common'
import Logger from '../util/Logger'
import { Callback } from '@ohos.base';
const STORE_ID = 'etskvstore'
const TAG = 'KvStoreModel'

export class KvStoreModel {
  private kvManager?: distributedData.KVManager = undefined;
  private kvStore?: distributedData.SingleKVStore = undefined;
  constructor() { }
  async createKvStore(callback: Callback<void>) {
    if ((typeof (this.kvStore) !== 'undefined')) {
      callback()
      return;
    }
    let context: common.Context = getContext(this)
    let config: distributedData.KVManagerConfig = {
      context,
      bundleName: 'ohos.samples.kvstore',
    }
    Logger.info(TAG, 'createKVManager begin')
    try {
      this.kvManager = await distributedData.createKVManager(config)
    } catch (err) {
      Logger.info(TAG, `ecreateKVManager err:${JSON.stringify(err)}`)
    }
    Logger.info(TAG, 'ecreateKVManager end')
    let options: distributedData.Options = {
      createIfMissing: true, encrypt: false,
      backup: false, autoSync: true,
      kvStoreType: distributedData.KVStoreType.SINGLE_VERSION,
      securityLevel: distributedData.SecurityLevel.S1,
    }
    Logger.info(TAG, 'kvManager.getKVStore begin')
    if (this.kvManager !== undefined) {
      this.kvManager.getKVStore(STORE_ID, options, (err, store: distributedData.SingleKVStore) => {
        Logger.info(TAG, `getKVStore success, kvStore= ${store}`);
        this.kvStore = store;
        callback();
      })
    }
    Logger.info(TAG, 'kvstorTAG,ekvManager.getKVStore end')
  }
  put(key: string, value: string) {
    if (this.kvStore) {
      Logger.debug(TAG, `kvStore.put ${key} = ${value}`);
      this.kvStore.put(
        key,
        value
      ).then((data) => {
        Logger.debug(TAG, `kvStore.put ${key} finished, data= ${JSON.stringify(data)}`);
      }).catch((err: object) => {
        Logger.debug(TAG, `kvStore.put ${key} failed, ${JSON.stringify(err)}`);
      })
    }
  }
  get(key: string, callback: Callback<string>) {
    if (this.kvStore) {
      Logger.debug(TAG, `kvStore.get ${key}`);
      this.kvStore.get(key).then((data) => {
        Logger.debug(TAG, `kvStore.get ${key} finished, data= ${JSON.stringify(data)}`);
        callback(data.valueOf().toString());
      }).catch((err: object) => {
        Logger.debug(TAG, `kvStore.get ${key} failed, ${JSON.stringify(err)}`);
        callback('');
      })
    }
  }
  setOnMessageReceivedListener(msg: string, callback: Callback<string>) {
    Logger.info(TAG, `setOnMessageReceivedListener ${msg}`);  // 记录日志，指示设置了消息接收监听器
    let self = this; // 将当前对象存储在self中，以便在回调函数中使用
    this.createKvStore(() => { // 创建键值存储后执行的回调函数
      Logger.info(TAG, 'kvStore.on(dataChange) begin'); // 记录日志，表示开始监听键值存储的数据变化
      self.kvStore!.on('dataChange', 1, (data) => { // 监听键值存储的dataChange事件，处理数据变化
        Logger.info(TAG, `dataChange, ${JSON.stringify(data)}`); // 记录日志，显示数据变化的具体内容
        Logger.info(TAG, `dataChange, insert ${data.insertEntries.length} udpate ${data.updateEntries.length}`); // 记录日志，显示插入和更新的条目数
        let entries = data.insertEntries.length > 0 ? data.insertEntries : data.updateEntries; // 根据变化类型选择处理的条目集合
        for (let i = 0; i < entries.length; i++) { // 遍历处理的条目集合
          if (entries[i].key === msg) { // 如果条目的键与传入的消息键(msg)相同
            let value = entries[i].value.value.toString(); // 获取条目的值，并转换为字符串
            Logger.info(TAG, `Entries receive ${msg} = ${value}`); // 记录日志，显示接收到的消息键和值
            callback(value); // 调用传入的回调函数，将值作为参数传递给回调函数
            return; // 结束处理，只处理第一个匹配的条目
          }
        }
      });
      Logger.info(TAG, 'kvStore.on(dataChange) end'); // 记录日志，表示结束监听键值存储的数据变化
    });
  }
}