import router from '@ohos.router';

@Entry
@Component
struct Index {
  @State handlePopup: boolean = false
  @StorageLink('leaveHomeLightControl') leaveLightControl: number = 0; //等于0时候代表关，等于1时候代表开
  @StorageLink('leaveHomeLight0') leaveHomeLight0: number = 1; //等于1时候代表关，等于0时候代表开
  @StorageLink('leaveHomeLight1') leaveHomeLight1: number = 1;
  @StorageLink('leaveHomeLight2') leaveHomeLight2: number = 1;
  @StorageLink('leaveHomeLight3') leaveHomeLight3: number = 1;
  @StorageLink('backHomeTime') backHomeTime: number = 0;
  @StorageLink('leaveHomeTime') leaveHomeTime: number = 0;
  @State hour: number = 0;
  @State minute: number = 0;
  @State isMilitaryTime: boolean = true;
  private selectedTime: Date = new Date('2022-07-22T08:30:00'); // 默认时间

  build() {
    Row() {
      Column() {
        Text($r('app.string.leftHomeSelect'))
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
              if (this.leaveHomeLight0 % 2 == 0) {
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
                  this.leaveHomeLight0++;
                })
              } else if (this.leaveHomeLight0 % 2 == 1) {
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
                //.offset({ x: '0%', y: '10%' })
                .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.7 })
                .onClick(() => {
                  this.leaveHomeLight0--;
                });
              }
              if (this.leaveHomeLight1 % 2 == 0) {
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
                  this.leaveHomeLight1++;
                });
              } else if (this.leaveHomeLight1 % 2 == 1) {
                Button({ type: ButtonType.Normal, stateEffect: true }) {
                  Text($r('app.string.behindLiving'))
                    .textAlign(TextAlign.Center)
                    .fontSize(25)
                    .fontWeight(FontWeight.Bold);
                }
                .border({ radius: 15 })
                .width(180)
                .height(100)
                .backgroundColor($r('app.color.openLight'))
                .opacity(0.5)
                //.offset({ x: '0%', y: '10%' })
                .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.7 })
                .onClick(() => {
                  this.leaveHomeLight1--;
                });
              }
            }

            Row({ space: 20 }) {
              if (this.leaveHomeLight2 % 2 == 0) {
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
                  this.leaveHomeLight2++;
                });
              } else if (this.leaveHomeLight2 % 2 == 1) {
                Button({ type: ButtonType.Normal, stateEffect: true }) {
                  Text($r('app.string.bedroom'))
                    .textAlign(TextAlign.Center)
                    .fontSize(25)
                    .fontWeight(FontWeight.Bold)
                }
                .border({ radius: 15 })
                .width(180)
                .height(100)
                .backgroundColor($r('app.color.openLight'))
                .opacity(0.5)
                //.offset({ x: '0%', y: '10%' })
                .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.7 })
                .onClick(() => {
                  this.leaveHomeLight2--;
                });
              }
              if (this.leaveHomeLight3 % 2 == 0) {
                Button({ type: ButtonType.Normal, stateEffect: true }) {
                  LoadingProgress()
                    .width(20)
                    .height(20)
                    .color($r('app.color.start_window_background'))
                }
                .border({ radius: 15 })
                .backgroundColor($r('app.color.sliderClose'))
                .opacity(0.7)
                .width(180)
                .height(100)
                //.offset({ x: '0%', y: '10%' })
                .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.7 })
                .onClick(() => {
                  this.leaveHomeLight3++;
                });
              } else if (this.leaveHomeLight3 % 2 == 1) {
                Button({ type: ButtonType.Normal, stateEffect: true }) {
                  Text($r('app.string.kitchen'))
                    .textAlign(TextAlign.Center)
                    .fontSize(25)
                    .fontWeight(FontWeight.Bold);
                }
                .border({ radius: 15 })
                .width(180)
                .height(100)
                .backgroundColor($r('app.color.openLight'))
                .opacity(0.5)
                //.offset({ x: '0%', y: '10%' })
                .clickEffect({ level: ClickEffectLevel.LIGHT, scale: 0.7 })
                .onClick(() => {
                  this.leaveHomeLight3--;
                });
              }
            }
          }
          .offset({ x: '0%', y: '4%' })
        }
      }
      .offset({ x: '2%', y: '0%' })
      .width('50%')
      .height('75%');

      //.backgroundColor(Color.Green)
      Column({ space: 15 }) {
        Text($r('app.string.closeSelectTime'))
          .fontColor(Color.White)
          .size({ width: 280, height: '50' })
          .backgroundColor($r('app.color.openLightBack'))
          .fontSize(28)
          .fontWeight(FontWeight.Bold)
          .offset({ x: '0%', y: '0%' })
          .textAlign(TextAlign.Center)//文字居中
          .border({ radius: 5 });
        Stack() {
          Text()
            .backgroundColor(Color.White)
            .size({ width: 350, height: 250 })
            .offset({ x: '0%', y: '0%' })
            .opacity(0.3)
            .border({ radius: 15, style: BorderStyle.Dotted, width: 5, color: $r('app.color.textBackground') });
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
            .width(300);
        }

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
            this.leaveHomeTime = (86400 + this.hour * 3600 + this.minute * 60 - 28800) % 86400;
            this.handlePopup = !this.handlePopup;
          })
          .bindPopup(this.handlePopup, {
            message: 'The light off time has been updated',
          });
          if (this.leaveLightControl == 0) {
            Button() {
              Text($r('app.string.closeLeftHome'))
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
              this.leaveLightControl++;
            });
          } else if (this.leaveLightControl == 1) {
            Button() {
              Text($r('app.string.openLeftHome'))
                .textAlign(TextAlign.Center)
                .fontColor(Color.White)
                .fontWeight(FontWeight.Bold)
                .fontSize(28)
            }
            .backgroundColor($r('app.color.sliderClose'))
            .offset({ x: '0%', y: '0%' })
            .width(220)
            .height(45)
            .onClick(() => {
              this.leaveLightControl--;
            });
          }
        }
        .opacity(0.8)
        .offset({ x: '0%', y: '5%' });
      }
      .width('50%')
      .height('75%')
      .offset({ x: '-2%', y: '0%' });
    }
    .backgroundImage($r('app.media.secondbackground3'))
    .backgroundImageSize(ImageSize.Cover)
    .height('100%');
  }
}