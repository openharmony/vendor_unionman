/*
 * Copyright 2022 Unionman Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import distributedData from '@ohos.data.distributedData';
import prompt from '@ohos.prompt';

let kvManager;
let kvStore;

@Entry
@Component
struct Index {
  private my_key: string = "key_test";
  @State value: number = 0;

  aboutToAppear() {
    this.my_KVManager();
  }

  build() {
    Row() {
      Column() {
        Text("" + this.value)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)

        Button() {
          Text('+')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
            .fontStyle(FontStyle.Italic)
        }
        .type(ButtonType.Capsule)
        .margin({
          top: 20
        })
        .backgroundColor('#0D9FFB')
        .width('20%')
        .height('10%')
        .onClick(() => {
          this.value = this.value + 1;
        })

        Button() {
          Text('写入数据库')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
            .fontStyle(FontStyle.Italic)
        }
        .type(ButtonType.Capsule)
        .margin({
          top: 20
        })
        .backgroundColor('#0D9FFB')
        .width('20%')
        .height('10%')
        .onClick(() => {
          this.database_write();
        })
      }
      .width('100%')
    }
    .height('100%')
  }

  private database_write() {
    try {
      kvStore.put(this.my_key, this.value, function (err, data) {
        if (err != undefined) {
          console.log("xxxxxx put err: " + JSON.stringify(err));
          return;
        }
        console.log("xxxxxxx put success");
        prompt.showToast({
          message: "写入成功",
        })
      });
    } catch (e) {
      console.log("An unexpected error occurred. Error: " + e);
    }
  }

  private my_KVManager() {
    try {
      const kvManagerConfig = {
        bundleName: 'com.example.distributed_database',
        userInfo: {
          userId: '0',
          userType: distributedData.UserType.SAME_USER_ID
        }
      }
      distributedData.createKVManager(kvManagerConfig).then((manager) => {
        console.log("xxxxxx createKVManager success");
        kvManager = manager;
        this.my_getKVStore();
      }).catch((err) => {
        console.log("xxxxxx createKVManager err: " + JSON.stringify(err));
      });
    } catch (e) {
      console.log("xxxxxx my_KVManager An unexpected error occurred. Error: " + e);
    }
  }

  private my_getKVStore() {
    try {
      const options = {
        createIfMissing: true,
        encrypt: false,
        backup: false,
        autoSync: true,
        kvStoreType: distributedData.KVStoreType.SINGLE_VERSION,
        securityLevel: distributedData.SecurityLevel.S0
      };
      kvManager.getKVStore('storeId', options).then((store) => {
        console.log("xxxxxx getKVStore success");
        kvStore = store;
        this.my_subscribe();
        prompt.showToast({
          message: "数据库初始化成功",
        })
      }).catch((err) => {
        console.log("xxxxxx getKVStore err: " + err);
      });
    } catch (e) {
      console.log("xxxxxx my_getKVStroe An unexpected error occurred. Error: " + e);
    }
  }

  private my_subscribe() {
    kvStore.on('dataChange', distributedData.SubscribeType.SUBSCRIBE_TYPE_ALL, (data) => {
      this.value = data.updateEntries[0].value.value;
      console.log("xxxxxx dataChange callback call data: " + JSON.stringify(data));
      console.log("xxxxxx this.value: " + this.value);
    });
  }
}

