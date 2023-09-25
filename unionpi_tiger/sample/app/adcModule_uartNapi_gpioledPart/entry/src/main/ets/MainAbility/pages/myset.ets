/*
 * Copyright 2023 Unionman Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import router from '@ohos.router';
import prompt from '@ohos.prompt';

@Entry
@Component
struct Myset {
  @State mode:boolean = true//只有state变量才能给check赋值
  @State clickAble: boolean = true
  //带local的变量是用来不保存返回时传递值的
  local_mode:boolean;
  local_lowthreshold:number;
  local_highthreshold:number;
  local_lowthreshold_led:number;
  local_highthreshold_led:number;
  @State lowthreshold:number=0;
  @State highthreshold:number=0;
  lowthreshold_led:number;
  highthreshold_led:number;
  onPageShow()
  {
    this.mode=router.getParams()['mode'];
    this.lowthreshold=router.getParams()['lowthreshold'];
    this.highthreshold=router.getParams()['highthreshold'];
    this.lowthreshold_led=router.getParams()['lowthreshold_led'];
    this.highthreshold_led=router.getParams()['highthreshold_led'];
    console.log("highthreshold的值为："+this.highthreshold);
    console.log("high_led的值为:"+this.highthreshold_led)
    this.local_mode=this.mode;
    this.local_lowthreshold=this.lowthreshold;
    this.local_lowthreshold_led=this.lowthreshold_led;
    this.local_highthreshold=this.highthreshold;
    this.local_highthreshold_led=this.highthreshold_led;
    console.log("在设置界面的onpageshow,拿到了mode为："+this.mode)
  }
  save()
  {
    router.replace({
      url:'pages/myMenu',
      params: {
        mode: this.mode,
        lowthreshold:this.lowthreshold,
        highthreshold:this.highthreshold,
        low_led:this.lowthreshold_led,
        high_led:this.highthreshold_led,
      },
    })
  }
  back()
  {
    router.replace({
      url:'pages/myMenu',
      params: {
        mode: this.local_mode,
        lowthreshold:this.local_lowthreshold,
        highthreshold:this.local_highthreshold,
        low_led:this.local_lowthreshold_led,
        high_led:this.local_highthreshold_led,
      },
    })
  }
  build() {
    Column() {
      Row() {
        Row() {
          Image($r('app.media.icon_back'))
            .width(24)
            .height(24)
        }
        .padding({ left: 2, right: 17, top: 10, bottom: 10 })
        .onClick(() => {
          this.back()
        })
        Blank()
        Button($r('app.string.save'))
          .fontSize(22)
          .backgroundColor(this.clickAble ? '#4F7EFD' : "#C0CFF7")
          .height(44)
          .borderRadius(7)
          .padding({
            left: 14,
            right: 14,
          })
          .margin({ bottom: 4 })
          .enabled(this.clickAble)
          .onClick(() => {
            this.save()
          })
          .type(ButtonType.Normal)

      }
      .width('100%')
      .height(54)
      .margin({
        top: 5
      })
      Row() {
        Column() {
          Text("温度单位:")
            .fontSize(50)
            .fontWeight(FontWeight.Bold)
            .textAlign(TextAlign.Start)
          Row() {
            Column() {
              Radio({ value: 'centigrade', group: 'tempchoice' })
                .checked(this.mode)
                .width(50)
                .onChange((isChecked: boolean) => {
                  if (isChecked) {
                    // 切换为摄氏度模式
                    prompt.showToast({ message: 'centigrade mode.' })
                    this.mode=true;
                  }
                })
              Text('°C')
            }
            Column() {
              Radio({ value: 'Fahrenheit', group: 'tempchoice' })
                .checked(!this.mode)
                .width(50)
                .onChange((isChecked: boolean) => {
                  if (isChecked) {
                    // 切换为华氏度模式
                    prompt.showToast({ message: 'Fahrenheit mode.' })
                    this.mode=false;
                  }
                })
              Text('°F')
            }
          }
        }
        .margin({top:80})
        Blank()
        Column() {
          Row() {
            Text("低温阈值:")
              .fontSize(50)
              .fontWeight(FontWeight.Bold)
            TextInput({placeholder:this.lowthreshold.toString()})
              .type(InputType.Number)
              .width(100)
              .backgroundColor(Color.Gray)
              .onChange((value:string)=>{
                this.lowthreshold=+value
              })
          }
          .margin({ top: 30 })

          Select([{ value: 'GPIO1' },
            { value: 'GPIO2' },
            { value: 'GPIO3' },
            { value: 'GPIO4' },
            { value: 'GPIO5' },
            { value: 'GPIO6' },
            { value: 'GPIO7' },
            { value: 'GPIO8' },
            { value: 'GPIO9' },
            { value: 'GPIO10' },
            { value: 'GPIO11' },
            { value: 'GPIO12' },
            { value: 'GPIO13' },
            { value: 'GPIO14' },
            { value: 'GPIO15' },
            { value: 'GPIO16' },
          ])
            .selected(this.lowthreshold_led-1)
            .value('selected led')
            .font({ size: 30, weight: 500 })
            .selectedOptionFont({ size: 30, weight: 400 })
            .optionFont({ size: 30, weight: 400 })
            .onSelect((index: number) => {
              this.lowthreshold_led=index+1;
            })

          Row() {
            Text("高温阈值:")
              .fontSize(50)
              .fontWeight(FontWeight.Bold)
            TextInput({placeholder:this.highthreshold.toString()})
              .type(InputType.Number)
              .width(100)
              .backgroundColor(Color.Grey)
              .onChange((value:string)=>{
                this.highthreshold=+value
              })
          }
          .margin({top:10})

          Select([{ value: 'GPIO1' },
            { value: 'GPIO2' },
            { value: 'GPIO3' },
            { value: 'GPIO4' },
            { value: 'GPIO5' },
            { value: 'GPIO6' },
            { value: 'GPIO7' },
            { value: 'GPIO8' },
            { value: 'GPIO9' },
            { value: 'GPIO10' },
            { value: 'GPIO11' },
            { value: 'GPIO12' },
            { value: 'GPIO13' },
            { value: 'GPIO14' },
            { value: 'GPIO15' },
            { value: 'GPIO16' },
          ])
            .selected(this.highthreshold_led-1)
            .value('selected led')
            .font({ size: 30, weight: 500 })
            .selectedOptionFont({ size: 30, weight: 400 })
            .optionFont({ size: 30, weight: 400 })
            .onSelect((index: number) => {
              this.highthreshold_led=index+1;
            })
        }
      }
      .width('90%')
  }
    .height('100%')
    .width('100%')
    .backgroundImage($r('app.media.bg1')).backgroundImageSize(ImageSize.Cover)
  }
}