/*
 * Copyright 2023 Unionman Technology Co., Ltd.
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

import distributedKVStore from '@ohos.data.distributedKVStore';

const STORE_ID = 'distributedvideoplayer';

export class KvStoreModel {
  private kvManager: distributedKVStore.KVManager = undefined;
  private kvStore: distributedKVStore.SingleKVStore = undefined;

  constructor() {
  }

  async createKvStore(callback) {
    if ((typeof (this.kvStore) !== 'undefined')) {
      callback()
      return
    }
    let kvManagerConfig = {
      bundleName: 'com.unionman.distributedvideoplayer',
      context: globalThis.context
    }
    this.kvManager = distributedKVStore.createKVManager(kvManagerConfig)

    let options = {
      createIfMissing: true, // 当数据库文件不存在时创建数据库
      encrypt: false, // 设置数据库文件不加密
      backup: false, // 设置数据库文件不备份
      autoSync: true, // 设置数据库文件自动同步
      kvStoreType: distributedKVStore.KVStoreType.SINGLE_VERSION, // 设置要创建的数据库类型为表示单版本数据库
      securityLevel: distributedKVStore.SecurityLevel.S1, // 设置数据库安全级别为低级别
    }
    // 通过指定Options和storeId，创建并获取KVStore数据库
    this.kvStore = await this.kvManager.getKVStore(STORE_ID, options)
    callback()
  }

  put(key, value) {
    this.kvStore.put(key, value)
  }

  setOnMessageReceivedListener(msg, callback) {
    this.createKvStore(() => {
      this.kvStore.on('dataChange', distributedKVStore.SubscribeType.SUBSCRIBE_TYPE_REMOTE, (data) => {
        let entries = data.insertEntries.length > 0 ? data.insertEntries : data.updateEntries
        for (let i = 0; i < entries.length; i++) {
          if (entries[i].key === msg) {
            let value = entries[i].value.value
            callback(value)
            return;
          }
        }
      });
    })
  }
}