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
import { Icon } from './Icon'

@Component
export struct Header {
  @Link username: string;

  build() {
    Flex({
      direction: FlexDirection.Row,
      justifyContent: FlexAlign.SpaceBetween,
      alignItems: ItemAlign.Center
    }) {
      Row() {
        Icon({
          iconUrl: $r('app.media.return_icon')
        })
          .width(40)
          .height(40)
          .margin({ right: 10 })
          .onClick((e: ClickEvent) => {
            AppStorage.Delete('user')
            router.pushUrl({
              url: 'pages/Login'
            })
          })

        Text(this.username + ' 欢迎您')
          .fontColor(Color.White)
          .fontSize(20)
      }
      .width(130)
      .height(40)
      .margin({ left: 15 })


      Row() {
        Text($r('app.string.application_name'))
          .fontSize(25)
          .fontColor('#fff')
          .fontWeight(600)
      }

      Row() {
        Icon({
          iconUrl: $r('app.media.more_icon')
        })
      }
      .width(40)
      .height(40)
      .margin({ right: 15 })
    }
    .backgroundColor($r('app.color.app_header_color'))
    .width('100%')
    .height('100%')
  }
}