/* Copyright 2023 Unionman Technology Co., Ltd.
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
import { BusinessError } from '@ohos.base';
import gpioled from '@ohos.gpioled';
import adc_napi from '@ohos.adc_napi';

@Entry
@Component
struct Light {
  private ledPort = 385;
  private adcThreshold = 3100; // 设定的光敏电阻 ADC 值阈值
  private autoModeInterval: number | undefined;

  @State private isOn: boolean = false;
  @State private isAutoMode: boolean = false;
  @State private imagePath: Resource = $r('app.media.light_off'); // 中间变量存储图片路径

  private onImage: Resource = $r('app.media.light_on');
  private offImage: Resource = $r('app.media.light_off');

  // 页面出现时执行的操作
  aboutToAppear() {
    // 从 AppStorage 中获取 LED 状态和自动模式状态
    const storedState = AppStorage.get<boolean>('isON');
    const storedAutoMode = AppStorage.get<boolean>('isAutoMode');
    console.log('从 AppStorage 获取的值: ' + storedState);
    this.isOn = storedState ?? false;
    this.isAutoMode = storedAutoMode ?? false;
    this.setLedStatus(this.isOn);
    this.updateImagePath(); // 更新图片路径
    console.log('获取上次关闭时的LED值: ' + this.isOn);
    console.log('获取上次关闭时的自动模式值: ' + this.isAutoMode);

    // 获取 ADC 值并打印日志
    this.getAdcValueAndLog();

    // 如果自动模式开启，启动定时器
    if (this.isAutoMode) {
      this.startAutoMode();
    }
  }

  // 页面消失时执行的操作
  aboutToDisappear() {
    // 页面消失时停止自动模式
    if (this.autoModeInterval) {
      clearInterval(this.autoModeInterval);
    }
  }

  build() {
    Column({ space: 20 }) {
      // 返回按钮行
      Row({ space: 10 }) {
        Button('<', { type: ButtonType.Circle, stateEffect: true })
          .width(50)
          .height(50)
          .fontSize(40)
          .fontColor(Color.Black)
          .backgroundColor(Color.White)
          .onClick(() => {
            try {
              // 返回第一页
              router.back();
            } catch (err) {
              let code = (err as BusinessError).code;
              let message = (err as BusinessError).message;
              console.error(`Failed to return to the first page. Code is ${code}, message is ${message}`);
            }
          });
      }
      .width('100%')
      .height('10%')
      .padding(10)
      .justifyContent(FlexAlign.Start)
      .alignItems(VerticalAlign.Center)
      .backgroundColor(Color.White); // 返回按钮行背景颜色

      // 主内容行，包含图片和控制容器
      Row({ space: 20 }) {
        // 左侧图片容器
        Column({ space: 10 }) {
          Image(this.imagePath) // 使用中间变量存储的图片路径
            .width('100%')  // 调整图片宽度
            .height('100%') // 调整图片高度
            .borderRadius(20)
            .interpolation(ImageInterpolation.High)
            .padding(10)
            .border({ width: '2px', color: '#FFFFFF' })
            .shadow({ radius: 10, color: Color.Gray });
        }
        .width('40%')
        .height('100%')
        .justifyContent(FlexAlign.Center)
        .alignItems(HorizontalAlign.Center)
        .backgroundColor(Color.White);

        // 右侧控制容器
        Column({ space: 20 }) {
          // 模式选择列
          Column({ space: 10 }) {
            Text('模式选择：')
              .fontSize(60)
              .fontColor(Color.Black)
              .textShadow({ radius: 10, color: Color.White, offsetX: 0, offsetY: 0 });

            Row({ space: 10 }) {
              Text('手动')
                .fontColor(Color.Black)
                .fontSize(40);

              // 使用 isAutoMode 属性来绑定状态
              Toggle({ type: ToggleType.Switch, isOn: this.isAutoMode })
                .margin({ top: 6, right: 14, bottom: 6, left: 14 })
                .selectedColor(Color.Blue)
                .onChange((isAutoMode: boolean) => {
                  this.isAutoMode = isAutoMode;
                  AppStorage.setOrCreate('isAutoMode', isAutoMode);
                  console.log('存储 ' + (isAutoMode ? '自动' : '手动') + ' 模式到缓存');
                  if (isAutoMode) {
                    this.startAutoMode();
                    console.log('自动模式开启')
                  } else {
                    if (this.autoModeInterval) {
                      clearInterval(this.autoModeInterval);
                    }
                  }
                });

              Text('自动')
                .fontColor(Color.Black)
                .fontSize(40);
            }
            .width('100%')
            .justifyContent(FlexAlign.Center)
            .alignItems(VerticalAlign.Center);
          }
          .width('100%')
          .padding(20) // 增加内边距
          .backgroundColor(Color.Orange) // 模式选择背景颜色
          .border({ width: '10px', color: '#FFFFFF' })
          .shadow({ radius: 10, color: Color.Gray })
          .justifyContent(FlexAlign.Center)
          .alignItems(HorizontalAlign.Center);

          // 开关控制列（仅在手动模式下显示）
          if (!this.isAutoMode) {
            Column({ space: 10 }) {
              Text('开关：')
                .fontSize(60)
                .fontColor(Color.Black)
                .textShadow({ radius: 10, color: Color.White, offsetX: 0, offsetY: 0 });

              Row({ space: 10 }) {
                Text('OFF')
                  .fontColor(Color.Black)
                  .fontSize(40);

                // 使用 isOn 属性来绑定状态
                Toggle({ type: ToggleType.Switch, isOn: this.isOn })
                  .margin({ top: 6, right: 14, bottom: 6, left: 14 })
                  .selectedColor(Color.Blue)
                  .onChange((isOn: boolean) => {
                    console.log('开关状态改变: ' + isOn);
                    this.isOn = isOn;
                    this.setLedStatus(isOn);
                    this.updateImagePath(); // 更新图片路径
                    AppStorage.setOrCreate('isON', isOn);
                    console.log('存储 ' + (isOn ? '真' : '假') + '到缓存');
                  });

                Text('ON')
                  .fontColor(Color.Black)
                  .fontSize(40);
              }
              .width('100%')
              .justifyContent(FlexAlign.Center)
              .alignItems(VerticalAlign.Center);
            }
            .width('100%')
            .padding(20) // 增加内边距
            .backgroundColor(Color.Orange) // 开关控制背景颜色
            .border({ width: '10px', color: '#FFFFFF' })
            .shadow({ radius: 10, color: Color.Gray })
            .justifyContent(FlexAlign.Center)
            .alignItems(HorizontalAlign.Center);
          }
        }
        .width('40%')
        .height('100%')
        .justifyContent(FlexAlign.Center)
        .alignItems(HorizontalAlign.Center)
        .backgroundColor(Color.White); // 右侧控制容器背景颜色
      }
      .width('100%')
      .height('80%')
      .justifyContent(FlexAlign.Center)
      .alignItems(VerticalAlign.Center);

    }
    .width('100%')
    .height('100%')
    .backgroundColor(Color.White)
    .padding(20);
  }

  // 设置LED状态的函数
  private setLedStatus(on: boolean) {
    let ledStatus = on ? gpioled.ON : gpioled.OFF;
    gpioled.setLedStatusWithPromise(this.ledPort, ledStatus).then((result: number) => {
      if (result === 0) {
        console.info(`LED turned ${on ? 'on' : 'off'}.`);
      } else {
        console.error(`Failed to turn ${on ? 'on' : 'off'} LED. Error code: ${result}`);
      }
    });
  }

  // 更新图片路径的函数
  private updateImagePath() {
    this.imagePath = this.isOn ? this.onImage : this.offImage;
    console.info(`Image path updated to ${this.isOn ? 'onImage' : 'offImage'}`);
  }

  // 获取 ADC 值并打印日志的函数
  private getAdcValueAndLog(): number | null {
    try {
      const adcValue: number = adc_napi.sync_get_photoresistor_value(); // 调用光敏电阻的接口函数
      console.info(`Photoresistor ADC value: ${adcValue}`);
      return adcValue;
    } catch(err) {
      console.error(`Error while getting ADC value: ${err}`);
      return null;
    }
  }

  // 启动自动模式的函数
  private startAutoMode() {
    this.autoModeInterval = setInterval(() => {
      if (this.isAutoMode) {
        const adcValue = this.getAdcValueAndLog();
        if (adcValue !== null && adcValue > this.adcThreshold) { // 大于阈值时开灯
          if (!this.isOn) {
            this.isOn = true;
            this.setLedStatus(true);
            this.updateImagePath(); // 更新图片路径
            AppStorage.setOrCreate('isON', true);
            console.info('Light turned on in auto mode.');
          }
        } else {
          if (this.isOn) {
            this.isOn = false;
            this.setLedStatus(false);
            this.updateImagePath(); // 更新图片路径
            AppStorage.setOrCreate('isON', false);
            console.info('Light turned off in auto mode.');
          }
        }
      }
    }, 10000); // 每10秒检查一次
  }
}