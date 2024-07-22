/*
 * Copyright (c) 2022 Unionman Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import router from '@ohos.router';
import prompt from '@ohos.prompt';
import gpioled from '@ohos.gpioled';
@Entry
@Component
struct fmq {
  @State pin: number = 386;
  intervalID: number = 0;
  I :number = 0;
  getValue() {
    if (this.I) {
      gpioled.setLedStatus(this.pin, gpioled.LED_OFF);
      this.I--;
    } else {
      gpioled.setLedStatus(this.pin, gpioled.LED_ON);
      this.I++;
    }
  }
  startWork() {
    this.intervalID = setInterval(() => {
      this.getValue();
    }, 1000);
  }
  build() {
    Stack() {
      // 背景图片
      Image($r('app.media.bj1'));

      Row() {
        Text('蜂鸣器')
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .margin({top: -410});
      }
      // 主体页面
      Row() {
        // 第一个按钮
        Button() {
          Image($r('app.media.11'));
        }
        .width(300)
        .height(300)
        .margin({top: 80})
        .backgroundColor('rgba(0, 0, 0, 0)')
        .onClick(() => {
          gpioled.setLedStatus(this.pin, gpioled.LED_OFF);
        });
        Blank();
        // 第二个按钮
        Button() {
          Image($r('app.media.12'));
        }
        .width(300)
        .height(300)
        .margin({top: 80})
        .backgroundColor('rgba(0, 0, 0, 0)')
        .onClick(() => {
          clearTimeout(this.intervalID);
          gpioled.setLedStatus(this.pin, gpioled.LED_ON);
        });

        Blank();

        // 第三个按钮
        Button() {
          Image($r('app.media.13'));
        }
        .width(300)
        .height(300)
        .margin({top: 80})
        .backgroundColor('rgba(0, 0, 0, 0)')
        .onClick(() => {
          this.startWork();
        });
      }
      .width('90%')
      .height('100%');
    }
    .width('100%')
    .height('100%');
  }
}