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

import router from '@ohos.router';
import i2cnapidemo from '@ohos.i2cnapidemo';
import prompt from '@ohos.prompt';

@Extend(Button) function ButtonStyle () {
  .fontSize(25)
  .fontWeight(FontWeight.Bold)
  .fontStyle(FontStyle.Italic)
  .type(ButtonType.Capsule)
  .backgroundColor('#0D9FFB')
}

@Entry
@Component
struct Second {
  @State ReadTemperatureC: number = 0 ;
  @State ReadHumidity: number = 0 ;
  @State ReadTemperatureF: number = 0 ;
  @State message: string = '使用教程:一.启动/重启 二.滑动选择后按下模式选择 三.获取'
  private select1: number = 3
  private Mps: string[] = ['Mps:0.5Hz', 'Mps:1Hz', 'Mps:2Hz', 'Mps:4Hz', 'Mps:10Hz']
  private select2: number = 1
  private Repeatability: string[] = ['刷新率:低', '刷新率:中', '刷新率:高']

  build() {
    Column() {
      Row() {
        Column() {
          Row() {
            TextPicker({ range: this.Mps, selected: this.select1 })
              .onChange((value: string, index: number) => {
                console.info('Picker item changed, value: ' + value + ', index: ' + index)
                this.select1 = index;
              })
              .width('50%')
              .height('100%')
              .margin({
                bottom: 10
              })

            TextPicker({ range: this.Repeatability, selected: this.select2 })
              .onChange((value: string, index: number) => {
                console.info('Picker item changed, value: ' + value + ', index: ' + index)
                this.select2 = index;
              })

              .margin({
                bottom: 10
              })
              .width('50%')
              .height('100%')
          }
          .height('50%')
        }
        .width('50%')

        Column() {
          Text("温度：" + this.ReadTemperatureC.toFixed(4) + "°C\n" +
          "湿度：" + this.ReadHumidity.toFixed(4) + "%RH\n" +
          "温度：" + this.ReadTemperatureF.toFixed(4) + "°F")
            .fontSize(35)
            .fontWeight(FontWeight.Bold)
            .textAlign(TextAlign.Start)
        }
        .width('50%')
      }
      .height('60%')

      Row() {
        Text(this.message)
          .fontSize(25)
          .fontWeight(FontWeight.Bold)
          .margin({
            bottom: 25
          })
      }

      Row() {
        Button('启动/重启')
          .ButtonStyle()
          .margin({
            right: 40
          })
          .width('20%')
          .height('10%')
          .onClick(() => {
            if (i2cnapidemo.ReadTemperatureC() <= 800) {
              i2cnapidemo.SoftReset();
              prompt.showToast({
                message: "启动/重启成功", // 显示文本
                duration: 500, // 显示时长
                bottom: 100 // 距离底部的距离
              })
            }
            else {
              prompt.showToast({
                message: "启动失败，请检查连线", // 显示文本
                duration: 1000, // 显示时长
                bottom: 100 // 距离底部的距离
              })
            }
          })

        Button('模式选择')
          .ButtonStyle()
          .margin({
            right: 70
          })
          .width('20%')
          .height('10%')
          .onClick(() => {
            if (i2cnapidemo.ReadTemperatureC() <= 800) {
              i2cnapidemo.ModeSet(this.select1, this.select2);
              prompt.showToast({
                message: "模式选择成功", // 显示文本
                duration: 500, // 显示时长
                bottom: 100 // 距离底部的距离
              })
            }
            else {
              prompt.showToast({
                message: "模式选择失败，请检查连线", // 显示文本
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
          .onClick(() => {
            if (i2cnapidemo.ReadTemperatureC() <= 800) {
              i2cnapidemo.ReadData();
              this.ReadTemperatureC = i2cnapidemo.ReadTemperatureC();
              this.ReadHumidity = i2cnapidemo.ReadHumidity();
              this.ReadTemperatureF = i2cnapidemo.ReadTemperatureF();
              console.info(" i2cvalue " + this.ReadTemperatureC + "\n" + this.ReadHumidity + "\n" + this.ReadTemperatureF);
              prompt.showToast({
                message: "获取成功", // 显示文本
                duration: 500, // 显示时长
                bottom: 100 // 距离底部的距离
              })
            }
            else {
              prompt.showToast({
                message: "获取失败，请检查连线", // 显示文本
                duration: 1000, // 显示时长
                bottom: 100 // 距离底部的距离
              })
            }
          })

        Button("返回")
          .ButtonStyle()
          .width('16%')
          .height('10%')
            // 返回按钮绑定onClick事件，点击按钮时返回到第一页
          .onClick(() => {
            router.back()
          })
      }
    }
  }
}
