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

@Entry
@Component
struct Index {
  build() {
    Column({ space: 20 }) {
      Row({ space: 100 }) {
        Column({ space: 5 }) {
          Image($r('app.media.light'))
            .width('200px')  // 调整图片宽度
            .height('200px') // 调整图片高度
            .borderRadius(20)
            .interpolation(ImageInterpolation.High)
            .padding(10)
            .border({ width: '2px', color: '#FFFFFF' })
            .shadow({ radius: 10, color: Color.Gray })
            .onClick(() => {
              router.pushUrl({ url: 'pages/light' })
                .then(() => {})
                .catch((err: BusinessError) => {
                  console.error(`Failed to jump to the light page. Code is ${err.code}, message is ${err.message}`);
                });
            });
          Text('灯光控制')
            .fontSize(16)
            .fontColor('#000000')
            .margin({ top: 5 });
        }

        Column({ space: 5 }) {
          Image($r('app.media.fan'))
            .width('200px')  // 调整图片宽度
            .height('200px') // 调整图片高度
            .borderRadius(20)
            .interpolation(ImageInterpolation.High)
            .padding(10)
            .border({ width: '2px', color: '#FFFFFF' })
            .shadow({ radius: 10, color: Color.Gray })
            .onClick(() => {
              router.pushUrl({ url: 'pages/fan' })
                .then(() => {})
                .catch((err: BusinessError) => {
                  console.error(`Failed to jump to the fan page. Code is ${err.code}, message is ${err.message}`);
                });
            });
          Text('风力控制')
            .fontSize(16)
            .fontColor('#000000')
            .margin({ top: 5 });
        }
      }
      .justifyContent(FlexAlign.Center);

      Row({ space: 100 }) {
        Column({ space: 5 }) {
          Image($r('app.media.curtains'))
            .width('200px')  // 调整图片宽度
            .height('200px') // 调整图片高度
            .borderRadius(20)
            .interpolation(ImageInterpolation.High)
            .padding(10)
            .border({ width: '2px', color: '#FFFFFF' })
            .shadow({ radius: 10, color: Color.Gray })
            .onClick(() => {
              router.pushUrl({ url: 'pages/curtain' })
                .then(() => {})
                .catch((err: BusinessError) => {
                  console.error(`Failed to jump to the curtain page. Code is ${err.code}, message is ${err.message}`);
                });
            });
          Text('窗帘控制')
            .fontSize(16)
            .fontColor('#000000')
            .margin({ top: 5 });
        }

        Column({ space: 5 }) {
          Image($r('app.media.alarm'))
            .width('200px')  // 调整图片宽度
            .height('200px') // 调整图片高度
            .borderRadius(20)
            .interpolation(ImageInterpolation.High)
            .padding(10)
            .border({ width: '2px', color: '#FFFFFF' })
            .shadow({ radius: 10, color: Color.Gray })
            .onClick(() => {
              router.pushUrl({ url: 'pages/alarm' })
                .then(() => {})
                .catch((err: BusinessError) => {
                  console.error(`Failed to jump to the alarm page. Code is ${err.code}, message is ${err.message}`);
                });
            });
          Text('闹钟')
            .fontSize(16)
            .fontColor('#000000')
            .margin({ top: 5 });
        }
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#FFDDAA')
    .justifyContent(FlexAlign.SpaceAround)
    .padding(20);
  }
}