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
import pwmtest from '@ohos.pwmtest';

@Entry
@Component
struct CurtainView {
  @State isOpen: boolean = false;
  @State imagePath: Resource = $r('app.media.curtain_close'); // 定义类型为 Resource
  private onImage: Resource = $r('app.media.curtain_open');
  private offImage: Resource = $r('app.media.curtain_close');

  aboutToAppear() {
    const storedState = AppStorage.get<boolean>('Curtain_isOpen');
    console.log('从 AppStorage 获取的窗帘状态: ' + storedState);
    this.isOpen = storedState ?? false;
    this.updateImagePath(); // 更新图片路径
    console.log('获取上次关闭时的窗帘状态: ' + this.isOpen);
  }

  aboutToDisappear() {
    AppStorage.setOrCreate('Curtain_isOpen', this.isOpen);
    console.log('存储窗帘状态: ' + this.isOpen);
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
      .alignItems(VerticalAlign.Center)
      .backgroundColor('#e9f8f5'); // 返回按钮行背景颜色

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
        .width('40%') // 左侧容器占40%宽度
        .height('100%')
        .justifyContent(FlexAlign.Center)
        .alignItems(HorizontalAlign.Center)
        .backgroundColor(Color.White); // 左侧图片容器背景颜色

        // 右侧功能容器
        Column({ space: 20 }) {
          // 窗帘控制列
          Column({ space: 10 }) {
            Text('窗帘控制：')
              .fontSize(60)
              .fontColor(Color.Black)
              .textShadow({ radius: 10, color: Color.White, offsetX: 0, offsetY: 0 });

            Row({ space: 20 }) {
              Button('拉开', { type: ButtonType.Normal, stateEffect: true })
                .borderRadius(20)
                .backgroundColor('#e9f8f5')
                .width('40%')
                .height(50)
                .fontSize(30)
                .fontColor(Color.Black)
                .onClick(() => {
                  pwmtest.setPwmStatus(1, 179);
                  this.isOpen = true;
                  this.updateImagePath(); // 更新背景图片
                });

              Button('收起', { type: ButtonType.Normal, stateEffect: true })
                .borderRadius(20)
                .backgroundColor('#e9f8f5')
                .width('40%')
                .height(50)
                .fontSize(30)
                .fontColor(Color.Black)
                .onClick(() => {
                  pwmtest.setPwmStatus(1, 0);
                  this.isOpen = false;
                  this.updateImagePath(); // 更新背景图片
                });
            }
            .width('100%')
            .height('80%')
            .justifyContent(FlexAlign.Center)
            .alignItems(VerticalAlign.Center);
          }
          .width('100%')
          .padding(20) // 增加内边距
          .backgroundColor('#F5DEB3') // 模式选择背景颜色
          .border({ width: '10px', color: '#FFFFFF' })
          .shadow({ radius: 10, color: Color.Gray })
          .justifyContent(FlexAlign.Center)
          .alignItems(HorizontalAlign.Center);
        }
        .width('60%') // 右侧容器占60%宽度
        .height('80%')
        .backgroundColor('#e9f8f5'); // 右侧功能容器背景颜色
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

  // 更新图片路径的函数
  private updateImagePath() {
    this.imagePath = this.isOpen ? this.onImage : this.offImage;
    console.info(`Image path updated to ${this.isOpen ? 'open' : 'close'}`);
  }
}