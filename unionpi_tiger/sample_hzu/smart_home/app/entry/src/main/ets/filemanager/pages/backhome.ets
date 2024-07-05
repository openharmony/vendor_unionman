import router from '@ohos.router';

@Entry
@Component
struct Index {
  @State handlePopup: boolean = false
  @StorageLink('backHomeLightControl') backLightControl: number = 0; //等于0时候代表关，等于1时候代表开
  @StorageLink('backHomeLight0') backHomeLight0: number = 1; //等于1时候代表关，等于0时候代表开
  @StorageLink('backHomeLight1') backHomeLight1: number = 1;
  @StorageLink('backHomeLight2') backHomeLight2: number = 1;
  @StorageLink('backHomeLight3') backHomeLight3: number = 1;
  @StorageLink('backHomeTime') backHomeTime: number = 0;
  @State hour: number = 0;
  @State minute: number = 0;
  @State isMilitaryTime: boolean = true;
  private selectedTime: Date = new Date('2022-07-22T18:30:00'); //默认时间

  build() {
    Row() {
      Column() {
        Text($r('app.string.backHomeSelect'))
          .fontColor(Color.White)
          .size({ width: 280, height: '50' })
          .backgroundColor($r('app.color.openLightBack'))
          .fontSize(28)
          .fontWeight(FontWeight.Bold)
          .offset({ x: '0%', y: '0%' })
          .textAlign(TextAlign.Center)//文字居中
          .border({ radius: 5 });
        Stack({}) {
          Text()
            .size({ width: 400, height: 270 })
            .offset({ x: '0%', y: '5%' })
            .border({ radius: 15, style: BorderStyle.Dotted, width: 6, color: 0x895b8a });
          Column({ space: 25 }) {
            Row({ space: 20 }) {
              if (this.backHomeLight0 % 2 == 0) {
                Button({ type: ButtonType.Normal, stateEffect: true }) {
                  LoadingProgress()
                    .width(20)
                    .height(20)
                    .color($r('app.color.start_window_background'));
                }
                .border({ radius: 15 })
                .backgroundColor($r('app.color.sliderClose'))
                .opacity(0.7)
                .width(180)
                .height(100)
                //.offset({ x: '0%', y: '10%' })
                .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.7 })
                .onClick(() => {
                  this.backHomeLight0++;
                });
              } else if (this.backHomeLight0 % 2 == 1) {
                Button({ type: ButtonType.Normal, stateEffect: true }) {
                  Text($r('app.string.frontLiving'))
                    .textAlign(TextAlign.Center)
                    .fontSize(25)
                    .fontWeight(FontWeight.Bold);
                }
                .border({ radius: 15 })
                .width(180)
                .height(100)
                .backgroundColor($r('app.color.openLight'))
                .opacity(0.5)
                .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.7 })
                .onClick(() => {
                  this.backHomeLight0--;
                });
              }
              if (this.backHomeLight1 % 2 == 0) {
                Button({ type: ButtonType.Normal, stateEffect: true }) {
                  LoadingProgress()
                    .width(20)
                    .height(20)
                    .color($r('app.color.start_window_background'));
                }
                .border({ radius: 15 })
                .backgroundColor($r('app.color.sliderClose'))
                .opacity(0.7)
                .width(180)
                .height(100)
                .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.7 })
                .onClick(() => {
                  this.backHomeLight1++;
                });
              } else if (this.backHomeLight1 % 2 == 1) {
                Button({ type: ButtonType.Normal, stateEffect: true }) {
                  Text($r('app.string.behindLiving'))
                    .fontSize(25)
                    .fontWeight(FontWeight.Bold)
                    .textAlign(TextAlign.Center);
                }
                .border({ radius: 15 })
                .width(180)
                .height(100)
                .backgroundColor($r('app.color.openLight'))
                .opacity(0.5)
                .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.7 })
                .onClick(() => {
                  this.backHomeLight1--;
                });
              }
            }

            Row({ space: 20 }) {
              if (this.backHomeLight2 % 2 == 0) {
                Button({ type: ButtonType.Normal, stateEffect: true }) {
                  LoadingProgress()
                    .width(20)
                    .height(20)
                    .color($r('app.color.start_window_background'));
                }
                .border({ radius: 15 })
                .backgroundColor($r('app.color.sliderClose'))
                .opacity(0.7)
                .width(180)
                .height(100)
                .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.7 })
                .onClick(() => {
                  this.backHomeLight2++;
                });
              } else if (this.backHomeLight2 % 2 == 1) {
                Button({ type: ButtonType.Normal, stateEffect: true }) {
                  Text($r('app.string.bedroom'))
                    .fontSize(25)
                    .fontWeight(FontWeight.Bold)
                    .textAlign(TextAlign.Center);
                }
                .border({ radius: 15 })
                .width(180)
                .height(100)
                .backgroundColor($r('app.color.openLight'))
                .opacity(0.5)
                .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.7 })
                .onClick(() => {
                  this.backHomeLight2--;
                });
              }
              if (this.backHomeLight3 % 2 == 0) {
                Button({ type: ButtonType.Normal, stateEffect: true }) {
                  LoadingProgress()
                    .width(20)
                    .height(20)
                    .color($r('app.color.start_window_background'));
                }
                .border({ radius: 15 })
                .backgroundColor($r('app.color.sliderClose'))
                .opacity(0.7)
                .width(180)
                .height(100)
                .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.7 })
                .onClick(() => {
                  this.backHomeLight3++;
                });
              } else if (this.backHomeLight3 % 2 == 1) {
                Button({ type: ButtonType.Normal, stateEffect: true }) {
                  Text($r('app.string.kitchen'))
                    .fontSize(25)
                    .fontWeight(FontWeight.Bold)
                    .textAlign(TextAlign.Center);
                }
                .border({ radius: 15 })
                .width(180)
                .height(100)
                .backgroundColor($r('app.color.openLight'))
                .opacity(0.5)
                .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.7 })
                .onClick(() => {
                  this.backHomeLight3--;
                });
              }
            }
          }
          .offset({ x: '0%', y: '4%' });
        }
      }
      .offset({ x: '2%', y: '0%' })
      .width('50%')
      .height('75%');

      //.backgroundColor(Color.Green)
      Column({ space: 15 }) {
        Text($r('app.string.openSelectTime'))
          .fontColor(Color.White)
          .size({ width: 280, height: '50' })
          .backgroundColor($r('app.color.openLightBack'))
          .fontSize(28)
          .fontWeight(FontWeight.Bold)
          .offset({ x: '0%', y: '0%' })
          .textAlign(TextAlign.Center)//文字居中
          .border({ radius: 5 });
        Stack() {
          // 框
          Text()
            .backgroundColor(Color.White)
            .size({ width: 350, height: 250 })
            .offset({ x: '0%', y: '0%' })
            .opacity(0.3)
            .border({ radius: 15, style: BorderStyle.Dotted, width: 5, color: $r('app.color.textBackground') });
          // 时间滑动选择
          TimePicker({
            selected: this.selectedTime,
          })
            .backgroundColor(Color.White)
            .opacity(0.7)
            .useMilitaryTime(this.isMilitaryTime)
            .onChange((value: TimePickerResult) => {
              this.selectedTime.setHours(value.hour, value.minute);
            })
            .textStyle({ color: Color.Black, font: { size: 25, weight: FontWeight.Normal } })
            .selectedTextStyle({ color: $r('app.color.pickerColor'), font: { size: 30, weight: FontWeight.Bolder } })
            .width(250);
        }

        // 返回按钮
        Row({ space: 45 }) {
          Button() {
            Text($r('app.string.return'))
              .textAlign(TextAlign.Center)
              .fontColor(Color.White)
              .fontWeight(FontWeight.Bold)
              .fontSize(28);
          }
          .backgroundColor($r('app.color.returnColor'))
          .offset({ x: '0%', y: '0%' })
          .width(90)
          .height(45)
          .onClick(() => {
            router.pushUrl({ url: 'filemanager/pages/intelligentscenario' });
          });
          // 更新按钮
          Button() {
            Text($r('app.string.refresh'))
              .textAlign(TextAlign.Center)
              .fontColor(Color.White)
              .fontWeight(FontWeight.Bold)
              .fontSize(28);
          }
          .backgroundColor($r('app.color.returnColor'))
          .offset({ x: '0%', y: '0%' })
          .width(90)
          .height(45)
          .onClick((event: ClickEvent) => {
            this.hour = this.selectedTime.getHours();
            this.minute = this.selectedTime.getMinutes();
            this.backHomeTime = (86400 + this.hour * 3600 + this.minute * 60 - 28800) % 86400;
            this.handlePopup = !this.handlePopup;
          })
          .bindPopup(this.handlePopup, {
            message: 'The lighting time has been updated',
          });

          if (this.backLightControl == 0) {
            Button() {
              Text($r('app.string.closeBackHome'))
                .textAlign(TextAlign.Center)
                .fontColor(Color.Black)
                .fontWeight(FontWeight.Bold)
                .fontSize(28);
            }
            .backgroundColor(Color.Gray)
            .offset({ x: '0%', y: '0%' })
            .width(220)
            .height(45)
            .onClick(() => {
              this.backLightControl++;
            });
          } else if (this.backLightControl == 1) {
            Button() {
              Text($r('app.string.openBackHome'))
                .textAlign(TextAlign.Center)
                .fontColor(Color.White)
                .fontWeight(FontWeight.Bold)
                .fontSize(28);
            }
            .backgroundColor($r('app.color.sliderClose'))
            .offset({ x: '0%', y: '0%' })
            .width(220)
            .height(45)
            .onClick(() => {
              this.backLightControl--;
            });
          }
        }
        .opacity(0.8)
        .offset({ x: '0%', y: '5%' });
      }
      .width('50%')
      .height('75%')
      .offset({ x: '-2%', y: '0%' });

      //.backgroundColor(Color.Red)
    }
    .backgroundImage($r('app.media.secondbackground3'))
    .backgroundImageSize(ImageSize.Cover)
    .height('100%');
  }
}