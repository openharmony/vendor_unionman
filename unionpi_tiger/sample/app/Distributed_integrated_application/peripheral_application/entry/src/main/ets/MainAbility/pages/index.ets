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

import i2cnapidemo from '@ohos.i2cnapidemo';

import distributedData from '@ohos.data.distributedData';
import prompt from '@ohos.prompt';

import featureAbility from '@ohos.ability.featureAbility'
import bundle from '@ohos.bundle'
import abilityAccessCtrl from '@ohos.abilityAccessCtrl';
import pwmtest from '@ohos.pwmtest';
var kvManager;
var kvStore;

function CreateKVManager() {
  try {
    const kvManagerConfig = {
      bundleName: 'com.example.myapplication',
      userInfo: {
        userId: '0',
        userType: distributedData.UserType.SAME_USER_ID
      }
    }
    distributedData.createKVManager(kvManagerConfig, function (err, manager) {
      if (err) {
        console.log("xxxxx createKVManager err: " + JSON.stringify(err));
        return;
      }
      console.log("xxxxx createKVManager success");
      kvManager = manager;
    });
  } catch (e) {
    console.log("xxxxx An unexpected error occurred. Error: " + e);
  }
}

function GetKVStore() {
  try {
    const options = {
      createIfMissing: true,
      encrypt: false,
      backup: false,
      autoSync: true,
      kvStoreType: distributedData.KVStoreType.SINGLE_VERSION,
      securityLevel: distributedData.SecurityLevel.S0
    };
    kvManager.getKVStore('storeId677', options, function (err, store) {
      if (err) {
        console.log("xxxxx getKVStore err: " + JSON.stringify(err));
        return;
      }
      console.log("xxxxx getKVStore success");
      kvStore = store;
    });
  } catch (e) {
    console.log("xxxxx An unexpected error occurred. Error: " + e);
  }
}

function Subscribe() {
  kvStore.on('dataChange', distributedData.SubscribeType.SUBSCRIBE_TYPE_ALL, function (data) {
    console.log("xxxxx dataChange callback call data: " + JSON.stringify(data));
  });
  console.info("xxxxx dataChange subscribe sucess");
  prompt.showToast({
    message: "数据库初始化成功",
  })
}

async function RequestPermission() {
  console.info('RequestPermission begin');
  let array: Array<string> = ["ohos.permission.DISTRIBUTED_DATASYNC"];
  let bundleFlag = 0;
  let tokenID = undefined;
  let userID = 100;
  let appInfo = await bundle.getApplicationInfo('com.example.myapplication', bundleFlag, userID);
  tokenID = appInfo.accessTokenId;
  let atManager = abilityAccessCtrl.createAtManager();
  let requestPermissions: Array<string> = [];
  for (let i = 0;i < array.length; i++) {
    let result = await atManager.verifyAccessToken(tokenID, array[i]);
    console.info("verifyAccessToken result:" + JSON.stringify(result));
    if (result != abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED) {
      requestPermissions.push(array[i]);
    }
  }
  console.info("requestPermissions:" + JSON.stringify(requestPermissions));
  if (requestPermissions.length == 0 || requestPermissions == []) {
    return;
  }
  let context = featureAbility.getContext();
  context.requestPermissionsFromUser(requestPermissions, 1, (data) => {
    console.info("data:" + JSON.stringify(data));
    console.info("data requestCode:" + data.requestCode);
    console.info("data permissions:" + data.permissions);
    console.info("data authResults:" + data.authResults);
  });
  console.info('RequestPermission end');
}

function WriteData(key: string, value: string) {
  try {
    kvStore.put(key, value, function (err) {
      if (err != undefined) {
        console.log("xxxxx put err: " + JSON.stringify(err));
        return;
      }
      console.info("xxxxx dataChange put1 sucess");
    });
  } catch (e) {
    console.log("An unexpected error occurred. Error: " + e);
  }
}

@Extend(Button) function ButtonStyle () {
  .fontSize(25)
  .fontWeight(FontWeight.Bold)
  .fontStyle(FontStyle.Italic)
  .type(ButtonType.Capsule)
  .backgroundColor('#0D9FFB')
}

@Entry
@Component
struct Second {
  @State ReadTemperatureC: number = 0 ;
  @State ReadHumidity: number = 0 ;
  @State ReadTemperatureF: number = 0 ;
  @State message: string = '使用教程:一.启动/重启 二.滑动选择后按下模式选择 三.获取'
  @State pwm_Angle:number = 0;
  private select1: number = 3
  private Mps: string[] = ['Mps:0.5Hz', 'Mps:1Hz', 'Mps:2Hz', 'Mps:4Hz', 'Mps:10Hz']
  private select2: number = 1
  private Repeatability: string[] = ['刷新率:低', '刷新率:中', '刷新率:高']
  private intervalID: number = 0 ;
  tem: string = "Tem";
  tem_value: number = 0;
  hum: string = "Hum";
  hum_value: number = 0;
  temf: string = "Temf";
  temf_value: number = 0;
  Angle_key: string = "Pwm";


  aboutToAppear() {
    CreateKVManager();
    setTimeout(function () {
      GetKVStore();
    }, 10);
    RequestPermission();
  }

  build() {
    Column() {
      Row() {
        Column() {
          Row() {
            TextPicker({ range: this.Mps, selected: this.select1 })
              .onChange((value: string, index: number) => {
                console.info('Picker item changed, value: ' + value + ', index: ' + index)
                this.select1 = index;
              })
              .width('50%')
              .height('100%')
              .margin({
                bottom: 10
              })

            TextPicker({ range: this.Repeatability, selected: this.select2 })
              .onChange((value: string, index: number) => {
                console.info('Picker item changed, value: ' + value + ', index: ' + index)
                this.select2 = index;
              })

              .margin({
                bottom: 10
              })
              .width('50%')
              .height('100%')
          }
          .height('50%')
        }
        .width('50%')

        Column() {
          Text("温度：" + this.ReadTemperatureC.toFixed(4) + "°C\n" +
          "湿度：" + this.ReadHumidity.toFixed(4) + "%RH\n" +
          "温度：" + this.ReadTemperatureF.toFixed(4) + "°F\n" +
          "舵机角度：" + this.pwm_Angle.toFixed(2) + "°")
            .fontSize(35)
            .fontWeight(FontWeight.Bold)
            .textAlign(TextAlign.Start)
        }
        .width('50%')
      }
      .height('60%')

      Row() {
        Text(this.message)
          .fontSize(25)
          .fontWeight(FontWeight.Bold)
          .margin({
            bottom: 25
          })
      }

      Row() {
        Button('启动/重启')
          .ButtonStyle()
          .margin({
            right: 40
          })
          .width('20%')
          .height('10%')
          .onClick(() => {
            if (i2cnapidemo.ReadTemperatureC() <= 800) {
              i2cnapidemo.SoftReset();
              clearInterval(this.intervalID);
              prompt.showToast({
                message: "启动/重启成功", // 显示文本
                duration: 500, // 显示时长
                bottom: 100 // 距离底部的距离
              })
            }
            else {
              prompt.showToast({
                message: "启动失败，请检查连线", // 显示文本
                duration: 1000, // 显示时长
                bottom: 100 // 距离底部的距离
              })
            }
          })

        Button('模式选择')
          .ButtonStyle()
          .margin({
            right: 70
          })
          .width('20%')
          .height('10%')
          .onClick(() => {
            if (i2cnapidemo.ReadTemperatureC() <= 800) {
              i2cnapidemo.ModeSet(this.select1, this.select2);
              prompt.showToast({
                message: "模式选择成功", // 显示文本
                duration: 500, // 显示时长
                bottom: 100 // 距离底部的距离
              })
            }
            else {
              prompt.showToast({
                message: "模式选择失败，请检查连线", // 显示文本
                duration: 1000, // 显示时长
                bottom: 100 // 距离底部的距离
              })
            }
          })

        Button('获取')
          .ButtonStyle()
          .margin({
            right: 40
          })
          .width('16%')
          .height('10%')
          .onClick(() => {
            this.intervalID = setInterval(() => {
              i2cnapidemo.ReadData();
              this.ReadTemperatureC = i2cnapidemo.ReadTemperatureC();
              this.tem_value = this.ReadTemperatureC;
              WriteData(this.tem, this.tem_value);
              this.ReadHumidity = i2cnapidemo.ReadHumidity();
              this.hum_value = this.ReadHumidity;
              WriteData(this.hum, this.hum_value);
              this.ReadTemperatureF = i2cnapidemo.ReadTemperatureF();
              this.temf_value = this.ReadTemperatureF;
              WriteData(this.temf, this.temf_value);
              kvStore.get(this.Angle_key).then((result)=>{
                this.pwm_Angle=result;
              });
              pwmtest.setPwmStatus(1, this.pwm_Angle);
            },100);
          })

        Button("订阅")
          .ButtonStyle()
          .width('16%')
          .height('10%')
          .onClick(() => {
            Subscribe();
          })
      }
    }
  }
}
