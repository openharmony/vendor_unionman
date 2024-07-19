import router from '@ohos.router';
import hilog from '@ohos.hilog';
import testNapi from 'libentry.so';
import pwmtest from '@ohos.pwmtest'
import { KvStoreModel } from '../model/KvStoreModel';
import Logger from '../util/Logger';

const PWM = 'pwm'
let kvStoreModel: KvStoreModel = new KvStoreModel() // 实例化键值存储模型
const TAG = 'camera'

@Entry
@Component
struct camera {
  @State text: string = '0度'
  @State index: number = 0
  @State@Watch('angleChange') angle: number = 0  // 使用 @State 来跟踪角度状态

  angleChange() {
    console.log(TAG,'anglechange:'+this.angle);
    pwmtest.setPwmStatus(2, this.angle);
  }
  updateAngle(text: string) {
    let angle = 0;
    switch (text) {
      case '0度':
        angle = 0;
        break;
      case '45度':
        angle = 45;
        break;
      case '90度':
        angle = 90;
        break;
      case '135度':
        angle = 135;
        break;
    }
    this.angle = angle;  // 更新角度状态
    kvStoreModel.put(PWM,this.angle.toString());
  }
  aboutToAppear() {
    kvStoreModel.setOnMessageReceivedListener(PWM , (value) => {
      Logger.info(TAG, `PWM${value}`);  // 记录日志，显示接收到NOTES_CHANGE消息的值
          this.angle = JSON.parse(value);//从数据库中获得新变化的数据
        })
  }

  build() {
    Column() {
      Text('angle:'+this.angle)
      Row() {
        Image($r('app.media.back'))
          .height(32)
          .width(32)
        Text('返回')
          .fontColor('#858585')
          .fontSize(20)
          .fontWeight(FontWeight.Medium)
          .margin({top: 4, left: 5})
      }
      .margin({top: 60, left: 20})
      .onClick(() => {
        router.back()
      })
      Row() {
        Image($r('app.media.camera1'))
          .width(150)
          .height(150)
          .margin({top: 60, left: 80})
        Row() {
          Text('角度：')
            .fontColor('#474747')
            .fontSize(24)
            .fontWeight(FontWeight.Medium)
          Select([{ value: '0度'},
            { value: '45度'},
            { value: '90度'},
            { value: '135度'}])
            .width(120)
            .selected(this.index)
            .value(this.text)
            .font({ size: 16, weight: FontWeight.Medium })
            .fontColor('#474747')
            .selectedOptionBgColor('#FAFAFA')
            .selectedOptionFont({ size: 16, weight: 500 })
            .selectedOptionFontColor('#4ab388')
            .optionFont({ size: 16, weight: 500 })
            .optionFontColor('#474747')
            .onSelect((index:number, text?: string | undefined)=>{
              console.info('Select:' + index)
              this.index = index;
              if(text){
                this.text = text;
                this.updateAngle(text);
              }
            })
        }
        .margin({top: 70, left: 40})
        Image($r('app.media.light'))// 使用动画实现旋转效果
          .width(100)
          .height(100)
          .margin({top: 60, left: 70})
          .rotate({ x: 0, y: 0, z: 1, angle: this.angle })  // 使用 rotate 实现旋转
      }
      .alignItems(VerticalAlign.Center)
    }
    .alignItems(HorizontalAlign.Start)
    .height('100%')
    .width('100%')
    .backgroundColor('#F2F2F2')
  }
}
