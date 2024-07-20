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
import promptAction from '@ohos.promptAction';

@Entry
@Component
struct Alarm {
  private alarmPort = 384;
  @State hours: string = ''; // 初始小时为空字符串
  @State minutes: string = ''; // 初始分钟为空字符串
  @State remainingTime: number = 0; // 剩余时间，单位为秒
  @State alarmText: string = ''; // 闹钟文本
  @State inputText: string = ''; // 用户输入的文本
  private timerId: number | null = null;
  private intervalId: number | null = null; // 用于倒计时的定时器
  private ringingTimerId: number | null = null; // 用于响铃时间的定时器

  aboutToAppear() {
    // 从 AppStorage 中获取闹钟状态
    const storedRemainingTime = AppStorage.get<number>('remainingTime');
    const storedAlarmText = AppStorage.get<string>('alarmText');
    console.log('从 AppStorage 获取的值: ' + storedRemainingTime);
    this.remainingTime = storedRemainingTime ?? 0;
    this.alarmText = storedAlarmText ?? '';

    if (this.remainingTime > 0) {
      this.startCountdownInterval();
    }
  }

  build() {
    Column({ space: 20 }) {
      // 返回按钮行
      Row({ space: 10 }) {
        Button('<', { type: ButtonType.Circle, stateEffect: true })
          .width(50)
          .height(50)
          .fontColor(Color.Black)
          .fontSize(30)
          .backgroundColor('#e9f8f5')
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
        // 左侧图片和倒计时容器
        Column({ space: 10 }) {
          // 图片容器
          Column({ space: 10 }) {
            Image($r('app.media.naozhong')) // 使用中间变量存储的图片路径
              .width('100%')  // 调整图片宽度
              .height('100%') // 调整图片高度
              .borderRadius(20)
              .interpolation(ImageInterpolation.High)
              .padding(10)
              .border({ width: '2px', color: '#FFFFFF' })
              .shadow({ radius: 10, color: Color.Gray });
          }
          .width('50%')
          .height('50%')
          .justifyContent(FlexAlign.Center)
          .alignItems(HorizontalAlign.Center)
          .backgroundColor(Color.White);

          // 倒计时显示
          Row({ space: 5 }) {
            Text('倒计时：').fontSize(30);
            Text(`${Math.floor(this.remainingTime / 3600)} 小时 ${Math.floor((this.remainingTime % 3600) / 60)} 分钟 ${this.remainingTime % 60} 秒`).fontSize(30);
          }
          .width('100%')
          .height('10%')
          .justifyContent(FlexAlign.Start)
          .alignItems(VerticalAlign.Top);

          // 备注显示
          Row({ space: 10 }) {
            Text('备注：').fontSize(30);
            Text(this.alarmText).fontSize(30);
          }
          .width('100%')
          .height('40%')
          .justifyContent(FlexAlign.Start)
          .alignItems(VerticalAlign.Top);
        }
        .width('50%') // 左侧容器占40%宽度
        .height('100%')
        .backgroundColor(Color.White); // 左侧容器背景颜色

        // 右侧功能容器
        Column({ space: 5 }) {
          // 闹钟时长输入
          Column({ space: 10 }) {
            Text('闹钟时长：')
              .fontSize(20)
              .fontColor(Color.Black)
              .textShadow({ radius: 10, color: Color.White, offsetX: 0, offsetY: 0 });

            Row({ space: 5 }) {
              // 小时输入框
              TextInput({
                text: this.hours,
              })
                .width(100)
                .height(60)
                .fontSize(20)
                .backgroundColor(Color.White)
                .onChange((value) => {
                  this.hours = value;
                });

              Text('小时').fontSize(20);

              // 分钟输入框
              TextInput({
                text: this.minutes,
              })
                .width(100)
                .height(60)
                .fontSize(20)
                .backgroundColor(Color.White)
                .onChange((value) => {
                  this.minutes = value;
                });

              Text('分钟').fontSize(20);
            }
            .width('100%')
            .justifyContent(FlexAlign.Center)
            .alignItems(VerticalAlign.Center);
          }
          .width('100%')
          .padding(10) // 增加内边距
          .backgroundColor('#E6E6FA') // 模式选择背景颜色
          .border({ width: '10px', color: '#FFFFFF' })
          .shadow({ radius: 10, color: Color.Gray })
          .justifyContent(FlexAlign.Center)
          .alignItems(HorizontalAlign.Center);

          // 闹钟文本输入框
          Column({ space: 10 }) {
            Text('闹钟备注：')
              .fontSize(20)
              .fontColor(Color.Black)
              .textShadow({ radius: 10, color: Color.White, offsetX: 0, offsetY: 0 });

            TextInput({
              text: this.inputText,
            })
              .width(300)
              .height(60)
              .fontSize(20)
              .backgroundColor(Color.White)
              .onChange((value) => {
                this.inputText = value;
              });
          }
          .width('100%')
          .padding(10) // 增加内边距
          .backgroundColor('#E6E6FA') // 模式选择背景颜色
          .border({ width: '10px', color: '#FFFFFF' })
          .shadow({ radius: 10, color: Color.Gray })
          .justifyContent(FlexAlign.Center)
          .alignItems(HorizontalAlign.Center);

          // 开始和结束按钮
          Row({ space: 10 }) {
            Button('开始', { type: ButtonType.Capsule, stateEffect: true })
              .width(200)
              .height(60)
              .fontSize(20)
              .backgroundColor('#e9f8f5')
              .fontColor(Color.Black)
              .onClick(() => {
                this.startAlarm();
              });

            Button('结束', { type: ButtonType.Capsule, stateEffect: true })
              .width(200)
              .height(60)
              .fontSize(20)
              .backgroundColor('#e9f8f5')
              .fontColor(Color.Black)
              .onClick(() => {
                this.stopAlarm();
              });
          }
          .width('100%')
          .padding(10) // 增加内边距
          .backgroundColor('#E6E6FA') // 模式选择背景颜色
          .border({ width: '10px', color: '#FFFFFF' })
          .shadow({ radius: 10, color: Color.Gray })
          .alignItems(VerticalAlign.Center)
          .justifyContent(FlexAlign.Center);
        }
        .width('50%') // 右侧容器占60%宽度
        .height('100%')
        .backgroundColor('#E6E6FA'); // 右侧功能容器背景颜色
      }
      .width('100%')
      .height('80%')
      .justifyContent(FlexAlign.Center)
      .alignItems(VerticalAlign.Center);
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#e9f8f5')
    .padding(20);
  }

  // 解析字符串为整数
  customParseInt(value: string): number | null {
    let result = 0;
    let sign = 1;
    let i = 0;

    if (value.length === 0) {
      return null;
    }

    // 检查是否为负数
    if (value[0] === '-') {
      sign = -1;
      i = 1;
    }

    for (; i < value.length; i++) {
      const charCode = value.charCodeAt(i);
      if (charCode < 48 || charCode > 57) { // '0' = 48, '9' = 57
        return null;
      }
      result = result * 10 + (charCode - 48);
    }
    return result * sign;
  }

  // 开始闹钟
  startAlarm() {
    const totalSeconds = (this.customParseInt(this.hours) || 0) * 3600 + (this.customParseInt(this.minutes) || 0) * 60;
    if (totalSeconds > 0) {
      this.remainingTime = totalSeconds;
      this.alarmText = this.inputText; // 设置闹钟文本
      this.setAlarmStatus(false); // 闹钟倒计时开始
      this.hours = ''; // 清除小时输入框内容
      this.minutes = ''; // 清除分钟输入框内容
      this.inputText = ''; // 清除用户输入的文本

      // 保存状态到AppStorage
      AppStorage.setOrCreate('remainingTime', this.remainingTime);
      AppStorage.setOrCreate('alarmText', this.alarmText);

      this.timerId = setTimeout(() => {
        console.log('闹钟时间到！');
        this.startRinging(); // 闹钟时间到时开始响铃
        this.clearCountdownInterval();
        this.remainingTime = 0; // 重置倒计时
        // 清除状态
        AppStorage.setOrCreate('remainingTime', 0);
      }, totalSeconds * 1000);

      this.startCountdownInterval();

      console.log(`闹钟已设置为 ${this.hours} 小时 ${this.minutes} 分钟`);
      try {
        promptAction.showToast({
          message: '闹钟已设置',
          duration: 2000
        });
      } catch (error) {
        let message = (error as BusinessError).message;
        let code = (error as BusinessError).code;
        console.error(`showToast args error code is ${code}, message is ${message}`);
      }
    } else {
      console.error('无效的闹钟时长');
      promptAction.showToast({
        message: '无效的闹钟时长',
        duration: 2000
      });
    }
  }

  // 停止闹钟
  stopAlarm() {
    if (this.timerId !== null) {
      clearTimeout(this.timerId);
      this.timerId = null;
    }
    if (this.ringingTimerId !== null) {
      clearTimeout(this.ringingTimerId);
      this.ringingTimerId = null;
    }
    this.setAlarmStatus(false); // 停止响铃
    this.clearCountdownInterval();
    this.remainingTime = 0; // 重置倒计时
    this.hours = ''; // 清除小时输入框内容
    this.minutes = ''; // 清除分钟输入框内容
    this.alarmText = ''; // 清除闹钟文本
    this.inputText = ''; // 清除用户输入的文本
    // 清除状态
    AppStorage.setOrCreate('remainingTime', 0);
    AppStorage.setOrCreate('alarmText', '');
    console.log('闹钟已关闭');
    promptAction.showToast({
      message: '闹钟已关闭',
      duration: 2000
    });
  }

  // 启动倒计时
  startCountdownInterval() {
    if (this.intervalId !== null) {
      clearInterval(this.intervalId);
    }

    this.intervalId = setInterval(() => {
      if (this.remainingTime > 0) {
        this.remainingTime -= 1;
        // 更新状态到AppStorage
        AppStorage.setOrCreate('remainingTime', this.remainingTime);
      } else {
        this.clearCountdownInterval();
        this.setAlarmStatus(false); // 倒计时结束时关闭LED
        // 清除状态
        AppStorage.setOrCreate('remainingTime', 0);
        AppStorage.setOrCreate('alarmText', '');
      }
    }, 1000);
  }

  // 清除倒计时
  clearCountdownInterval() {
    if (this.intervalId !== null) {
      clearInterval(this.intervalId);
      this.intervalId = null;
    }
  }

  // 设置闹钟状态的函数
  private setAlarmStatus(on: boolean) {
    let alarmStatus = on ? gpioled.ON : gpioled.OFF;
    gpioled.setAlarmStatusWithPromise(this.alarmPort, alarmStatus).then((result: number) => {
      if (result === 0) {
        console.info('Turned ' + (on ? 'on' : 'off') + ' Alarm. ');
      } else {
        console.info('Failed to turn ' + (on ? 'on' : 'off') + ' alarm. Error code: ' + result);
      }
    });
  }

  // 开始响铃
  private startRinging() {
    this.setAlarmStatus(true);
    this.ringingTimerId = setTimeout(() => {
      this.setAlarmStatus(false);
      this.ringingTimerId = null;
      console.log('闹钟响铃已自动停止');
    }, 5 * 60 * 1000); // 5分钟后自动停止响铃
  }
}