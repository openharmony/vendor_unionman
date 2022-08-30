/* Copyright 2022 Unionman Technology Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// @ts-nocheck
import pwmtest from '@ohos.pwmtest';
import prompt from '@ohos.prompt';

@Component
struct TextForm {
  @State textContent: string = 1

  build() {
    Text(this.textContent)
      .textOverflow({ overflow: TextOverflow.Clip })
      .fontSize(21)
      .fontColor(Color.Orange)
      .fontWeight(700)
  }
}

@Entry
@Component
struct MarqueeExample {
  // 开关标记位
  @State start: boolean = true
  // 舵机的角度
  @State pwm_Angle: number = 0
  // 舵机的引脚
  @State Pex: number = 1
  // 开关按钮显示的文本
  @State power: string = 'ON'
  @State mid: number = 1
  private select: number = 0
  private pwm_pex: string[] = ['PWM_1', 'PWM_2']

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Divider().vertical(false).strokeWidth(5).color(Color.Gray).lineCap(LineCapStyle.Round)
      Row() {
        Column() {
          // 选择引脚：PWM_1、PWM_2（默认为PWM_1）
          TextPicker({ range: this.pwm_pex, selected: this.Pex - 1 })
            .onChange((value: string, index: number) => {
              this.mid = index + 1;
              console.info('Picker item changed, value: ' + value + ', index: ' + index)
            }).width('60%').margin({ right: 20, left: 60 }).height('40%')

          Row() {
            // 舵机开关：默认为开
            Button(this.power, { type: ButtonType.Capsule, stateEffect: true })
              .backgroundColor(Color.Gray)
              .margin({ top: 10, right: 20, left: 60, bottom: 20 })
              .width(90)
              .onClick(() => {
                // 开关标记为进行判断
                if (this.start) {
                  prompt.showToast({
                    message: "关闭舵机", // 显示文本
                  })
                  this.power = "OFF"
                } else {
                  prompt.showToast({
                    message: "打开舵机", // 显示文本
                  })
                  this.power = "ON"
                }
                // 开关标记位取反
                this.start = !this.start
                console.info("power: " + this.power + " ,start:   " + this.start)
              })
            // 确定选择引脚
            Button('Set', { type: ButtonType.Capsule, stateEffect: true })
              .margin({ top: 10, bottom: 20 })
              .backgroundColor(Color.Gray)
              .width(90)
              .onClick(() => {
                this.Pex = this.mid
                prompt.showToast({
                  message: "选择" + this.Pex + "号引脚", // 显示文本
                })
              })
          }.height('30%')

          // 按钮功能提示
          Text('ON 按钮控制舵机的开关')
            .textOverflow({ overflow: TextOverflow.Clip })
            .fontSize(18)
            .fontColor(Color.Gray)
            .fontWeight(700)
            .margin({ left: 50, top: 7 })
          Text('Set 按钮控制舵机的引脚')
            .textOverflow({ overflow: TextOverflow.Clip })
            .fontSize(18)
            .fontColor(Color.Gray)
            .fontWeight(700)
            .margin({ left: 50, top: 14 })
        }.width('50%')

        Column() {
          Row() {
            // 根据设置的角度，模拟舵机转动角度
            Gauge({ value: 0, min: -90, max: 90 })
              .startAngle(-90)
              .endAngle(90)
              .colors([[Color.Gray, 1]])
              .strokeWidth(30)
              .value(this.pwm_Angle - 90)
              .height('60%')
              .width('100%')
              .margin({ top: 20 })
          }

          TextForm({ textContent: this.pwm_Angle.toFixed(0) + '°' })

          Row() {
            // 设置舵机的转动角度
            Slider({
              value: this.pwm_Angle,
              min: 0,
              max: 180,
              step: 1,
              style: SliderStyle.OutSet
            })
              .width('60%')
              .blockColor(Color.Gray)
              .trackColor(Color.Gray)
              .selectedColor(0x317aff)
              .showSteps(true)
              .showTips(true)
              .onChange((value: number) => {
                // 根据开关的标识位，来选择是否转动舵机
                if (this.start) {
                  this.pwm_Angle = value
                  console.info("onChange pwm_Angle = " + this.pwm_Angle)
                  pwmtest.setPwmStatus(this.Pex, this.pwm_Angle)
                }
              })
          }
        }.width('50%')
      }.height('70%')

      // 显示舵机的引脚和角度信息
      Row() {
        TextForm({ textContent: 'The selected pin is PWM_' + this.Pex + ", " })
        TextForm({ textContent: 'and the Angle is ' + this.pwm_Angle.toFixed(0) + '°!' })
      }.height('20%')

      Divider().vertical(false).strokeWidth(5).color(Color.Gray).lineCap(LineCapStyle.Round)
    }.backgroundImage($r("app.media.bg3")).backgroundImageSize(ImageSize.Cover)
  }
}
