import router from '@ohos.router';
import promptAction from '@ohos.promptAction'
import pwm from '@ohos.pwm';
import gpio from '@ohos.gpioled';
import TempAndHum from '@ohos.tempandhum'
import hilog from '@ohos.hilog';
import prompt from '@ohos.prompt'
import { BusinessError } from '@ohos.base'

@Entry
@Component
struct Index {
  // Popup组件标志位
  @State handlePopup: boolean = false;
  // 摄氏度值
  @State getTemperatureC: number = 0;
  // 湿度值
  @State getHumidity: number = 0;
  // 光照值
  @StorageLink('getLightIntensity') getLightIntensity: number = 0;
  // 侧边栏
  @State arr: number[] = [1, 2, 3];
  @State current: number = 1;
  // 平均值
  @State averageTemperatureC: number = 0;
  @State averageHumidity: number = 0
  @State sumTemperatureC: number = 0;
  @State sumHumidity: number = 0;
  @State account: number = 1;
  // 统计
  @StorageLink('sumBackHome') sumDayBackHome: number = 0;
  @StorageLink('sumLeaveHome') sumDayLeaveHome: number = 0;
  @StorageLink('sumWeekBackHome') sumWeekBackHome: number = 0;
  @StorageLink('sumWeekLeaveHome') sumWeekLeaveHome: number = 0;
  @StorageLink('lightCount0') count0: number = 1;
  @StorageLink('lightCount1') count1: number = 1;
  @StorageLink('lightCount2') count2: number = 1;
  @StorageLink('lightCount3') count3: number = 1;
  // 获取第二页的秒数
  @StorageLink('leaveHomeLight0') leaveHomeLight0: number = 1; //等于1时候代表关，等于0时候代表开
  @StorageLink('leaveHomeLight1') leaveHomeLight1: number = 1;
  @StorageLink('leaveHomeLight2') leaveHomeLight2: number = 1;
  @StorageLink('leaveHomeLight3') leaveHomeLight3: number = 1;
  @StorageLink('backHomeLight0') backHomeLight0: number = 1; //等于1时候代表关，等于0时候代表开
  @StorageLink('backHomeLight1') backHomeLight1: number = 1;
  @StorageLink('backHomeLight2') backHomeLight2: number = 1;
  @StorageLink('backHomeLight3') backHomeLight3: number = 1;
  @StorageLink('backHomeLightControl') backLightControl: number = 0; //等于0时候代表关，等于1时候代表开
  @StorageLink('leaveHomeLightControl') leaveLightControl: number = 0;
  @StorageLink('leaveHomeTime') leaveHomeTime: number = 0;
  @StorageLink('backHomeTime') backHomeTime: number = 0;
  //窗帘角度
  @StorageLink('curtainAngle') pwmAngle: number = 0;
  // 时钟
  @State accumulateTime: number = 0;
  @StorageLink('timeHour') timeHour: number = 0;
  @StorageLink('timeMinute') timeMinute: number = 0;
  // 灯开关
  @State lightswitch: number = 0;
  controller: TextClockController = new TextClockController();
  // 第二页判断开关
  @StorageLink('isOn1') isOn1: boolean = false;
  @StorageLink('isOn2') isOn2: boolean = false;
  @StorageLink('isOn3') isOn3: boolean = false;
  // 与第二页光强匹配
  @StorageLink('getLightValue1') getLightValue1: number = 1000;
  @StorageLink('getLightValue2') getLightValue2: number = 1000;
  // 灯光引脚
  @State light1: number = 381; // 客厅前灯
  @State light2: number = 382; // 客厅后
  @State light3: number = 383; // 卧室
  @State light4: number = 384; // 厨房
  // 定时器ID
  @State intervalID1: number = 0;
  @State intervalID2: number = 0;
  @State intervalID3: number = 0;
  @State intervalID4: number = 0;
  @State intervalID5: number = 0;
  // 计时
  @State sumLightTime1: number = 0;
  @State sumLightTime2: number = 0;
  @State sumLightTime3: number = 0;
  @State sumLightTime4: number = 0;
  @State sumPwmTime1: number = 0;

  // 获取光照、温度、湿度值
  getValue() {
    let setMode = 0
    setMode = TempAndHum.SetMode(3, 2);
    TempAndHum.ReadTemp_Hum(); // 读取数据
    this.getTemperatureC = TempAndHum.GetTemperatureC();
    this.getHumidity = TempAndHum.GetHumidity();
  }

  // 用于刷新获取温湿度值
  startWork() {
    this.getValue();
    setInterval(() => {
      this.getValue();
    }, 1000);
  }

  build() {
    Stack() {

      Column() {
        Row() {
          Column({ space: 2 }) {
            // 灯光控制
            Text($r('app.string.controlLight'))
              .size({ width: 200, height: 50 })
              .backgroundColor($r('app.color.controlLight'))
              .fontSize(28)
              .opacity(0.7)
              .fontColor(Color.Pink)
              .fontWeight(FontWeight.Bold)
              .offset({ x: '0%', y: '9%' })
              .textAlign(TextAlign.Center)//文字居中
              .border({ radius: 5 })
            Stack({}) {
              Text()
                .size({ width: 380, height: 250 })//.offset({x:'-18.5%',y:'24%'})
                .border({ radius: 50, style: BorderStyle.Dotted, width: 5, color: Color.White })
              Column() {
                Row({ space: 5 }) {
                  // 客厅前灯
                  if (this.count0 % 2 == 0) {
                    Button({ type: ButtonType.Circle, stateEffect: true }) {
                      LoadingProgress()
                        .width(20)
                        .height(20)
                        .color($r('app.color.start_window_background'))
                    }
                    .backgroundColor($r('app.color.closeLight'))
                    .width(75)
                    .height(75)
                    .offset({ x: '-3%', y: '10%' })
                    .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.7 })
                    .onClick(() => {
                      this.count0++;
                      promptAction.showToast({
                        message: 'The duration of the living room light: ' + this.sumLightTime1 + 's',
                        duration: 2000
                      });
                      clearInterval(this.intervalID1);
                      this.sumLightTime1 = 0;
                      gpio.SetLedValue(this.light1, (this.count0 + 1) % 2).then();
                    });
                  } else if (this.count0 % 2 == 1) {
                    Button({ type: ButtonType.Circle, stateEffect: true }) {
                      Text($r('app.string.frontLiving'))
                        .fontWeight(FontWeight.Bold)
                        .textAlign(TextAlign.Center)
                        .fontSize(20)
                    }
                    .width(75)
                    .height(75)
                    .backgroundColor($r('app.color.openLight'))
                    .opacity(0.6)
                    .offset({ x: '-3%', y: '10%' })
                    .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.7 })
                    .onClick(() => {
                      this.count0--;
                      this.intervalID1 = setInterval(() => {
                        this.sumLightTime1++;

                      }, 1000);
                      gpio.SetLedValue(this.light1, (this.count0 + 1) % 2).then();
                    });
                  }
                  // 客厅后灯
                  if (this.count1 % 2 == 0) {
                    Button({ type: ButtonType.Circle, stateEffect: true }) {
                      LoadingProgress()
                        .width(20).height(20)
                        .color($r('app.color.start_window_background'));
                    }
                    .width(100)
                    .height(100)
                    .backgroundColor($r('app.color.closeLight'))
                    .offset({ x: '0%', y: '40%' })
                    .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.7 })
                    .onClick(() => {
                      this.count1++;
                      promptAction.showToast({
                        message: 'The duration of the living room light: ' + this.sumLightTime2 + 's',
                        duration: 2000
                      });
                      clearInterval(this.intervalID2);
                      this.sumLightTime2 = 0;
                      gpio.SetLedValue(this.light2, (this.count1 + 1) % 2).then();

                    });
                  } else if (this.count1 % 2 == 1) {
                    Button({ type: ButtonType.Circle, stateEffect: true }) {
                      Text($r('app.string.behindLiving'))
                        .fontWeight(FontWeight.Bold)
                        .textAlign(TextAlign.Center)
                        .fontSize(20)
                    }
                    .width(100)
                    .height(100)
                    .backgroundColor($r('app.color.openLight'))
                    .opacity(0.6)
                    .offset({ x: '0%', y: '40%' })
                    .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.7 })
                    .onClick(() => {
                      this.count1--;
                      this.intervalID2 = setInterval(() => {
                        this.sumLightTime2++
                      }, 1000);
                      gpio.SetLedValue(this.light2, (this.count1 + 1) % 2).then()
                    })
                  }
                  // 卧室灯光
                  if (this.count2 % 2 == 0) {
                    Button({ type: ButtonType.Circle, stateEffect: true }) {
                      LoadingProgress()
                        .width(20).height(20)
                        .color($r('app.color.start_window_background'));
                    }
                    .width(75)
                    .height(75)
                    .backgroundColor($r('app.color.closeLight'))
                    .offset({ x: '4%', y: '14%' })
                    .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.7 })
                    .onClick(() => {
                      this.count2++;
                      promptAction.showToast({
                        message: 'The duration of the bedroom light: ' + this.sumLightTime3 + 's',
                        duration: 2000
                      });
                      clearInterval(this.intervalID3);
                      this.sumLightTime3 = 0;
                      gpio.SetLedValue(this.light3, (this.count2 + 1) % 2).then();
                    })
                  } else if (this.count2 % 2 == 1) {
                    Button({ type: ButtonType.Circle, stateEffect: true }) {
                      Text($r('app.string.bedroom'))
                        .fontSize(20)
                        .fontWeight(FontWeight.Bold)
                        .textAlign(TextAlign.Center)
                    }
                    .width(75)
                    .height(75)
                    .backgroundColor($r('app.color.openLight'))
                    .opacity(0.6)
                    .offset({ x: '4%', y: '14%' })
                    .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.7 })
                    .onClick(() => {
                      this.count2--;
                      this.intervalID3 = setInterval(() => {
                        this.sumLightTime3++;
                      }, 1000);
                      gpio.SetLedValue(this.light3, (this.count2 + 1) % 2).then();
                    })
                  }
                }.height('50%')

                Row() {
                  // 厨房灯光
                  if (this.count3 % 2 == 0) {
                    Button({ type: ButtonType.Circle, stateEffect: true }) {
                      LoadingProgress()
                        .width(20)
                        .height(20)
                        .color($r('app.color.start_window_background'))
                    }
                    .width(90)
                    .height(90)
                    .backgroundColor($r('app.color.closeLight'))
                    .offset({ x: '-8%', y: '-30%' })
                    .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.8 })
                    .onClick(() => {
                      this.count3++;
                      promptAction.showToast({
                        message: 'The duration of the kitchen light:' + this.sumLightTime4 + 's',
                        duration: 2000
                      });
                      clearInterval(this.intervalID4);
                      this.sumLightTime4 = 0
                      gpio.SetLedValue(this.light4, (this.count3 + 1) % 2).then()

                    })
                  } else if (this.count3 % 2 == 1) {
                    Button({ type: ButtonType.Circle, stateEffect: true }) {
                      Text($r('app.string.kitchen'))
                        .fontSize(20)
                        .fontWeight(FontWeight.Bold)
                        .textAlign(TextAlign.Center)
                    }
                    .width(90)
                    .height(90)
                    .backgroundColor($r('app.color.openLight'))
                    .opacity(0.6)
                    .offset({ x: '-8%', y: '-30%' })
                    .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.8 })
                    .onClick(() => {
                      this.count3--;
                      this.intervalID4 = setInterval(() => {
                        this.sumLightTime4++;
                      }, 1000);
                      gpio.SetLedValue(this.light4, (this.count3 + 1) % 2).then();
                    })
                  }
                  //一键关灯组件
                  Button() {
                    Text($r('app.string.closeAll'))
                      .textAlign(TextAlign.Center)
                      .fontSize(18)
                  }
                  .backgroundColor($r('app.color.lightALL'))
                  .offset({ x: '3%', y: '-8%' })
                  .width(100)
                  .height(50)
                  .onClick(() => {
                    if (this.count0 % 2 == 0) {
                      this.count0++;
                    }
                    if (this.count1 % 2 == 0) {
                      this.count1++;
                    }
                    if (this.count2 % 2 == 0) {
                      this.count2++
                    }
                    if (this.count3 % 2 == 0) {
                      this.count3++
                    }
                    this.sumLightTime1 = 0
                    this.sumLightTime2 = 0
                    this.sumLightTime3 = 0
                    this.sumLightTime4 = 0
                    clearInterval(this.intervalID1);
                    clearInterval(this.intervalID2);
                    clearInterval(this.intervalID3);
                    clearInterval(this.intervalID4);
                    gpio.SetLedValue(this.light1, 0).then()
                    gpio.SetLedValue(this.light2, 0).then()
                    gpio.SetLedValue(this.light3, 0).then()
                    gpio.SetLedValue(this.light4, 0).then() // 0表示关灯
                  })

                  Button() {
                    Text($r('app.string.openAll'))
                      .textAlign(TextAlign.Center)
                      .fontSize(18)
                  }
                  .backgroundColor($r('app.color.lightALL'))
                  .offset({ x: '10%', y: '-20%' })
                  .width(50)
                  .height(90)
                  .onClick(() => {
                    if (this.count0 % 2 == 1) {
                      this.count0++;
                      this.intervalID1 = setInterval(() => {
                        this.sumLightTime1++;
                      }, 1000);
                    }
                    if (this.count1 % 2 == 1) {
                      this.count1++;
                      this.intervalID2 = setInterval(() => {
                        this.sumLightTime2++;
                      }, 1000);
                    }
                    if (this.count2 % 2 == 1) {
                      this.count2++;
                      this.intervalID3 = setInterval(() => {
                        this.sumLightTime3++;
                      }, 1000);
                    }
                    if (this.count3 % 2 == 1) {
                      this.count3++;
                      this.intervalID4 = setInterval(() => {
                        this.sumLightTime4++;
                      }, 1000);
                    }
                    gpio.SetLedValue(this.light1, 1).then();
                    gpio.SetLedValue(this.light2, 1).then();
                    gpio.SetLedValue(this.light3, 1).then();
                    gpio.SetLedValue(this.light4, 1).then(); // 1表示开灯
                  })
                }.height('50%');
              }
            }
          }
          .width('50%')
          .offset({ x: '0%', y: '5%' })

          Column({ space: 25 }) {
            //时钟组件
            TextClock({ timeZoneOffset: -8, controller: this.controller })// timeZoneOffset 时区偏移ian
              .fontColor(Color.Black)
              .format('Hms')
              .opacity(0.7)
              .fontWeight(FontWeight.Bolder)
              .onDateChange((value: number) => {
                this.accumulateTime = value;
                this.timeHour = this.accumulateTime % 86400; // 一天等于86400s
                // 离家模式关灯
                if (this.timeHour == this.leaveHomeTime && this.leaveLightControl == 1 && this.leaveHomeLight0 == 0) {
                  if (this.count0 % 2 == 0) {
                    this.count0++;
                  }
                  gpio.SetLedValue(this.light1, 0).then();
                }
                if (this.timeHour == this.leaveHomeTime + 1 && this.leaveLightControl == 1 && this.leaveHomeLight1 == 0) {
                  if (this.count1 % 2 == 0) {
                    this.count1++;
                  }
                  gpio.SetLedValue(this.light2, 0).then();
                }
                if (this.timeHour == this.leaveHomeTime + 2 && this.leaveLightControl == 1 && this.leaveHomeLight2 == 0) {
                  if (this.count2 % 2 == 0) {
                    this.count2++;
                  }
                  gpio.SetLedValue(this.light3, 0).then();
                }
                if (this.timeHour == this.leaveHomeTime + 3 && this.leaveLightControl == 1 && this.leaveHomeLight3 == 0) {
                  if (this.count3 % 2 == 0) {
                    this.count3++;
                  }
                  gpio.SetLedValue(this.light4, 0).then();
                }
                // 回家模式开灯
                if (this.timeHour == this.backHomeTime && this.backLightControl == 1 && this.backHomeLight0 == 0) {
                  if (this.count0 % 2 == 1) {
                    this.count0++;
                  }
                  gpio.SetLedValue(this.light1, 1).then();
                }
                if (this.timeHour == this.backHomeTime + 1 && this.backLightControl == 1 && this.backHomeLight1 == 0) {
                  if (this.count1 % 2 == 1) {
                    this.count1++;
                  }
                  gpio.SetLedValue(this.light2, 1).then();
                }
                if (this.timeHour == this.backHomeTime + 2 && this.backLightControl == 1 && this.backHomeLight2 == 0) {
                  if (this.count2 % 2 == 1) {
                    this.count2++;
                  }
                  gpio.SetLedValue(this.light3, 1).then();
                }
                if (this.timeHour == this.backHomeTime + 3 && this.backLightControl == 1 && this.backHomeLight3 == 0) {
                  if (this.count3 % 2 == 1) {
                    this.count3++;
                  }
                  gpio.SetLedValue(this.light4, 1).then();
                }
              })
              .fontSize(90)
              .fontWeight(FontWeight.Bold)
              .fontStyle(FontStyle.Italic);
            // 窗帘模拟
            Gauge({ value: 0, min: -90, max: 90 })
              .startAngle(-90)
              .endAngle(90)
              .strokeWidth(35)
              .colors([[$r('app.color.curtainModel'), 1]])
              .value(this.pwmAngle - 90)
              .height('70%')
              .width('90%')
          }
          .width('50%')
          .offset({ x: '0%', y: '18%' })
        }
        .height('70%');

        Row({ space: 3 }) {
          Column() {
            Row() {
              Row() {
                // 展示温度值
                Text("") {
                  Span($r('app.string.temperature'))
                  Span(':   ' + this.getTemperatureC.toFixed(2) + '℃') // 打印摄氏温度值（两位数)
                }
                .size({ width: 200, height: 40 })
                .fontColor(Color.Black)
                .fontSize(22)
                .backgroundColor(Color.White)
                .fontWeight(FontWeight.Bold)
                .textAlign(TextAlign.End)
                .padding({ right: 10 })
                .backgroundImage($r('app.media.temperature2'))
                .backgroundImageSize(ImageSize.Contain)
                .border({
                  radius: 20,
                });
              }

              // 展示湿度值
              Row() {
                Text("") {
                  Span($r('app.string.Humidity'))
                  Span(':   ' + this.getHumidity.toFixed(2) + '%RH') // 打印湿度度值（两位数)
                } //打印湿度值
                .size({ width: 230, height: 40 })
                .fontColor(Color.Black)
                .backgroundColor(Color.White)
                .fontSize(22)
                .fontWeight(FontWeight.Bold)
                .textAlign(TextAlign.End)
                .padding({ right: 10 })
                .backgroundImage($r('app.media.humidity2'))
                .backgroundImageSize(ImageSize.Contain)
                .border({
                  radius: 20,
                });
              }
            }
            .opacity(0.7);
          }.width('45%');

          Column() {
            // 温湿度刷新按钮
            Button() {
              Text($r('app.string.refresh'))
                .textAlign(TextAlign.Center)
                .fontSize(20);
            }
            .backgroundColor($r('app.color.lightALL'))
            .offset({ x: '1%', y: '0%' })
            .width(70)
            .height(40)
            .onClick(() => {
              if (this.getTemperatureC == 0 && this.getHumidity == 0) {
                this.startWork();
              }
            })
          }

          Column() {
            Text($r('app.string.controlCurtain'))
              .size({ width: 200, height: 50 })
              .fontColor(Color.Black)
              .fontSize(29)
              .opacity(0.8)
              .fontWeight(FontWeight.Bold)
              .offset({ x: '0%', y: '-10%' })
              .textAlign(TextAlign.Center)// 文字居中
              .border({ radius: 5 })
            // Slider滑块
            Slider({
              style: SliderStyle.InSet,
              value: this.pwmAngle,
              min: 0,
              max: 100
            })
              .opacity(0.7)
              .showTips(true, 'value:' + this.pwmAngle.toFixed())
              .offset({ x: '4%', y: '-15%' })
              .width('80%')
              .blockColor($r('app.color.start_window_background'))
              .trackColor($r('app.color.sliderOpen'))
              .selectedColor($r('app.color.sliderClose'))
              .onChange(value => {
                this.pwmAngle = value * 1.8
                if (this.pwmAngle > 0 && this.sumPwmTime1 == 0) {
                  this.sumPwmTime1 = 1;
                  this.intervalID5 = setInterval(() => {
                    this.sumPwmTime1++;
                  }, 1000);
                }
                if (this.pwmAngle == 0 && this.sumPwmTime1 != 0) {
                  promptAction.showToast({
                    message: 'The duration of shading by the curtains: ' + this.sumPwmTime1 + 's',
                    duration: 2000
                  });
                  this.sumPwmTime1 = 0;
                  clearInterval(this.intervalID5);
                }

                pwm.SetPwmAsync(1, this.pwmAngle).then(); // 引脚为1
              })
          }.width('40%');
          Column() {
            // 更多模式：跳转到下一页面
            Button() {
              Text('More..')
                .textAlign(TextAlign.Center)
                .fontColor(Color.Blue)
                .fontSize(19)
            }
            .backgroundColor($r('app.color.lightALL'))
            .offset({ x: '-2%', y: '0%' })
            .width(70)
            .height(40)
            .onClick(() => {
              router.pushUrl({ url: 'filemanager/pages/intelligentscenario' })
            })
          }
        }.height('30%')
      }.width('100%')
      .backgroundImage($r('app.media.secondbackground3'))
      .backgroundImageSize(ImageSize.Cover)
    }.alignContent(Alignment.TopStart);
  }
}