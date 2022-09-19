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
import distributedData from '@ohos.data.distributedData';
import prompt from '@ohos.prompt';
import { RequestPermission, CreatePullWork } from './pullwork'

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
  @State message: string = '使用教程:一.ON/OFF（舵机） 二.拉起APP 三.获取 四.订阅'
  private intervalID: number = 0 ;
  tem: string = "Tem";
  hum: string = "Hum";
  temf: string = "Temf";
  @State pwm_Angle: number = 0;
  @State power: string = 'ON'
  @State start: boolean = true
  Angle_key: string = "Pwm";


  aboutToAppear() {
    CreateKVManager();
    setTimeout(function () {
      GetKVStore();
    }, 10);
    RequestPermission('com.example.myapplication');
  }

  build() {
    Column() {
      Row() {
        Column() {
          // 根据设置的角度，模拟舵机转动角度
          Gauge({ value: 0, min: -90, max: 90 })
            .startAngle(-90)
            .endAngle(90)
            .colors([[Color.Gray, 1]])
            .strokeWidth(30)
            .value(this.pwm_Angle - 90)
            .height('60%')
            .width('100%')
            .margin({ top: 20 })

          // 设置舵机的转动角度
          Slider({
            value: this.pwm_Angle,
            min: 0,
            max: 180,
            step: 1,
            style: SliderStyle.OutSet
          })
            .width('60%')
            .blockColor(Color.Gray)
            .trackColor(Color.Gray)
            .selectedColor(0x317aff)
            .showSteps(true)
            .showTips(true)
            .onChange((value: number) => {
              // 根据开关的标识位，来选择是否转动舵机
              if (this.start) {
                this.pwm_Angle = value
                WriteData(this.Angle_key, this.pwm_Angle);
                console.info("onChange pwm_Angle = " + this.pwm_Angle)
              }
            })

        }
        .width('50%')

        Column() {
          Text("温度：" + this.ReadTemperatureC.toFixed(4) + "°C\n" +
          "湿度：" + this.ReadHumidity.toFixed(4) + "%RH\n" +
          "温度：" + this.ReadTemperatureF.toFixed(4) + "°F")
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
        Button(this.power)
          .ButtonStyle()
          .margin({
            right: 40
          })
          .width('20%')
          .height('10%')
          .onClick(() => {
            // 开关标记为进行判断
            if (this.start) {
              prompt.showToast({
                message: "关闭舵机", // 显示文本
              })
              this.power = "OFF"
            } else {
              prompt.showToast({
                message: "打开舵机", // 显示文本
              })
              this.power = "ON"
            }
            // 开关标记位取反
            this.start = !this.start
          })

        // 拉起另一块板子的app
        Button('拉起')
          .ButtonStyle()
          .margin({
            right: 70
          })
          .width('20%')
          .height('10%')
          .onClick(() => {
            CreatePullWork("com.example.myapplication", "com.example.myapplication", "com.example.entry.MainAbility");
          })

        // 从数据库中获取对应的值
        Button('获取')
          .ButtonStyle()
          .margin({
            right: 40
          })
          .width('16%')
          .height('10%')
          .onClick(() => {
            this.intervalID = setInterval(() => {
              kvStore.get(this.tem).then((result)=>{
                this.ReadTemperatureC=result;
              });
              kvStore.get(this.hum).then((result)=>{
                this.ReadHumidity=result;
              });
              kvStore.get(this.temf).then((result)=>{
                this.ReadTemperatureF=result;
              });
              console.info("xxxxxx "+this.ReadTemperatureC);
            },1000);

          })

        // 订阅数据库的数据变化
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
