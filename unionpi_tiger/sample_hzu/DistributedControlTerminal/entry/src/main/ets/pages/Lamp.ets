import router from '@ohos.router';
import promptAction from '@ohos.promptAction';
import hilog from '@ohos.hilog';
import testNapi from 'libentry.so';
import adc_napi from '@ohos.adc_napi';
import { KvStoreModel } from '../model/KvStoreModel';
import Logger from '../util/Logger';
const ADC = 'adc'
const TAG ='Lamp'
let kvStoreModel: KvStoreModel = new KvStoreModel() // 实例化键值存储模型

@Entry
@Component
struct Lamp {
  @State localButton:promptAction.ShowToastOptions = {'message': '本地已连接'}
  @State remoteButton:promptAction.ShowToastOptions = {'message': '远程已连接'}
  @State adcvalue:number = 0;
  @State getRemoteAdc:number = 0;
  @State status:string = ' '
  @State env:string = ' '
  @State local: number = 0;
  @State remote: number = 0;
  intervalID:number=0;

  getAdc() {
    adc_napi.sync_get_permission();
    if(this.getRemoteAdc >= 3000) {
      this.env = '昏暗'
      this.status = '开启'
    } else {
      this.env = '正常'
      this.status = '关闭'
    }
  }
  aboutToAppear() {
    kvStoreModel.setOnMessageReceivedListener(ADC , (value) => {
      Logger.info(TAG, `ADC${value}`);  // 记录日志，显示接收到NOTES_CHANGE消息的值
    })
  }
  startWork() {
    this.intervalID = setInterval(() => {
      if (this.local == 1) {
        this.adcvalue = adc_napi.sync_get_adc_value()
        kvStoreModel.put(ADC,this.adcvalue.toString())
      }
      kvStoreModel.get(ADC, (value) => {
        this.getRemoteAdc = JSON.parse(value);
      })
      this.getAdc();
    }, 3000)
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
          .margin({top: 4, left: 5})
        Text('本地')
          .onClick(() => {
            if (this.intervalID) {
              clearInterval(this.intervalID)
            }
            this.local = 1;
            promptAction.showToast(this.localButton)
            this.startWork();
          })
          .fontColor('#30ab76')
          .fontSize(20)
          .fontWeight(FontWeight.Medium)
          .margin({ top: 4, left: 100 })
        Text('远程')
          .onClick(() => {
            if (this.intervalID) {
              clearInterval(this.intervalID)
            }
            this.remote = 1;
            promptAction.showToast(this.remoteButton)
            this.startWork();
          })
          .fontColor('#30ab76')
          .fontSize(20)
          .fontWeight(FontWeight.Medium)
          .margin({ top: 4, left: 10 })
      }
      .margin({top: 30, left: 20})
      .onClick(() => {
        clearInterval(this.intervalID);
        this.env = ' '
        this.status = ' '
        router.back()
      })
      Row() {
        Image($r('app.media.lamp1'))
          .width(200)
          .height(200)
          .margin({top: -10, left: 30})
        Column() {
          Row() {
            Text('当前环境')
              .fontColor('#474747')
              .fontSize(20)
              .fontWeight(FontWeight.Medium)
            Text('Current Environment')
              .fontColor('#a9dfc9')
              .fontSize(14)
              .fontWeight(FontWeight.Medium)
              .margin({left: 10})
          }
          Row() {
            Text(this.env)
              .fontColor('#474747')
              .fontSize(36)
              .fontWeight(FontWeight.Medium)
          }
          .backgroundColor('#FAFAFA')
          .borderRadius(12)
          .height(105)
          .width(165)
          .justifyContent(FlexAlign.Center)
          Row() {
            Text('灯光状态')
              .fontColor('#474747')
              .fontSize(20)
              .fontWeight(FontWeight.Medium)
            Text('Lamp Status')
              .fontColor('#a9dfc9')
              .fontSize(14)
              .fontWeight(FontWeight.Medium)
              .margin({left: 10})
          }
          .margin({top: 20, right: 60})
          Row() {
            Text(this.status)
              .fontColor('#474747')
              .fontSize(36)
              .fontWeight(FontWeight.Medium)
          }
          .backgroundColor('#FAFAFA')
          .borderRadius(12)
          .height(105)
          .width(165)
          .justifyContent(FlexAlign.Center)
        }
        .margin({top: 0, left: 150})
      }
      .alignItems(VerticalAlign.Center)
    }
    .alignItems(HorizontalAlign.Start)
    .height('100%')
    .width('100%')
    .backgroundColor('#F2F2F2')
  }
}