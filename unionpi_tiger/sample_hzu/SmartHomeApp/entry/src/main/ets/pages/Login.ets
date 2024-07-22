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

@Entry
@Component
struct Login {
  @State username: string = 'admin'
  @State password: string = 'admin'

  build() {

    Column() {
      Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.Center, alignItems: ItemAlign.Center }) {
        Row() {
          Text('智能家居系统')
            .fontColor('#555')
            .fontSize(40)
        }
        .padding({
          top: 10,
          bottom: 10
        })
        .width('100%')
        .justifyContent(FlexAlign.Center)

        Row() {
          TextInput({
            placeholder: '用户名'
          })
            .type(InputType.Normal)
            .borderRadius(15)
            .onChange((value: string) => {
              this.username = value;
            })
        }
        .width('60%')
        .margin({
          top: 20,
          bottom: 20
        })
        .justifyContent(FlexAlign.Center)

        Row() {
          TextInput({
            placeholder: '密码'
          })
            .type(InputType.Normal)
            .borderRadius(15)
            .onChange((value: string) => {
              this.password = value
            })
        }
        .width('60%')
        .margin({
          bottom: 25
        })
        .justifyContent(FlexAlign.Center)

        Row() {
          Button('登录')
            .width('100%')
            .onClick((e: ClickEvent) => {
              console.log(this.username + ' ' + this.password)

              if (this.username == '' || this.password == '') {
                return
              }

              if(this.username == 'admin' && this.password == 'admin') {
                AppStorage.SetOrCreate('user', this.username)
                router.pushUrl({
                  url: 'pages/Index'
                })
              }
            })
        }
        .width('50%')
        .margin({
          bottom: 20
        })
        .justifyContent(FlexAlign.Center)

      }
      .width('50%')
      .height('65%')
      .backgroundColor('#ffffff')
      .zIndex(10)
    }
    .shadow({
      radius: 5,
      color: '#e1e1e1',
      offsetX: 2,
      offsetY: 2
    })
    .borderRadius(10)
    .width('100%')
    .height('100%')
    .alignItems(HorizontalAlign.Center)
    .justifyContent(FlexAlign.Center)
    .backgroundImage($r('app.media.login_bg'))
    .backgroundImageSize({
      width: '100%',
      height: '100%'
    })
  }
}