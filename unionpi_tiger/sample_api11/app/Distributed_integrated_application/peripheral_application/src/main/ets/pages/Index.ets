import pwmtest from '@ohos.pwmtest';
import i2cnapidemo from '@ohos.i2cnapidemo';
import DistributedData from '@ohos.data.distributedKVStore';
import common from '@ohos.app.ability.common';
import { BusinessError } from '@ohos.base';
import Prompt from '@system.prompt';
import { RequestPermission } from './pullwork'
import prompt from '@ohos.prompt';

let kvManager: DistributedData.KVManager;
let kvStore: DistributedData.SingleKVStore;

function CreateKVManager() {
  try {
    let context = getContext() as common.BaseContext;
    console.info('CreateKVManager: Obtaining context:', context);

    const kvManagerConfig: DistributedData.KVManagerConfig = {
      bundleName: 'com.example.distributed_integrated_application',
      context: context
    };
    console.info('CreateKVManager: KV Manager configuration:', kvManagerConfig);

    try {
      kvManager = DistributedData.createKVManager(kvManagerConfig);
      console.info('CreateKVManager: KV Manager created successfully.');
    } catch (err) {
      console.error('CreateKVManager: Failed to create KV Manager', err);
    }
  } catch (err) {
    console.error('CreateKVManager: Error occurred during manager creation', err);
  }
}

function GetKVStore() {
  try {
    const options: DistributedData.Options = {
      createIfMissing: true,
      encrypt: false,
      backup: false,
      autoSync: true,
      kvStoreType: DistributedData.KVStoreType.SINGLE_VERSION,
      securityLevel: DistributedData.SecurityLevel.S1
    };
    console.info('GetKVStore: KV Store options:', options);

    kvManager.getKVStore('storeId0974', options, (err: BusinessError, store: DistributedData.SingleKVStore) => {
      if (err) {
        console.error('GetKVStore: Failed to get KV Store', err);
        return;
      }
      console.info('GetKVStore: KV Store retrieved successfully.');
      kvStore = store;
    });
  } catch (err) {
    console.error('GetKVStore: Error occurred while fetching KV Store', err);
  }
}

function Subscribe() {
  try {
    if (!kvStore) {
      console.error('Subscribe: KV Store is not initialized.');
      return;
    }

    kvStore.on('dataChange', DistributedData.SubscribeType.SUBSCRIBE_TYPE_ALL, (data) => {
      console.info('Subscribe: Data change detected. Data:', data);
    });
    console.info('Subscribe: Data subscription established.');

    Prompt.showToast({
      message: '数据库初始化成功'
    });
    console.info('Subscribe: Toast message shown - Database initialization successful.');
  } catch (err) {
    console.error('Subscribe: Error occurred during subscription', err);
  }
}

function WriteData(key: string, value: string) {
  try {
    if (!kvStore) {
      console.error('WriteData: KV Store is not initialized.');
      return;
    }

    kvStore.put(key, value, (err: BusinessError) => {
      if (err !== undefined) {
        console.error('WriteData: Failed to write data. Key:', key, 'Value:', value, 'Error:', err);
        return;
      }
      console.info('WriteData: Data written successfully. Key:', key, 'Value:', value);
    });
  } catch (e) {
    console.error('WriteData: Error occurred while writing data. Key:', key, 'Value:', value, 'Error:', e);
  }
}

@Extend(Button)
function ButtonStyle() {
  .fontSize(25)
  .fontWeight(FontWeight.Bold)
  .fontStyle(FontStyle.Italic)
  .type(ButtonType.Capsule)
  .backgroundColor('#0D9FFB')
}


@Entry
@Component
struct Second {
  @State ReadTemperatureC: number = 0;
  @State ReadHumidity: number = 0;
  @State ReadTemperatureF: number = 0;
  @State message: string = '使用教程:一.启动/重启 二.滑动选择后按下模式选择 三.获取'
  @State pwm_Angle: number = 0;
  private select1: number = 3
  private Mps: string[] = ['Mps:0.5Hz', 'Mps:1Hz', 'Mps:2Hz', 'Mps:4Hz', 'Mps:10Hz']
  private select2: number = 1
  private Repeatability: string[] = ['刷新率:低', '刷新率:中', '刷新率:高']
  private intervalID: number = 0;
  tem: string = "Tem";
  tem_value: number = 0;
  hum: string = "Hum";
  hum_value: number = 0;
  temf: string = "Temf";
  temf_value: number = 0;
  Angle_key: string = "Pwm";

  aboutToAppear() {
    CreateKVManager();
    setTimeout(() => {
      GetKVStore();
    }, 10);
    RequestPermission('');
  }

  build() {
    Column() {
      Row() {
        Column() {
          Row() {
            TextPicker({ range: this.Mps, selected: this.select1 })
              .onChange((value: string[] | string, index: number | number[]) => {
                console.info('Picker item changed, value: ' + value + ', index: ' + index)
                this.select1 = index as number;
              })
              .width('50%')
              .height('100%')
              .margin({
                bottom: 10
              })

            TextPicker({ range: this.Repeatability, selected: this.select2 })
              .onChange((value: string[] | string, index: number | number[]) => {
                console.info('Picker item changed, value: ' + value + ', index: ' + index)
                this.select2 = index as number;
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
              WriteData(this.tem, this.tem_value.toString());
              this.ReadHumidity = i2cnapidemo.ReadHumidity();
              this.hum_value = this.ReadHumidity;
              WriteData(this.hum, this.hum_value.toString());
              this.ReadTemperatureF = i2cnapidemo.ReadTemperatureF();
              this.temf_value = this.ReadTemperatureF;
              WriteData(this.temf, this.temf_value.toString());
              kvStore.get(this.Angle_key).then((result) => {
                this.pwm_Angle = result as number;
              });
              pwmtest.setPwmStatus(1, this.pwm_Angle);
            }, 100);
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
