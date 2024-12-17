import DistributedData from '@ohos.data.distributedKVStore';
import common from '@ohos.app.ability.common';
import { BusinessError } from '@ohos.base';
import Prompt from '@system.prompt';
import { RequestPermission, CreateWork } from './pullwork'
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
  @State message: string = '使用教程:一.ON/OFF（舵机） 二.拉起APP 三.获取 四.订阅'
  private intervalID: number = 0;
  tem: string = "Tem";
  hum: string = "Hum";
  temf: string = "Temf";
  @State pwm_Angle: number = 0;
  @State power: string = 'ON'
  @State start: boolean = true
  Angle_key: string = "Pwm";

  aboutToAppear() {
    CreateKVManager();
    setTimeout(() => {
      GetKVStore();
    }, 10);
    RequestPermission('com.example.distributed_integrated_application');
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
                WriteData(this.Angle_key, this.pwm_Angle.toString());
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
            CreateWork("com.example.distributed_integrated_application", "com.example.distributed_integrated_application", "com.example.control_application.Peripheral_applicationAbility");
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
              kvStore.get(this.tem).then((result) => {
                this.ReadTemperatureC = result as number;
              });
              kvStore.get(this.hum).then((result) => {
                this.ReadHumidity = result as number;
              });
              kvStore.get(this.temf).then((result) => {
                this.ReadTemperatureF = result as number;
              });
              console.info("xxxxxx " + this.ReadTemperatureC);
            }, 1000);

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
