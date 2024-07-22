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

import { Icon } from './Icon'

interface FooOptions {
  icon: Resource,
  title: string,
}

export interface ColorOption {
  context: ResourceStr
}


@Component
export struct Foo {
  @State fooList: FooOptions[] = [
    {
      icon: $r('app.media.home_icon'),
      title: '首页'
    },
    {
      icon: $r('app.media.me_icon'),
      title: '我的'
    }]
  @State color: ColorOption = {
    context: $r('app.color.icon_fuzhu_color')
  }
  @State activeColor: ColorOption = {
    context: $r('app.color.app_header_color')
  }
  @State fooWight: string = String(100 / this.fooList.length) + '%'
  @State activeNum: number = 0

  build() {
    Flex({
      direction: FlexDirection.Row,
      alignItems: ItemAlign.Center
    }) {

      ForEach(this.fooList, (item: FooOptions, index: number) => {
        Row() {
          Row() {
            Row() {
              Image(item.icon)
                .objectFit(ImageFit.Contain)
                .fillColor((this.activeNum == index ? this.activeColor.context : this.color.context))
                .width('100%')
                .height('100%')
            }
            .width('100%')
            .height('100%')
          }
          .width(40)
          .height(40)
          .margin({ right: 15 })

          Text(item.title)
            .fontColor((this.activeNum == index ? this.activeColor.context : this.color.context))
            .fontSize(20)
        }
        .width(this.fooWight)
        .height('100%')
        .backgroundColor((this.activeNum == index ? '#efefef' : '#fff'))
        .justifyContent(FlexAlign.Center)
        .onClick((e: ClickEvent) => {
          this.activeNum = index
        })
      }, (item: FooOptions, index: number) => index.toString())
    }
    .width('100%')
    .height('100%')
    .backgroundColor(Color.White)
  }
}