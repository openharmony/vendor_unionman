import router from '@ohos.router';
import hilog from '@ohos.hilog';
import testNapi from 'libentry.so'
import { KvStoreModel } from '../model/KvStoreModel';
import Logger from '../util/Logger';
import i2cnapidemo from '@ohos.i2cnapidemo';
import constant from '@ohos.bluetooth.constant';
import promptAction from '@ohos.promptAction';

const TAG = 'Tem_Hum'
const TEMC = "tem_c"
const TEMF = 'tem_f'
const HUM = 'hum'
let kvStoreModel: KvStoreModel = new KvStoreModel() // 实例化键值存储模型

@Entry
@Component
struct Tem_Hum {
  @State tem_c: number = 0;
  @State tem_f: number = 0;
  @State hum: number = 0;
  @State rtem_c: number = 0;
  @State rtem_f: number = 0;
  @State rhum: number = 0;
  @State tem_Status: string = ' ';
  @State hum_Status: string = ' ';
  @State local: number = 0;
  @State remote: number = 0;
  intervalID: number = 0;
  @State BOn: promptAction.ShowToastOptions = { 'message': '本地已连接' }
  @State BOff: promptAction.ShowToastOptions = { 'message': '远程已连接' }

  getValue() {
    if (this.rtem_c >= 50) {
      this.tem_Status = '警告：室温过高，谨防火灾'
    }
    if (this.rhum >= 80) {
      this.hum_Status = '警告：湿度过高，谨防短路'
    }
    if (this.rhum <= 40) {
      this.hum_Status = '警告：空气干燥，谨防火灾'
    }
    if (this.rtem_c < 50) {
      this.tem_Status = ' '
    }
    if (this.rhum < 80 && this.rhum > 40) {
      this.hum_Status = ' '
    }
  }

  startWork() {
    this.intervalID = setInterval(() => {
      if (this.local == 1) {
        i2cnapidemo.SoftReset();
        i2cnapidemo.ModeSet(3, 1);
        i2cnapidemo.ReadData();
        this.tem_c = i2cnapidemo.ReadTemperatureC();
        this.hum = i2cnapidemo.ReadHumidity();
        this.tem_f = i2cnapidemo.ReadTemperatureF();
        kvStoreModel.put(TEMC, this.tem_c.toString())
        kvStoreModel.put(TEMF, this.tem_f.toString())
        kvStoreModel.put(HUM, this.hum.toString())
      }
      kvStoreModel.get('tem_c', (value) => {
        this.rtem_c = JSON.parse(value); //从数据库中获得新变化的数据
      });
      kvStoreModel.get('tem_f', (value) => {
        this.rtem_f = JSON.parse(value); //从数据库中获得新变化的数据
      });
      kvStoreModel.get('hum', (value) => {
        this.rhum = JSON.parse(value); //从数据库中获得新变化的数据
      });
      this.getValue();
    }, 5000)
  }

  aboutToAppear() {
    kvStoreModel.setOnMessageReceivedListener(TAG, (value) => {
      // 记录日志，显示接收到NOTES_CHANGE消息的值
      Logger.info(TAG, `Tem_Hum${value}`);
    })
  }

  build() {
    Column() {
      Row() {
        Image($r('app.media.back'))
          .height(32)
          .width(32)
        Text('返回')
          .fontColor('#858585')
          .fontSize(20)
          .fontWeight(FontWeight.Medium)
          .margin({ top: 4, left: 5 })
          .onClick(() => {
            clearInterval(this.intervalID);
            router.back()
          })
        Text("本地")
          .onClick(() => {
            if (this.intervalID) {
              clearInterval(this.intervalID)
            }
            this.local = 1;
            promptAction.showToast(this.BOn)
            this.startWork();
          })
          .fontColor('#30ab76')
          .fontSize(20)
          .fontWeight(FontWeight.Medium)
          .margin({ top: 4, left: 100 })
        Text("远程")
          .onClick(() => {
            if (this.intervalID) {
              clearInterval(this.intervalID)
            }
            this.remote = 1;
            promptAction.showToast(this.BOff)
            this.startWork();
          })
          .fontColor('#30ab76')
          .fontSize(20)
          .fontWeight(FontWeight.Medium)
          .margin({ top: 4, left: 10 })
      }
      .margin({ top: 60, left: 20 })

      Row() {
        Column() {
          Row() {
            Text('温度')
              .fontColor('#474747')
              .fontSize(20)
              .fontWeight(FontWeight.Medium)
            Text('Temperature')
              .fontColor('#a9dfc9')
              .fontSize(14)
              .fontWeight(FontWeight.Medium)
              .margin({ left: 10 })
          }
          .margin({ top: 50, left: 20 })

          Column() {
            Row() {
              Text(this.rtem_c.toFixed(2))
                .fontColor('#474747')
                .fontSize(24)
                .fontWeight(FontWeight.Medium)
              Image($r('app.media.tem_c'))
                .width(30)
                .height(30)
                .margin({ top: -8, left: 20 })
            }
            .margin({ top: 20 })

            Row() {
              Text(this.rtem_f.toFixed(2))
                .fontColor('#474747')
                .fontSize(24)
                .fontWeight(FontWeight.Medium)
              Image($r('app.media.tem_f'))
                .width(35)
                .height(35)
                .margin({ top: -8, left: 20 })
            }
            .margin({ top: 25 })
          }
          .backgroundColor('#FAFAFA')
          .borderRadius(10)
          .height(121)
          .width(198)
          .margin({ left: 17, top: 10 })
          .alignItems(HorizontalAlign.Center)
        }
        .alignItems(HorizontalAlign.Start)

        Image($r('app.media.tem'))
          .height(200)
          .width(200)
          .margin({ left: -30, bottom: -30 })
        Row() {
          Image($r('app.media.hum'))
            .height(200)
            .width(200)
            .margin({ left: -30, bottom: -30 })
          Column() {
            Row() {
              Text('湿度')
                .fontColor('#474747')
                .fontSize(20)
                .fontWeight(FontWeight.Medium)
              Text('Humidity')
                .fontColor('#a9dfc9')
                .fontSize(14)
                .fontWeight(FontWeight.Medium)
                .margin({ left: 10 })
            }
            .margin({ left: -10 })
            Column() {
              Row() {
                Text(this.rhum.toFixed(2))
                  .fontColor('#474747')
                  .fontSize(30)
                  .fontWeight(FontWeight.Medium)
                Image($r('app.media.percent'))
                  .width(35)
                  .height(35)
                  .margin({ top: -8, left: 20 })
              }
              .margin({ top: 20 })
            }
            .backgroundColor('#FAFAFA')
            .borderRadius(10)
            .height(121)
            .width(198)
            .margin({ left: -20, top: 10 })
            .alignItems(HorizontalAlign.Center)
            .justifyContent(FlexAlign.Center)
          }
          .alignItems(HorizontalAlign.Start)
        }
      }
      Row() {
        Text(this.tem_Status)
          .fontColor('#FF0000')
          .fontSize(20)
          .fontWeight(FontWeight.Medium)
        Text(this.hum_Status)
          .fontColor('#FF0000')
          .fontSize(20)
          .fontWeight(FontWeight.Medium)
          .margin({ left: 470 })
      }
      .margin({ top: 20, left: 50 })
    }
    .alignItems(HorizontalAlign.Start)
    .height('100%')
    .width('100%')
    .backgroundColor('#F2F2F2')
  }
}