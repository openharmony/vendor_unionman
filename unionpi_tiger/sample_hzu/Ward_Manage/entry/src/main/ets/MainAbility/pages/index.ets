// @ts-nocheck
import { SettingDetails } from './common/SettingDetails';
import router from '@ohos.router';
import { HomeDetails } from './common/HomeDetails';
// @ts-ignore
import i2cnapidemo from '@ohos.i2cnapidemo';
import gpioled from '@ohos.gpioled';
import pwmtest from '@ohos.pwmtest';
import prompt from '@ohos.prompt';

@Entry
@Component
struct Index {
  @State ReadTemperatureC: number = 0;
  @State ReadHumidity: number = 0;
  @State ReadTemperatureF: number = 0;
  @State pwm_Angle: number = 0;
  @State time: string = "";
  data: {
    pin: 382,
    status: Boolean
  }

  // 构造函数中更新时间
  constructor() {
    this.updateTime();
    // 每分钟更新一次时间
    setInterval(() => {
      this.updateTime();
    }, 60000);
  }

  onInit() {
    this.syncButtonStatus()
  }

  syncButtonStatus() {
    gpioled.getLedStatus(this.pin).then((result) => {
      this.status = Boolean(result)
    })
  }

  changeGpio(msg) {
    this.pin = Number(msg.newValue)
    this.syncButtonStatus()
    console.info(`Select:${msg.newValue}`);
    console.info(`Select:${this.status}`);
  }

  switchChange(e) {
    if (e.checked) {
      this.addLedEffect()
    } else {
      this.removeLedEffect()
    }
  }

  onShow() {
    console.log('xxx')
    i2cnapidemo.SoftReset();
    i2cnapidemo.ModeSet(3, 3);
    this.updateSensorData();
  }

  updateSensorData() {
    i2cnapidemo.ReadData();
    this.ReadTemperatureC = i2cnapidemo.ReadTemperatureC();
    this.ReadHumidity = i2cnapidemo.ReadHumidity();
    this.ReadTemperatureF = i2cnapidemo.ReadTemperatureF();

    // 定期更新数据

    setTimeout(() => {
      this.updateSensorData();
    }, 2000); // 每5秒更新一次数据
  }

  build() {

    Column() {

      Row() {

        Image(("common/images/cloudy.png"))
          .objectFit(ImageFit.Contain)
          .width(150)
          .height(150)
          .borderRadius(21)
          .position({ x: 10, y: 20 })


        Column() {
          Text('2024年-6月')
            .fontSize(20).opacity(0.4)
            .fontWeight(FontWeight.Bold)
          Text('')
            .fontSize(50).opacity(0.6)
            .fontWeight(FontWeight.Bold)
            .position({ x: 40, y: 60 })
          Text(this.time)
            .fontSize(50).opacity(0.6)
            .fontWeight(FontWeight.Bold)
            .position({ x: -55, y: 160 })
        }.position({ x: 160, y: 20 })


        Text('多云',)
          .fontSize(30)
          .fontWeight(FontWeight.Bold)
          .position({ x: 70, y: 100 })

        Column() {
          Text(this.ReadTemperatureC.toFixed(1) + "°")
            .fontSize(40).opacity(0.5)
            .fontWeight(FontWeight.Bold)
            .margin({ top: 15 })
          Text('温度',)
            .fontSize(25).opacity(0.4)
        }
        .position({ x: 30, y: 250 })
        .backgroundColor('#A9A9A9')
        .borderRadius('30')
        .height('120')
        .width('120')
        .onClick(() => {
          i2cnapidemo.ReadData();
          this.ReadTemperatureC = i2cnapidemo.ReadTemperatureC();
          this.ReadHumidity = i2cnapidemo.ReadHumidity();
          this.ReadTemperatureF = i2cnapidemo.ReadTemperatureF();
        })

        Column() {
          Text(this.ReadHumidity.toFixed(0) + '%',)
            .fontSize(40).opacity(0.5)
            .fontWeight(FontWeight.Bold)
            .margin({ top: 15 })
          Text('湿度',)
            .fontSize(25).opacity(0.4)
        }
        .position({ x: 190, y: 250 })
        .backgroundColor('#A9A9A9')
        .borderRadius('30')
        .height('120')
        .width('120')

      }
      .position({ x: 10, y: 25 })
      .backgroundColor('#D3D3D3')
      .borderRadius('30')
      .height('400')
      .width('325')

      Row() {
        Column() {
          Toggle({ type: ToggleType.Button }) {
            Text('顶灯',)
              .fontSize(30).opacity(1)

          }
          .size({ width: 100, height: 100 })
          .borderStyle(BorderStyle.Solid)
          .onChange((isOn: bool) => {
            if (isOn) {
              gpioled.setLedStatus(382, gpioled.LED_ON)
              i2cnapidemo.ReadData();
            }
            else {
              gpioled.setLedStatus(382, gpioled.LED_OFF)
            }
          })

          Toggle({ type: ToggleType.Button }) {
            Text('床灯',)
              .fontSize(30).opacity(1)
          }

          .size({ width: 100, height: 100 })
          .borderStyle(BorderStyle.Solid)
          .opacity(1)
          .onChange((isOn: bool) => {
            if (isOn) {
              gpioled.setLedStatus(387, gpioled.LED_ON)
            }
            else {
              gpioled.setLedStatus(387, gpioled.LED_OFF)
            }
          })

          Toggle({ type: ToggleType.Button }) {
            Text('床前灯',)
              .fontSize(25).opacity(1)
          }
          .size({ width: 100, height: 100 })
          .borderStyle(BorderStyle.Solid)
          .onChange((isOn: bool) => {
            if (isOn) {
              gpioled.setLedStatus(388, gpioled.LED_ON)
            }
            else {
              gpioled.setLedStatus(388, gpioled.LED_OFF)
            }
          })

          Toggle({ type: ToggleType.Button }) {
            Text('台灯',)
              .fontSize(30).opacity(1)
          }
          .size({ width: 100, height: 100 })
          .borderStyle(BorderStyle.Solid)
          .onChange((isOn: bool) => {
            if (isOn) {
              gpioled.setLedStatus(388, gpioled.LED_ON)
            }
            else {
              gpioled.setLedStatus(388, gpioled.LED_OFF)
            }
          })
        }.margin({ left: 25 })

        Column() {
          Toggle({ type: ToggleType.Button }) {
            Text('床尾灯',)
              .fontSize(25).opacity(1)
          }
          .size({ width: 100, height: 100 })
          .borderStyle(BorderStyle.Solid)
          .onChange((isOn: bool) => {
            if (isOn) {
              gpioled.setLedStatus(382, gpioled.LED_ON)
              i2cnapidemo.ReadData();
            }
            else {
              gpioled.setLedStatus(382, gpioled.LED_OFF)
            }
          })

          Toggle({ type: ToggleType.Button }) {
            Text('厕所灯',)
              .fontSize(25).opacity(1)

          }
          .size({ width: 100, height: 100 })
          .borderStyle(BorderStyle.Solid)
          .opacity(1)
          .onChange((isOn: bool) => {
            if (isOn) {
              gpioled.setLedStatus(387, gpioled.LED_ON)
            }
            else {
              gpioled.setLedStatus(387, gpioled.LED_OFF)
            }
          })

          Toggle({ type: ToggleType.Button }) {
            Text('浴室灯',)
              .fontSize(25).opacity(1)
          }
          .size({ width: 100, height: 100 })
          .borderStyle(BorderStyle.Solid)
          .onChange((isOn: bool) => {
            if (isOn) {
              gpioled.setLedStatus(388, gpioled.LED_ON)
            }
            else {
              gpioled.setLedStatus(388, gpioled.LED_OFF)
            }
          })

          Toggle({ type: ToggleType.Button }) {
            Text('读书灯',)
              .fontSize(25).opacity(1)
          }
          .size({ width: 100, height: 100 })
          .borderStyle(BorderStyle.Solid)
          .onChange((isOn: bool) => {
            if (isOn) {
              gpioled.setLedStatus(388, gpioled.LED_ON)
            }
            else {
              gpioled.setLedStatus(388, gpioled.LED_OFF)
            }
          })
        }
      }
      .backgroundColor('#ffe5e5e5')
      .borderRadius('30')
      .height('625')
      .width('250')
      .margin({ left: -20 })


      Column() {

        Slider({
          min: 0,
          max: 50,
          style: SliderStyle.OutSet,

        })
          .size({ width: 100, height: 50 })
          .scale({ x: 3, y: 3 })
          .blockColor(Color.Orange)
          .trackColor(Color.Gray)
          .margin({ top: 15, left: 5 })
          .onChange((value: number) => {
            this.pwm_Angle = value
            pwmtest.setPwmStatus(1, this.pwm_Angle)
          })
        Text('空调角度',)
        Slider({
          min: 0,
          max: 50,
          style: SliderStyle.OutSet
        })
          .size({ width: 100, height: 50 })
          .scale({ x: 3, y: 3 })
          .blockColor(Color.Orange)
          .trackColor(Color.Gray)
          .margin({ top: 50, left: 5 })
          .onChange((value: number) => {
            this.pwm_Angle = value
            pwmtest.setPwmStatus(1, this.pwm_Angle)
          })
        Text('窗帘1',)
        Slider({
          min: 0,
          max: 50,
          style: SliderStyle.OutSet
        })
          .size({ width: 100, height: 50 })
          .scale({ x: 3, y: 3 })
          .blockColor(Color.Orange)
          .trackColor(Color.Gray)
          .margin({ top: 50, left: 5 })
          .onChange((value: number) => {
            this.pwm_Angle = value
            pwmtest.setPwmStatus(1, this.pwm_Angle)
          })
        Text('窗帘2',)
        Slider({
          min: 0,
          max: 50,
          style: SliderStyle.OutSet
        })
          .size({ width: 100, height: 50 })
          .scale({ x: 3, y: 3 })
          .blockColor(Color.Orange)
          .trackColor(Color.Gray)
          .margin({ top: 50, left: 5 })
          .onChange((value: number) => {
            this.pwm_Angle = value
            pwmtest.setPwmStatus(1, this.pwm_Angle)
          })
        Text('排气扇',)
      }
      .position({ x: 680, y: 5 })
      .backgroundColor('#ffe5e5e5')
      .borderRadius('30')
      .height('625')
      .width('270')
    }.width("100%")
    .height('100%').backgroundImage('/common/images/back.jpg', ImageRepeat.NoRepeat)
    .backgroundImageSize({ width: 1100, height: 1200 })

  }
}