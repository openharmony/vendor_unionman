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
import pwmtest from '@ohos.pwmtest';

@Entry
@Component
struct Fan {
  private fanPortAIN1 = 386;
  private fanPortAIN2 = 387;
  private fanPortSTBY = 388;
  private adcThreshold = 600; // 设定的温度传感器 ADC 值阈值
  private autoModeInterval: number | undefined;
  private temperatureUpdateInterval: number | undefined; // 新增温度更新定时器
  private temperatureCoefficient = 0.042; // 温度系数
  private onImage: Resource = $r('app.media.fan_open');
  private offImage: Resource = $r('app.media.fan_close');

  @State private isOn: boolean = false;
  @State private isAutoMode: boolean = false;
  @State private temperature: number = 0; // 温度状态
  @State private imagePath: Resource = $r('app.media.fan_close'); // 中间变量存储图片路径

  aboutToAppear() {
    const storedState = AppStorage.get<boolean>('Fan_isON');
    const storedAutoMode = AppStorage.get<boolean>('isAutoMode');
    this.isOn = storedState ?? false;
    this.isAutoMode = storedAutoMode ?? false;
    this.setFanStatus(this.isOn);

    // 获取 ADC 值并打印日志
    this.getAdcValueAndLog();

    // 如果自动模式开启，启动定时器
    if (this.isAutoMode) {
      this.startAutoMode();
    } else {
      this.startTemperatureUpdate(); // 启动温度更新定时器
    }
  }

  aboutToDisappear() {
    if (this.autoModeInterval) {
      clearInterval(this.autoModeInterval);
    }
    if (this.temperatureUpdateInterval) {
      clearInterval(this.temperatureUpdateInterval);
    }
  }

  build() {
    Column({ space: 20 }) {
      // 返回按钮行
      Row({ space: 10 }) {
        Button('<', { type: ButtonType.Circle, stateEffect: true })
          .width(50)
          .height(50)
          .fontSize(30)
          .fontColor(Color.Black)
          .backgroundColor('#40E0D0')
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
      .alignItems(VerticalAlign.Center);

      Row({ space: 20 }) {
        // 左侧容器，包含温度和图片
        Column({ space: 10 }) {
          // 显示温度行
          Row({ space: 10 }) {
            Text('当前温度：').fontSize(20);
            Text(`${this.temperature.toFixed(2)} °C`).fontSize(20);
          }
          .backgroundColor('#40E0D0')
          .width('100%')
          .height('10%')
          .justifyContent(FlexAlign.Center)
          .alignItems(VerticalAlign.Center);

          // 图片容器
          Column({ space: 10 }) {
            Image(this.imagePath)
              .width('100%')
              .height('100%')
              .borderRadius(20)
              .interpolation(ImageInterpolation.High)
              .padding(10)
              .border({ width: '2px', color: '#FFFFFF' })
              .shadow({ radius: 10, color: Color.Gray });
          }
          .width('90%')
          .height('90%')
          .justifyContent(FlexAlign.Center)
          .alignItems(HorizontalAlign.Center)
          .backgroundColor(Color.White);
        }
        .width('40%')
        .height('100%')
        .backgroundColor('#40E0D0');

        Column({ space: 0 }) {
          // 模式选择列
          Column({ space: 10 }) {
            Text('模式选择：')
              .fontSize(30)
              .fontColor(Color.Black)
              .textShadow({ radius: 10, color: Color.White, offsetX: 0, offsetY: 0 });

            Row({ space: 10 }) {
              Text('手动')
                .fontColor(Color.Black)
                .fontSize(20);

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
                    console.log('自动模式开启');
                  } else {
                    if (this.autoModeInterval) {
                      clearInterval(this.autoModeInterval);
                    }
                    this.startTemperatureUpdate(); // 启动温度更新定时器
                  }
                });

              Text('自动')
                .fontColor(Color.Black)
                .fontSize(20);
            }
            .width('100%')
            .justifyContent(FlexAlign.Center)
            .alignItems(VerticalAlign.Center);
          }
          .width('100%')
          .padding(20) // 增加内边距
          .backgroundColor('#87CEEB') // 模式选择背景颜色
          .border({ width: '10px', color: '#FFFFFF' })
          .shadow({ radius: 10, color: Color.Gray })
          .justifyContent(FlexAlign.Center)
          .alignItems(HorizontalAlign.Center);

          // 开关控制列（仅在手动模式下显示）
          if (!this.isAutoMode) {
            Column({ space: 10 }) {
              Text('开关：')
                .fontSize(30)
                .fontColor(Color.Black)
                .textShadow({ radius: 10, color: Color.White, offsetX: 0, offsetY: 0 });

              Row({ space: 10 }) {
                Text('OFF')
                  .fontColor(Color.Black)
                  .fontSize(20);

                // 使用 isOn 属性来绑定状态
                Toggle({ type: ToggleType.Switch, isOn: this.isOn })
                  .margin({ top: 6, right: 14, bottom: 6, left: 14 })
                  .selectedColor(Color.Blue)
                  .onChange((isOn: boolean) => {
                    this.isOn = isOn;
                    this.setFanStatus(isOn);
                    pwmtest.setPwmStatus(2, 45);
                    this.updateImagePath(); // 更新图片路径
                    AppStorage.setOrCreate('Fan_isON', isOn);
                    console.log('存储 ' + (isOn ? '真' : '假') + ' 到缓存');
                  });

                Text('ON')
                  .fontColor(Color.Black)
                  .fontSize(20);
              }
              .width('100%')
              .justifyContent(FlexAlign.Center)
              .alignItems(VerticalAlign.Center);
            }
            .width('100%')
            .padding(20) // 增加内边距
            .backgroundColor('#87CEEB') // 开关控制背景颜色
            .border({ width: '10px', color: '#FFFFFF' })
            .shadow({ radius: 10, color: Color.Gray })
            .justifyContent(FlexAlign.Center)
            .alignItems(HorizontalAlign.Center);
          }

          // 风力调节行
          Column() {
            Row({ space: 10 }) {
              Text('风力：').fontSize(30);
              Button('+', { type: ButtonType.Capsule, stateEffect: true })
                .width(50)
                .height(50)
                .fontSize(50)
                .fontColor(Color.Black)
                .backgroundColor('#87CEEB')
                .onClick(() => {
                  pwmtest.setPwmStatus(2, 90); // 加速
                });

              Button('-', { type: ButtonType.Capsule, stateEffect: true })
                .width(50)
                .height(50)
                .fontSize(50)
                .fontColor(Color.Black)
                .backgroundColor('#87CEEB')
                .onClick(() => {
                  pwmtest.setPwmStatus(2, 20); // 减速
                });
            }
          }
          .width('100%')
          .padding(20) // 增加内边距
          .backgroundColor('#87CEEB') // 风力调节背景颜色
          .border({ width: '10px', color: '#FFFFFF' })
          .shadow({ radius: 10, color: Color.Gray })
          .justifyContent(FlexAlign.Center)
          .alignItems(HorizontalAlign.Center);
        }
        .width('60%') // 右侧容器占60%宽度
        .height('100%')
        .backgroundColor('#40E0D0') // 右侧容器背景颜色
      }
      .width('100%')
      .height('80%')
      .justifyContent(FlexAlign.Center)
      .alignItems(VerticalAlign.Center);

    }
    .width('100%')
    .height('100%')
    .backgroundColor('#40E0D0')
    .padding(20);
  }

  // 设置风扇状态的函数
  private setFanStatus(on: boolean) {
    let fanStatus = on ? gpioled.ON : gpioled.OFF;
    gpioled.setFanStatusWithPromise(this.fanPortSTBY, fanStatus).then((result: number) => {
      if (result === 0) {
        console.info('Turned ' + (on ? 'on' : 'off') + ' fan.');
      } else {
        console.error('Failed to turn ' + (on ? 'on' : 'off') + ' fan. Error code: ' + result);
      }
    });
    // 控制风扇方向
    gpioled.setFanStatusWithPromise(this.fanPortAIN1, on ? gpioled.ON : gpioled.OFF);
    gpioled.setFanStatusWithPromise(this.fanPortAIN2, on ? gpioled.OFF : gpioled.ON);
  }

  // 更新图片路径的函数
  private updateImagePath() {
    this.imagePath = this.isOn ? this.onImage : this.offImage;
    console.info(`Image path updated to ${this.isOn ? 'onImage' : 'offImage'}`);
  }

  // 获取温度传感器的ADC值并打印日志的函数
  private getAdcValueAndLog(): number | null {
    try {
      const adcValue: number = adc_napi.sync_get_temp_sensor_value(); // 调用温度传感器的接口函数
      console.log('获取到的温度传感器 ADC 值: ' + adcValue); // 日志内容
      this.temperature = adcValue * this.temperatureCoefficient; // 计算温度
      return adcValue;
    } catch (err) {
      console.error('Error while getting ADC value: ' + err);
      return null;
    }
  }

  // 启动自动模式的函数
  private startAutoMode() {
    this.autoModeInterval = setInterval(() => {
      if (this.isAutoMode) {
        const adcValue = this.getAdcValueAndLog();
        if (adcValue !== null && adcValue > this.adcThreshold) { // 大于阈值时开风扇
          if (!this.isOn) {
            this.isOn = true;
            this.setFanStatus(true);
            AppStorage.setOrCreate('Fan_isON', true);
            console.log('自动模式下开风扇');
          }
        } else {
          if (this.isOn) {
            this.isOn = false;
            this.setFanStatus(false);
            AppStorage.setOrCreate('Fan_isON', false);
            console.log('自动模式下关风扇');
          }
        }
      }
    }, 10000);
  }

  // 启动温度更新定时器的函数
  private startTemperatureUpdate() {
    this.temperatureUpdateInterval = setInterval(() => {
      this.getAdcValueAndLog();
    }, 5000); // 每5秒更新一次温度
  }
}