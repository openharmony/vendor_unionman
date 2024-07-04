import router from '@ohos.router';
import prompt from '@ohos.prompt';
import gpioled from '@ohos.gpioled';
import pwmtest from '@ohos.pwmtest';
import promptAction from '@ohos.promptAction';
@Entry
@Component
struct fmq {
  @State
  pin1: number = 381;
  pin2: number = 382;
  pex: number = 2;
  pwmAngle: number = 0;
  intervalID: number = 0;
  I: number = 0;
  select1: number = 0;
  pattern: string[] = ['风扇', '吸尘'];
  select2: number = 0;
  gears: string[] = ['一档', '二档', '三档'];
  gearss: number[] = [10, 45, 90];

  build() {
    Stack() {
      // 背景
      Image($r('app.media.bj1'));

      Row() {
        Text('电风扇')
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .margin({ top: -420 });
      }

      // 主体页面
      Row() {
        Row() {
          // TextPicker组件选择pattern
          TextPicker({ range: this.pattern, selected: this.select1 })
            .onChange((value: string, index: number) => {
              console.info('Picker item changed, value: ' + value + ', index: ' + index);
              this.select1 = index;
            })
            .width('50%')
            .height('50%');

          // TextPicker组件选择Gear
          TextPicker({ range: this.gears, selected: this.select2 })
            .onChange((value: string, index: number) => {
              console.info('Picker item changed, value: ' + value + ', index: ' + index);
              this.select2 = index;
            })
            .width('50%')
            .height('50%');
        }
        .width('50%')
        .height('100%');

        Column() {
          Button() {
            Image($r('app.media.x1'));
          }
          .width(200)
          .height(200)
          .backgroundColor('rgba(0, 0, 0, 0)')
          .onClick(() => {
            if (this.select1 == 0) {
              gpioled.setLedStatus(this.pin2, gpioled.LED_OFF);
              gpioled.setLedStatus(this.pin1, gpioled.LED_ON);
              this.pwmAngle = this.gearss[this.select2];
              console.info('onChange pwmAngle = ' + this.pwmAngle);
              pwmtest.setPwmStatus(this.pex, this.pwmAngle);
            } else {
              gpioled.setLedStatus(this.pin1, gpioled.LED_OFF);
              gpioled.setLedStatus(this.pin2, gpioled.LED_ON);
              this.pwmAngle = this.gearss[this.select2];
              console.info('onChange pwmAngle = ' + this.pwmAngle);
              pwmtest.setPwmStatus(this.pex, this.pwmAngle);
            }
          });

          Button() {
            Image($r('app.media.x2'));
          }
          .width(200)
          .height(200)
          .backgroundColor('rgba(0, 0, 0, 0)')
          .onClick(() => {
            if (this.select1 == 0) {
              gpioled.setLedStatus(this.pin2, gpioled.LED_ON);
            } else {
              gpioled.setLedStatus(this.pin1, gpioled.LED_ON);
            }
          });
        }
        .width('50%')
        .height('70%');
      }
      .width('100%')
      .height('100%');
    }
    .width('100%')
    .height('100%');
  }
}