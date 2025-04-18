/*
 * Copyright 2024 Unionman Technology Co., Ltd.
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
import promptAction from '@ohos.promptAction';
import control_demo from '@ohos.control_demo';

const GESTURE_CONTROL_STRING = new Map<number, string>();
GESTURE_CONTROL_STRING.set(0x01, "RIGHT")
GESTURE_CONTROL_STRING.set(0x02, "LEFT")
GESTURE_CONTROL_STRING.set(0x03, "BACK")
GESTURE_CONTROL_STRING.set(0x04, "FORWARD")
GESTURE_CONTROL_STRING.set(0x05, "PULLUP")
GESTURE_CONTROL_STRING.set(0x06, "PULLDOWN")
GESTURE_CONTROL_STRING.set(0x07, "PULLREMOVE")
GESTURE_CONTROL_STRING.set(0x21, "TOUCH1")
GESTURE_CONTROL_STRING.set(0x22, "TOUCH2")
GESTURE_CONTROL_STRING.set(0x23, "TOUCH3")
GESTURE_CONTROL_STRING.set(0x24, "TOUCH4")
GESTURE_CONTROL_STRING.set(0x25, "TOUCH5")

class UartData {
  id: number;
  data: number;
  desc: string | undefined;

  constructor(id: number, data: number, desc: string) {
    this.id = id;
    this.data = data;
    this.desc = desc;
  }
}

@Component
struct backgroundInit{
  @State message: string = 'PWM Control'
  build() {

    Stack({ alignContent: Alignment.Center }) {
      Stack({ alignContent: Alignment.Center }) {

        Image($rawfile('4.jpg'))// Image($r('app.media.icon'))
          .width('100%')
          .height('100%')
          .border({ width: 1 })
          .objectFit(ImageFit.Cover)
          .margin(15)
        // 保持宽高比进行缩小或者放大，使得图片两边都大于或等于显示边界。
        Text(this.message)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .fontColor(0xffffff)
          .offset({ x: 200, y: -50 })
      }
      Text("触摸2：舵机速度-10").fontSize(16).fontColor(0xFFFFFF).offset({ x: 200, y: 90 })
      Text("触摸1：舵机速度+10").fontSize(16).fontColor(0xFFFFFF).offset({ x: 200, y: 65 })
      Text("触摸3：舵机停转").fontSize(16).fontColor(0xFFFFFF).offset({ x: 200, y: 115 })
    }
  }
}

@Entry
@Component
struct Index {

  // 初始值0，舵机停转
  @State pwm_value: number = 0
  // 设置定时器
  @State interval: number = -1
  // 组件开关变量
  @State Panel_isOn: boolean = false
  @State Toggle_uart_get_isOn: boolean = false
  @State Toggle_uart_sleep_isOn: boolean = false
  // 串口数据index和数组
  @State uart_data_id: number = 0
  @State uart_data_arr: Array<UartData> = []
  private scroller: Scroller = new Scroller()

  @Builder
  uart_builder() {
    Column() {
      // 初始化设置
      Row() {
        Text("初始化").fontSize(20)

        Toggle({ type: ToggleType.Switch, isOn: false })
          .width(50)
          .height(50)
          .selectedColor(0x7db6bf)
          .onChange((isOn: Boolean) => {
            if (isOn) {
              let napi_str: number = control_demo.uart_init()
              if (napi_str == 0) {
                promptAction.showToast({ message: "初始化成功!" })
              } else {
                promptAction.showToast({ message: "初始化失败!" })
              }
            } else {
              this.Toggle_uart_get_isOn = false;
              this.Toggle_uart_sleep_isOn = false;
            }
          })
      }
      .width("30%")
      .border({ radius: 9 })
      .backgroundColor(0xFFFFFF)
      .justifyContent(FlexAlign.SpaceEvenly)

      Row() {
        // 循环检测组件
        Row() {
          Text("循环检测").fontSize(20)

          Toggle({ type: ToggleType.Switch, isOn: $$this.Toggle_uart_get_isOn })
            .width(50).height(50)
            .selectedColor(0x7db6bf)
            .onChange((isOn: Boolean) => {
              if (isOn) {
                this.interval = setInterval(() => {
                  control_demo.uart_get((result: number) => {
                    // 结果加载到串口日志
                    if (result > 0) {
                      this.uart_data_arr.push(new UartData(this.uart_data_id, result, "当前手势：" + GESTURE_CONTROL_STRING.get(result)))
                      this.scroller.scrollToIndex(this.uart_data_id) // 滑动到最底部
                      this.uart_data_id++; // idex加一
                    }
                    switch (result) {
                      case 0x21:
                        this.pwm_value += 10
                        control_demo.pwm_set(this.pwm_value)
                        break;
                      case 0x22:
                        this.pwm_value -= 10
                        control_demo.pwm_set(this.pwm_value)
                        break;
                      case 0x23:
                        this.pwm_value = 0
                        control_demo.pwm_set(this.pwm_value)
                        break;
                      default:
                        break;
                    }
                  })
                }, 1000);
              } else {
                clearInterval(this.interval);
              }
            })
        }
        .width("30%")
        .border({ radius: 9 })
        .backgroundColor(0xFFFFFF)
        .justifyContent(FlexAlign.SpaceEvenly)

        // 休眠组件
        Row() {
          Text("休眠").fontSize(20)

          Toggle({ type: ToggleType.Switch, isOn: $$this.Toggle_uart_sleep_isOn })
            .width(50).height(50)
            .selectedColor(0x7db6bf)
            .onChange((isOn: Boolean) => {
              if (isOn) {
                let ret: number = control_demo.uart_sleep(1);
                if (ret == 0) {
                  promptAction.showToast({ message: "休眠成功!唤醒需重新上电" })
                } else {
                  isOn = false;
                  promptAction.showToast({ message: "休眠失败!" })
                }
                this.Toggle_uart_get_isOn = false;
              }
            })
        }
        .width("30%")
        .border({ radius: 9 })
        .backgroundColor(0xFFFFFF)
        .justifyContent(FlexAlign.SpaceEvenly)
      }
      .width("100%")
      .justifyContent(FlexAlign.SpaceEvenly)
    }
  }
  build() {

    Stack({ alignContent: Alignment.Center }) {
      // 背景渲染
      backgroundInit({ message: "Pwm Control" })
      //pwm控制
      Row() {
        Slider({
          value: this.pwm_value,
          min: -50,
          max: 50,
          step: 5,
          style: SliderStyle.InSet,
          direction: Axis.Horizontal,
          reverse: false
        })
          .showSteps(true)
          .width(200).height(60)
          .trackColor(0x3c4c4d)
          .selectedColor(0x7db6bf)
          .onChange((value: number, mode: SliderChangeMode) => {
            this.pwm_value = value
            control_demo.pwm_set(value)
          })
        Text(this.pwm_value.toFixed(0)).fontSize(16).fontColor(0xffffff)
      }.width(300).height(70).offset({ x: 250, y: 30 })

      //set
      Button("手势传感器设置", { type: ButtonType.Normal, stateEffect: true })
        .onClick(() => {
          this.Panel_isOn = true
        })
        .borderRadius(5)
        .backgroundColor(0x7db6bf)
        .offset({ x: -200, y: -145 })
      // 串口日志
      Column({ space: 3 }) {
        Text("串口日志").fontSize(20)
        List({ space: 2, scroller: this.scroller }) {
          ForEach(this.uart_data_arr, (item: UartData) => {
            ListItem() {
              Text(item.desc).fontSize(16)
            }
          }, (item: UartData) => item.id.toString())

        }
        .width("80%").height("80%")
        .alignListItem(ListItemAlign.Start)
        .scrollBar(BarState.On)
        .edgeEffect(EdgeEffect.Spring)
        .sticky(StickyStyle.Footer)
      }
      .offset({ x: -200, y: 25 })
      .width(200).height(250)
      .backgroundColor(0xA1A3B4)
      .border({ radius: 9 })
      .opacity(0.8)
      .justifyContent(FlexAlign.Center)

      // 传感器设置窗口
      Panel(this.Panel_isOn) {
        this.uart_builder()
      }
      .opacity(0.9)
      .backgroundColor(0xA1A3B4)
      .type(PanelType.Temporary)
      .mode(PanelMode.Half)
      .dragBar(true)
      .fullHeight(250)
      .onChange((width: number, height: number, mode: PanelMode) => {
        if (height < 0) {
          this.Panel_isOn = false
        }
      })
    }.width('100%').height('100%').margin({ top: 0 })
  }
}