//模式选择页面
import router from '@ohos.router';
import prompt from '@ohos.prompt'
import pwm from '@ohos.pwm';
import gpio from '@ohos.gpioled';
import light from '@ohos.light'
import { MatchPattern } from '@ohos.UiTest';

@Entry
@Component
struct SliderExample {
  // Popup组件标志位
  @State handlePopup1: boolean = false;
  @State handlePopup2: boolean = false;
  // 光照值
  @StorageLink('getLightIntensity') getLightIntensity: number = 0;
  // 判断值
  @StorageLink('isOn1') isOn1: boolean = false;
  @StorageLink('isOn2') isOn2: boolean = false;
  @StorageLink('isOn3') isOn3: boolean = false;
  // 窗帘角度
  @StorageLink('curtainAngle') pwmAngle: number = 0;
  // 获取选择的光强用于判断(1000为边界)
  @StorageLink('getLightValue1') getLightValue1: number = 1000;
  @StorageLink('getLightValue2') getLightValue2: number = 1000;
  // 选择光强参照
  private lightValue1: number[] = [1, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000];
  private lightValue2: number[] = [1000, 1500, 2000, 2500, 3000, 3500, 4000];
  // index判断数组
  private indexIf: number = 0;
  // 文本选择
  private select: number | number[] = 2;
  private selectLightValue1: string[] = ['1', '100', '200', '300', '400', '500', '600', '700', '800', '900', '1000'];
  private selectLightValue2: string[] = ['1000', '1500', '2000', '2500', '3000', '3500', '4000'];
  data: Record<string, string> = { 'data1': 'default' };

  // 获取光照、温度、湿度值
  getValue() {
    light.ReadLightIntensity(1).then(); // 读取光强传感器数据，引脚为1
    this.getLightIntensity = light.GetLightIntensity();
  }

  // 拉起窗帘
  ClosePwm() {
    setInterval(() => {
      if (this.isOn1 == true && this.getLightIntensity <= this.getLightValue1) {

        if (this.getLightIntensity <= 1) {
          this.pwmAngle = 0;
          pwm.SetPwmAsync(1, 0).then(); // 引脚为1
        } else if (this.getLightIntensity <= 100) {
          this.pwmAngle = 9;
          pwm.SetPwmAsync(1, 9).then(); // 引脚为1
        } else if (this.getLightIntensity <= 200) {
          this.pwmAngle = 18;
          pwm.SetPwmAsync(1, 18).then(); // 引脚为1
        } else if (this.getLightIntensity <= 300) {
          this.pwmAngle = 27;
          pwm.SetPwmAsync(1, 27).then(); // 引脚为1
        } else if (this.getLightIntensity <= 400) {
          this.pwmAngle = 36;
          pwm.SetPwmAsync(1, 36).then(); // 引脚为1
        } else if (this.getLightIntensity <= 500) {
          this.pwmAngle = 45;
          pwm.SetPwmAsync(1, 45).then(); // 引脚为1
        } else if (this.getLightIntensity <= 600) {
          this.pwmAngle = 54;
          pwm.SetPwmAsync(1, 54).then(); // 引脚为1
        } else if (this.getLightIntensity <= 700) {
          this.pwmAngle = 63;
          pwm.SetPwmAsync(1, 63).then(); // 引脚为1
        } else if (this.getLightIntensity <= 800) {
          this.pwmAngle = 72;
          pwm.SetPwmAsync(1, 72).then(); // 引脚为1
        } else if (this.getLightIntensity <= 900) {
          this.pwmAngle = 81;
          pwm.SetPwmAsync(1, 81).then(); // 引脚为1
        } else if (this.getLightIntensity < 1000) {
          this.pwmAngle = 90;
          pwm.SetPwmAsync(1, 90).then(); // 引脚为1
        }
      }
    }, 1000);
  }

  // 光照太强拉开窗帘
  OpenPwm() {
    setInterval(() => {
      if (this.isOn2 == true && this.getLightIntensity >= this.getLightValue2) {
        this.pwmAngle = 179;
        pwm.SetPwmAsync(1, 179).then(); // 最高转到179度，180度会超出阈值
      }
    }, 1000); // 每隔1s刷新一次
  }

  // value.index值转换
  turnIndex() {
    this.indexIf = 0;
    while (this.indexIf <= 10) {
      if (this.select == this.indexIf) {
        break;
      } else {
        this.indexIf++;
      }
    }
  }

  // value.index值转换
  turnIndex2() {
    this.indexIf = 0;
    while (this.indexIf <= 6) {
      if (this.select == this.indexIf) {
        break;
      } else {
        this.indexIf++;
      }
    }
  }

  // 定时器用于刷新获取温湿度值
  startWork() {
    this.getValue();
    setInterval(() => {
      this.getValue();
    }, 60000);
  }

  build() {
    Column() {
      Row() {
        // 智能场景
        Text($r('app.string.intelligentScenario'))
          .textAlign(TextAlign.Center)
          .fontSize(50)
          .fontWeight(FontWeight.Bolder)
          .width('100%')
          .fontColor($r('app.color.controlLight'))
          .opacity(0.8);
        Button()// 返回按钮
          .onClick(() => {
            router.pushUrl({ url: 'pages/Index' });
          })
          .backgroundColor(Color.White)
          .fontColor(Color.Blue)
          .opacity(0.7)
          .backgroundImage($r('app.media.return'))
          .backgroundImageSize(ImageSize.Cover)
          .width(50)
          .height(50)
          .offset({ x: '-97%', y: '0%' });
      }
      .height('20%')
      .width('100%');
      Row() {
        //回家模式跳转按钮
        Row() {
          Stack() {
            // 回家模式
            Text($r('app.string.backHome'))
              .fontSize(30)
              .textAlign(TextAlign.Start)
              .align(Alignment.Bottom)
              .fontColor(Color.White)
              .width('75%')
              .height('90%')
              .border({
                radius: 20,
              })
              .margin({ bottom: 5 })
              .padding({
                bottom: 5
              });
            // 跳转按钮
            Button({ type: ButtonType.Normal })
              .onClick(() => {
                router.pushUrl({ url: 'filemanager/pages/backhome' });
              })
              .opacity(0)
              .backgroundColor(Color.White)
              .width('100%')
              .height('100%');
          }
        }
        .backgroundImage($r('app.media.returnhome2'))
        .backgroundImageSize(ImageSize.Cover)
        .opacity(0.7)
        .width('45%')
        .height('90%')
        .border({
          radius: 20,
        });
        // 离家模式跳转按钮
        Row() {
          Stack() {
            // 离家模式
            Text($r('app.string.leftHome'))
              .fontSize(30)
              .fontColor(Color.White)
              .textAlign(TextAlign.Start)
              .align(Alignment.Bottom)
              .width('75%')
              .height('90%')
              .border({
                radius: 20,
              })
              .margin({ bottom: 5 })
              .padding({
                bottom: 5
              });
            // 跳转离家页面
            Button({ type: ButtonType.Normal })
              .onClick(() => {
                router.pushUrl({ url: 'filemanager/pages/lefthome' });
              })
              .opacity(0)
              .backgroundColor(Color.White)
              .width('100%')
              .height('100%');
          }
        }
        .backgroundImage($r('app.media.awayhome2'))
        .backgroundImageSize(ImageSize.Cover)
        .opacity(0.7)
        .width('45%')
        .height('90%')
        .border({
          radius: 20,
        });
      }
      .width('100%')
      .height('40%')
      .justifyContent(FlexAlign.SpaceEvenly); // 主轴对齐

      // 第二行
      Row() {
        // 采光模式跳转按钮
        Row() {
          Stack() {
            Text('') {
              Span($r('app.string.presentLight'));
              Span(' ' + this.getLightIntensity);
            }
            .fontSize(30)
            .textAlign(TextAlign.Start)
            .align(Alignment.Top)
            .fontColor($r('app.color.controlLight'))
            .width('75%')
            .height('90%')
            .border({
              radius: 20,
            })
            .margin({
              top: 5
            });
            // 采光模式
            Text($r('app.string.lighting'))
              .fontSize(30)
              .textAlign(TextAlign.Start)
              .align(Alignment.Bottom)
              .fontColor(Color.White)
              .width('75%')
              .height('90%')
              .border({
                radius: 20,
              })
              .margin({ bottom: 5 })
              .padding({
                bottom: 5
              });

            // 按钮弹出光照选择
            Button({ type: ButtonType.Normal })
              .onClick(() => {
                TextPickerDialog.show({
                  range: this.selectLightValue1,
                  selected: this.select,
                  disappearTextStyle: { color: Color.Red, font: { size: 15, weight: FontWeight.Lighter } },
                  textStyle: { color: Color.Black, font: { size: 20, weight: FontWeight.Normal } },
                  selectedTextStyle: { color: Color.Blue, font: { size: 30, weight: FontWeight.Bolder } },
                  onAccept: (value: TextPickerResult) => {
                    // 设置select为按下确定按钮时候的选中项index，这样当弹窗再次弹出时显示选中的是上一次确定的选项
                    this.select = value.index;
                    this.turnIndex();
                    this.getLightValue1 = this.lightValue1[this.indexIf];
                  },
                  onCancel: () => {
                  },
                  onChange: (value: TextPickerResult) => {
                  }
                })
              })
              .opacity(0)
              .backgroundColor(Color.White)
              .width('100%')
              .height('100%');

            //帮助提示
            Button('?')
              .backgroundColor(Color.Black)
              .onClick(() => {
                this.handlePopup1 = !this.handlePopup1
              })
              .bindPopup(this.handlePopup1, {
                message: 'night：light<1; ',
                onStateChange: (e) => { // 返回当前的气泡状态
                  if (!e.isVisible) {
                    this.handlePopup1 = false;
                  }
                }
              })
              .margin({
                bottom: 100,
                left: 400
              });
            // 开关按钮
            Toggle({ type: ToggleType.Switch, isOn: this.isOn1 })
              .selectedColor($r('app.color.sliderClose'))
              .switchPointColor($r('app.color.start_window_background'))
              .backgroundColor(Color.Black)
              .border({
                radius: 20,
              })
              .onChange((isOn: boolean) => {

                this.isOn1 = isOn;
                this.startWork();
                this.ClosePwm();
              })
              .margin({
                left: 400,
                top: 100,
              })
              .width('13.5%')
              .height('20%');
          }
        }
        .backgroundImage($r('app.media.lighting'))
        .backgroundImageSize(ImageSize.Cover)
        .opacity(0.7)
        .width('45%')
        .height('90%')
        .border({
          radius: 20,
        });

        //遮光模块
        Row() {
          Stack() {
            Text() {
              Span($r('app.string.presentSelect'));
              Span(' ' + this.getLightValue2);
            }
            .fontSize(30)
            .textAlign(TextAlign.Start)
            .align(Alignment.Bottom)
            .fontColor(Color.White)
            .width('75%')
            .height('90%')
            .border({
              radius: 20,
            })
            .margin({
              bottom: 45
            })
            .padding({
              bottom: 45
            });

            Text($r('app.string.shading'))
              .fontSize(30)
              .textAlign(TextAlign.Start)
              .align(Alignment.Bottom)
              .fontColor(Color.White)
              .width('75%')
              .height('90%')
              .border({
                radius: 20,
              })
              .margin(0)
              .padding({
                bottom: 5
              });

            // 打开弹窗
            Button({ type: ButtonType.Normal })
              .onClick(() => {
                TextPickerDialog.show({
                  range: this.selectLightValue2,
                  selected: this.select,
                  disappearTextStyle: { color: Color.Red, font: { size: 15, weight: FontWeight.Lighter } },
                  textStyle: { color: Color.Black, font: { size: 20, weight: FontWeight.Normal } },
                  selectedTextStyle: { color: Color.Blue, font: { size: 30, weight: FontWeight.Bolder } },
                  onAccept: (value: TextPickerResult) => {
                    // 设置select为按下确定按钮时候的选中项index，这样当弹窗再次弹出时显示选中的是上一次确定的选项
                    this.select = value.index;
                    this.turnIndex2();
                    this.getLightValue2 = this.lightValue2[this.indexIf];
                  },
                  onCancel: () => {
                  },
                  onChange: (value: TextPickerResult) => {
                  }
                })
              })
              .opacity(0)
              .backgroundColor(Color.White)
              .width('100%')
              .height('100%');

            //帮助提示
            Button('?')
              .backgroundColor(Color.Black)
              .onClick(() => {
                this.handlePopup2 = !this.handlePopup2;
              })
              .bindPopup(this.handlePopup2, {
                message: 'sunny： light>4000; \n' +
                  'cloudy：light>3000\n',
                onStateChange: (e) => { // 返回当前的气泡状态
                  if (!e.isVisible) {
                    this.handlePopup2 = false;
                  }
                }
              })
              .margin({
                bottom: 100,
                left: 400
              });

            //开关按钮
            Toggle({ type: ToggleType.Switch, isOn: this.isOn2 })
              .selectedColor($r('app.color.sliderClose'))
              .switchPointColor($r('app.color.start_window_background'))
              .backgroundColor(Color.Black)
              .border({
                radius: 25,
              })
              .onChange((isOn: boolean) => {
                console.info('Component status:' + this.isOn2)
                this.isOn2 = isOn
                this.startWork()
                this.OpenPwm();
              })
              .margin({
                left: 400,
                top: 100,
              })
              .width('13.5%')
              .height('20%');
          }
        }
        .backgroundImage($r('app.media.shading'))
        .backgroundImageSize(ImageSize.Cover)
        .opacity(0.7)
        .width('45%')
        .height('90%')
        .border({
          radius: 20,
        });
      }
      .width('100%').height('40%')
      .justifyContent(FlexAlign.SpaceEvenly); // 主轴对齐
    }
    .backgroundImage($r('app.media.secondbackground3'))
    .backgroundImageSize(ImageSize.Cover)
    .width('100%')
    .height('100%')
    .backgroundColor(Color.Orange)
    .justifyContent(FlexAlign.SpaceEvenly); // 主轴对齐
  }
}