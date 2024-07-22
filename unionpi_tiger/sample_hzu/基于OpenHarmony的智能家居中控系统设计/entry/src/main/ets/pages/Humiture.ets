import router from '@ohos.router';
import i2cnapidemo from '@ohos.i2cnapidemo';
import prompt from '@ohos.prompt';
import promptAction from '@ohos.promptAction';
import gpioled from '@ohos.gpioled';
@Entry
@Component
struct Second {
  // Popup组件标志位
  @State handlePopup: boolean = false
  // 摄氏度值
  @State readTemperatureC: number = 0;
  // 湿度值
  @State readHumidity: number = 0;
  // 华氏度值
  @State readTemperatureF: number = 0;
  // 使用教程文本
  @State message: string = '使用教程:一.启动/重启 二.滑动选择后按下模式选择 三.获取';
  // TextPicker组件参数
  @State select1: number = 3;
  @State mps: string[] = ['0.5Hz', '1Hz', '2Hz', '4Hz', '10Hz'];
  @State select2: number = 1;
  @State repeatability: string[] = ['low', 'mid', 'high'];
  // Popup组件初始化message值
  @State popupMessage: string = 'The current mps is ' + this.mps[this.select1] + '\n' +
    'And the current refresh rate is ' + this.Repeatability[this.select2] + '\n' +
    'Are you sure to refresh?';
  intervalID: number = 0;
  @State pin: number = 386;
  I:Number = 0 ;
  // 构造器供Popup组件调用显示当前模式
  @Builder
  popupBuilder() {
    Column() {

    }
  }
  // 获取温湿度
  getValue() {
    i2cnapidemo.ReadData();
    this.ReadTemperatureC = i2cnapidemo.ReadTemperatureC();
    this.ReadHumidity = i2cnapidemo.ReadHumidity();
    this.ReadTemperatureF = i2cnapidemo.ReadTemperatureF();
    if (this.ReadTemperatureC>=28){
      gpioled.setLedStatus(this.pin, gpioled.LED_OFF)
      this.I=1
    }
    if (this.I ==1 ) {
      if (this.ReadTemperatureC<28) {

      this.I =0
      gpioled.setLedStatus(this.pin, gpioled.LED_ON)
     }
    }
  }
  // 开启定时器
  startWork() {
    this.intervalID = setInterval(() => {
      this.getValue();
    }, 500)
  }
  build() {
    Column() {
      Stack(){
      Image($r('app.media.bj6'))
      // 显示使用说明
      // 按钮选项
      Column() {
        Row() {
          Button(){
            Text('蜂鸣器测试')
          }.fontSize(70)
          .offset({ // 使用相对定位，设置组件位置
            x: 700,
            y: -250
          })
          .onClick(() => {
            router.pushUrl({ url: 'pages/Buzzer' })
          })

          Column(){
            Text( '当前温度' )
              .fontSize(70)
              .fontWeight(FontWeight.Bold)
              .textAlign(TextAlign.Center)
              .id('TempC')
              //.margin({right:'30%'})
              .fontColor(Color.White)
          Text( this.ReadTemperatureC.toFixed(2)+ '°C' )
            .fontSize(100)
            .fontWeight(FontWeight.Bold)
            .textAlign(TextAlign.Center)
            .id('TempC')
            //.margin({right:'30%'})
            .fontColor(Color.White)
          }.margin({right:'10%'})
             Blank()
          Column(){
            Text( '当前湿度' )
              .fontSize(70)
              .fontWeight(FontWeight.Bold)
              .textAlign(TextAlign.Center)
              .id('TempC')
              //.margin({right:'30%'})
              .fontColor(Color.White)
          Text(this.ReadHumidity.toFixed(2)+ '%')
            .fontSize(100)
            .fontWeight(FontWeight.Bold)
            .textAlign(TextAlign.Center)
            .id('Hum')
            .fontColor(Color.White)
        }
        }.margin({ top: '15%' })


        Row() {
          Button('启动/重启')
            .ButtonStyle()
            .margin({
              right: 40
            })
            .width('20%')
            .height('10%')
            .id('softReset_button')
            //.margin({ top: '5%' })
            .onClick(() => {
              if (i2cnapidemo.ReadTemperatureC() <= 800) {
                i2cnapidemo.SoftReset();
                i2cnapidemo.ModeSet(this.select1, this.select2);
                promptAction.showToast({
                  message: '启动/重启成功', // 显示文本
                  duration: 500, // 显示时长
                  bottom: 100 // 距离底部的距离
                })
              }else {
                promptAction.showToast({
                  message: '启动失败，请检查连线', // 显示文本
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
            .id('getTemp_Button')
            //.margin({ top: '5%' })
            .onClick(() => {
              i2cnapidemo.ReadData();
              if (i2cnapidemo.ReadTemperatureC() <= 800) {
                i2cnapidemo.ReadData();
                this.ReadTemperatureC = i2cnapidemo.ReadTemperatureC();
                this.ReadHumidity = i2cnapidemo.ReadHumidity();
                this.ReadTemperatureF = i2cnapidemo.ReadTemperatureF();
                if (this.intervalID) {
                  clearInterval(this.intervalID)
                }
                this.startWork();
                console.info(' i2cvalue ' + this.ReadTemperatureC + '\n' + this.ReadHumidity + '\n' + this.ReadTemperatureF);
                promptAction.showToast({
                  message: '获取成功', // 显示文本
                  duration: 500, // 显示时长
                  bottom: 100 // 距离底部的距离
                })
            }else {
                clearInterval(this.intervalID);
                promptAction.showToast({
                  message: '获取失败，请检查连线', // 显示文本
                  duration: 1000, // 显示时长
                  bottom: 100 // 距离底部的距离
                })
              }

            })

        }
        .margin({ top: '5%' })
      }
    }.backgroundImageSize(ImageSize.Cover).height('100%').width('100%')
    }
  }
}
