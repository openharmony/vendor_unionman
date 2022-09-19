/*
 * Copyright 2022 Unionman Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import router from '@ohos.router';
import { note, NoteArray } from '../model/noteModel'
import { getDate, getTime } from '../utils/functionUtil'
import um_spi from '@ohos.um_a311d';

/**
 * 新建备忘录笔记编辑界面
 */
@Entry
@Component
struct noteNewPage {
  scroller: Scroller = new Scroller()
  @State date: number = router.getParams()['date']
  @State time: number = router.getParams()['time']
  title: string = ''
  @State noteValue: string = ''
  @State count: number = this.noteValue.length

  build() {
    Column({ space: 10 }) {
      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center }) {
        Image($r('app.media.ic_public_back'))
          .width(24)
          .height(24)
          .onClick(() => {
            router.replace({
              url: 'pages/SpiDevice',
            })
          })
        Blank()
        if (this.noteValue == '') {
          Image($r('app.media.ic_public_ok_unabled'))
            .width(24)
            .height(24)
        } else {
          Image($r('app.media.ic_public_ok_filled'))
            .width(24)
            .height(24)
            .onClick(() => {
              this.title = this.title == '' ? this.noteValue.slice(0, 16) : this.title
              um_spi.sync_write_buffer_value(this.title, this.noteValue, getDate() + getTime())
              router.replace({
                url: 'pages/SpiDevice',
              })
            })
        }
      }
      .padding({ left: '2%' })

      Scroll(this.scroller) {
        Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Start }) {
          TextArea({ placeholder: '标题' })
            .fontSize(30)
            .placeholderFont({ size: 30 })
            .fontWeight(FontWeight.Medium)
            .height(50)
            .backgroundColor($r('app.color.background_white'))
            .onChange((value: string) => {
              this.title = value
            })
          Text(this.date + ' ' + this.time)
            .fontColor($r('app.color.placeholderColor_grey'))
            .fontSize(14)
            .padding({ left: '2%' })
          TextArea({ text: this.noteValue })
            .fontSize(18)
            .backgroundColor($r('app.color.background_white'))
            .flexGrow(1)
            .onChange((value: string) => {
              this.noteValue = value
              this.count = value.length
            })
          Row() {
            Text('共' + this.count + '字')
              .fontColor($r('app.color.placeholderColor_grey'))
              .fontSize(14)
          }
          .margin({ top: 5 })
          .width('100%')
          .justifyContent(FlexAlign.End)
          .alignItems(VerticalAlign.Bottom)

          Row()
            .visibility(Visibility.Hidden)
            .height(40)
        }
        .height('100%')
      }
    }
    .padding({ top: 14, right: '3%', left: '8%' })
  }
}