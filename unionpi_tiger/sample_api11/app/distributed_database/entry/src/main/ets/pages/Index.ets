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
import UIAbility from '@ohos.app.ability.UIAbility';
import common from '@ohos.app.ability.common';
import { BusinessError, Callback } from '@ohos.base';
import distributedData from '@ohos.data.distributedKVStore';
import data from '@ohos.telephony.data';

@CustomDialog
struct DistributedKVStoreDialog {
  controller: CustomDialogController
  @Prop message: string = ''

  build() {
    Column() {
      Row() {
        Text(this.message)
          .width('100%')
          .height('100%')
          .fontSize(24)
          .lineHeight(28)
          .textAlign(TextAlign.Center)
      }
      .height(50)
      .width(100)
    }
  }
}

let context: common.Context = getContext(this)

let kvManager: distributedData.KVManager | null
let KvStore: distributedData.SingleKVStore | null

@Entry
@Component
struct Index {
  @State value: number = 0;
  private storageKey: string = "key_test_number";
  @State message: string = ''
  dialogController: CustomDialogController = new CustomDialogController({
    builder: DistributedKVStoreDialog({
      message: this.message
    }),
    alignment: DialogAlignment.Center
  })

  // 组件即将显示时初始化KVManager
  aboutToAppear() {
    this.createKvStore((data) => {

    });
  }

  // 构建UI
  build() {
    Row() {
      Column() {
        Text(`${this.value}`)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)

        Button() {
          Text('+')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
            .fontStyle(FontStyle.Italic)
        }
        .type(ButtonType.Capsule)
        .margin({ top: 20 })
        .backgroundColor('#0D9FFB')
        .width('20%')
        .height('10%')
        .onClick(() => {
          this.incrementValue();
        })

        Button() {
          Text('写入数据库')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
            .fontStyle(FontStyle.Italic)
        }
        .type(ButtonType.Capsule)
        .margin({ top: 20 })
        .backgroundColor('#0D9FFB')
        .width('20%')
        .height('10%')
        .onClick(() => {
          this.KvStorePut();
        })
      }
      .width('100%')
    }
    .height('100%')
  }

  // 弹窗关闭
  private CloseDialog() {
    setTimeout(() => {
      this.dialogController.close()
    }, 1000)
  }

  // 弹窗开启
  private OpenDialog(message: string) {
    this.message = message
    this.dialogController.open()
    this.CloseDialog()
  }

  // 递增value值
  private incrementValue() {
    this.value += 1;
  }

  // 写入数据到KVStore
  private KvStorePut() {
    try {
      if (KvStore) {
        KvStore.put(this.storageKey, this.value as number).then(() => {
          console.info(`Succeeded in putting data`);
          this.OpenDialog('写入成功')
        }).catch((err: BusinessError) => {
          console.error(`Failed to put data. Code: ${err.code}, Message: ${err.message}`);
          this.OpenDialog(`写入失败: ${err.message}`);
        });
      } else {
        console.error('KVStore is not initialized yet.');
        this.OpenDialog('KVStore未初始化');
      }
    } catch (e) {
      let error = e as BusinessError;
      console.error(`Unexpected error occurred. Code: ${error.code}, Message: ${error.message}`);
      this.OpenDialog(`写入失败: ${error.message}`);
    }
  }

  // 初始化KVManager
  private async createKvStore(callback: Callback<void>) {
    if ((typeof (KvStore) != 'undefined')) {
      callback();
      return;
    }
    let context: common.Context = getContext(this);
    const kvManagerConfig: distributedData.KVManagerConfig = {
      context: context,
      bundleName: 'com.example.distributed_database'
    };

    try {
      kvManager = distributedData.createKVManager(kvManagerConfig);
      console.info("KVManager initialized successfully");
      this.getKVStore(callback);
      this.setOnMessageReceivedListener();
    } catch (e) {
      let error = e as BusinessError;
      console.error(`Failed to create KVManager. Code: ${error.code}, Message: ${error.message}`);
      this.OpenDialog(`初始化失败: ${error.message}`);
    }
  }

  // 获取KVStore实例
  private getKVStore(callback: Callback<void>) {
    try {
      const options: distributedData.Options = {
        createIfMissing: true,
        encrypt: false,
        backup: false,
        autoSync: true,
        kvStoreType: distributedData.KVStoreType.SINGLE_VERSION,
        securityLevel: distributedData.SecurityLevel.S1,
      };

      if (kvManager) {
        kvManager.getKVStore('KVStoreTest', options, (err: BusinessError, store: distributedData.SingleKVStore) => {
          if (err) {
            console.error(`Failed to get KVStore. Code: ${err.code}, Message: ${err.message}`);
            this.OpenDialog(`获取KVStore失败: ${err.message}`);
            return;
          }
          console.info("Succeeded in getting KVStore");
          KvStore = store;
          callback()
        });
      } else {
        console.error('KVManager is not initialized yet.');
        this.OpenDialog('KVManager未初始化');
      }
    } catch (e) {
      let error = e as BusinessError;
      console.error(`Unexpected error occurred while getting KVStore. Code: ${error.code}, Message: ${error.message}`);
      this.OpenDialog(`获取KVStore失败: ${error.message}`);
    }
  }

  private closeKVStore() {
    try {
      const options: distributedData.Options = {
        createIfMissing: true,
        encrypt: false,
        backup: false,
        autoSync: false,
        kvStoreType: distributedData.KVStoreType.SINGLE_VERSION,
        schema: undefined,
        securityLevel: distributedData.SecurityLevel.S2,
      };
      if (kvManager) {
        kvManager.getKVStore('KVStoreTest', options, async (err: BusinessError, store: distributedData.SingleKVStore | null) => {
          if (err != undefined) {
            console.error(`Failed to get KVStore. Code: ${err.code}, Message: ${err.message}`);
            this.OpenDialog(`获取KVStore失败: ${err.message}`);
            return;
          }
          console.info("Succeeded in getting KVStore");
          KvStore = store;
          KvStore = null;
          store = null;
          kvManager?.closeKVStore('closeKVStore', 'KVStoreTest', (err: BusinessError) => {
            if (err != undefined) {
              console.error(`Failed to close KVStore.code is ${err.code},message is ${err.message}`)
              return;
            }
            console.info('Succeeded in closing KVStore');
          });
        });
      } else {
        console.error('KVManager is not initialized yet.');
        this.OpenDialog('KVManager未初始化');
      }
    } catch (e) {
      let error = e as BusinessError;
      console.error(`Unexpected error occurred while getting KVStore. Code: ${error.code}, Message: ${error.message}`);
      this.OpenDialog(`关闭KVStore失败: ${error.message}`);
    }
  }

  private deleteKVStore() {
    try {
      const options: distributedData.Options = {
        createIfMissing: true,
        encrypt: false,
        backup: false,
        autoSync: false,
        kvStoreType: distributedData.KVStoreType.SINGLE_VERSION,
        schema: undefined,
        securityLevel: distributedData.SecurityLevel.S2,
      };
      if (kvManager) {
        kvManager.getKVStore('KVStoreTest', options, async (err: BusinessError, store: distributedData.SingleKVStore | null) => {
          if (err != undefined) {
            console.error(`Failed to get KVStore. Code: ${err.code}, Message: ${err.message}`);
            this.OpenDialog(`获取KVStore失败: ${err.message}`);
            return;
          }
          console.info("Succeeded in getting KVStore");
          KvStore = store;
          KvStore = null;
          store = null;
          kvManager?.deleteKVStore('deleteKVStore', 'KVStoreTest', (err: BusinessError) => {
            if (err != undefined) {
              console.error(`Failed to delete KVStore.code is ${err.code},message is ${err.message}`)
              return;
            }
            console.info('Succeeded in deleting KVStore');
          });
        });
      } else {
        console.error('KVManager is not initialized yet.');
        this.OpenDialog('KVManager未初始化');
      }
    } catch (e) {
      let error = e as BusinessError;
      console.error(`Unexpected error occurred while getting KVStore. Code: ${error.code}, Message: ${error.message}`);
      this.OpenDialog(`销毁KVStore失败: ${error.message}`);
    }
  }

  private enableSync() {
    try {
      KvStore?.enableSync(true, (err: BusinessError) => {
        if (err == undefined) {
          console.info('Succeeded in enabling sync');
        } else {
          console.error(`Failed to enable sync.code is ${err.code},message is ${err.message}`);
        }
      });
    } catch (e) {
      let error = e as BusinessError;
      console.error(`An unexpected error occurred.code is ${error.code},message is ${error.message}`);
    }
  }

  private setOnMessageReceivedListener() {
    try {
      // 保存当前实例引用
      let self = this;

      // 判断 KVManager 是否初始化
      if (kvManager) {
        console.info('KVManager is listening for data changes.');

        // 定义 KVManager 服务异常处理的回调
        const deathCallback = () => {
          console.info('KVManager service is dead, handling recovery.');
          this.OpenDialog('KVManager服务已停止！');
        };

        // 订阅 dataChange 事件
        KvStore?.on('dataChange', 1, (data) => {
          // 处理接收到的数据
          console.info('Data change received: ', data);

          // 可以在这里处理数据变化的逻辑
          // 例如: self.handleDataChange(data);
        });

        // 如果 KVManager 需要监听死亡事件，加入回调
        kvManager.on('distributedDataServiceDie', deathCallback);

      } else {
        console.error('KVManager is not initialized yet, cannot subscribe.');
      }

    } catch (e) {
      // 捕获并处理异常
      console.error(`Unexpected error occurred while subscribing. Code: ${e.code}, Message: ${e.message}`);
    }
  }
}
