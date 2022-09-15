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
import { deleteDialog, noteItem, clickItem } from '../utils/componentUtil'
import um_spi from '@ohos.um_a311d';
import prompt from '@ohos.prompt';

/**
 * 家庭备忘录界面
 */
@Entry
@Component
struct SpiDeviceControl {
  scroller: Scroller = new Scroller()
  @State index: number = 0
  @State title: string = ''
  @State date: string = ''
  @State time: string = ''
  dialogController: CustomDialogController = new CustomDialogController({
    builder: deleteDialog({ confirm: this.onAccept }),
    customStyle: true,
    autoCancel: false
  })

  onAccept() {
    um_spi.sync_all_erase()
  }

  aboutToAppear() {
    NoteArray.splice(0, NoteArray.length)
    let read_sum_list = um_spi.sync_read_sum_list();
    for (var i = 0; i < read_sum_list; i++) {
      let str = um_spi.sync_read_date(i)
      let index = str.indexOf("日")
      NoteArray.push(new note(um_spi.sync_read_title(i), str.slice(0, index + 1), str.slice(index + 1, str.length), um_spi.sync_read_value(i)))
    }
  }

  build() {
    Row() {
      Stack({ alignContent: Alignment.BottomEnd }) {
        Flex({ direction: FlexDirection.Column }) {
          Row() {
            Text('全部笔记')
              .fontSize(30)
              .height(80)
              .fontWeight(FontWeight.Medium)
          }
          .margin({ bottom: 5, left: 10 })

          if (NoteArray.length === 0) {
            Column() {
              Row() {
                Text('空')
                  .fontColor($r('app.color.placeholderColor_grey'))
              }.height('80%')
            }.width('88%')

          } else {
            Scroll(this.scroller) {
              Column({ space: 10 }) {
                ForEach(NoteArray.map((item1, index1) => {
                  return { index: index1, data: item1 }
                }), item => {
                  noteItem({
                    index: item.index,
                    title: item.data.title,
                    date: item.data.date,
                    backgroundColor: this.index == item.index ? $r('app.color.noteItemColor_blue') : $r('app.color.background_white'),
                    callback: () => {
                      if (this.index != item.index) {
                        this.index = item.index
                      }
                    }
                  })
                })
                Row()
                  .visibility(Visibility.Hidden)
                  .width('98%')
                  .height(141)
                  .backgroundColor($r('app.color.placeholderColor_grey'))
              }
            }
            .scrollBar(BarState.Off)
          }
        }
        .width('100%')
        .height('100%')

        Button({ type: ButtonType.Circle }) {
          Image($r('app.media.ic_public_add_filled'))
            .width(24)
            .height(24)
        }
        .width(50)
        .height(50)
        .margin({ right: 25, bottom: 86 })
        .backgroundColor($r('app.color.checked_blue'))
        .onClick(() => {
          router.replace({
            url: 'pages/noteNewPage',
            params: {
              date: getDate(),
              time: getTime(),
            }
          })
        })

        Flex({ direction: FlexDirection.Row, justifyContent: FlexAlign.SpaceAround }) {
          Column() {
            Image($r('app.media.ic_public_notes'))
              .width(24)
              .height(24)
            Text('笔记')
              .margin({ top: 5 })
              .fontSize(12)
              .fontColor($r('app.color.checked_blue'))
          }

          Column() {
            Image($r('app.media.ic_public_todo'))
              .width(24)
              .height(24)
            Text('代办')
              .margin({ top: 5 })
              .fontSize(12)
          }
        }
        .padding({ top: 10, bottom: 10 })
        .backgroundColor('#F2F3F5')
      }
      .padding({ left: '6%' })
      .backgroundColor('#F2F3F5')
      .width('40%')
      .height('100%')

      Divider()
        .vertical(true)
        .color('#D1D1D1')
        .strokeWidth(2)

      Stack({ alignContent: Alignment.BottomStart }) {
        if (NoteArray.length != 0) {
          Scroll(this.scroller) {
            Flex({ direction: FlexDirection.Column, justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Start }) {
              Row() {
                Text(NoteArray[this.index].title)
                  .fontWeight(FontWeight.Medium)
                  .fontSize(30)
              }
              .alignItems(VerticalAlign.Bottom)
              .height('20%')

              Text(NoteArray[this.index].date + ' ' + NoteArray[this.index].time)
                .fontColor($r('app.color.placeholderColor_grey'))
                .fontSize(14)
                .margin({ top: 15 })
              Text(NoteArray[this.index].content)
                .fontSize(18)
                .lineHeight(25)
                .margin({ top: 15 })
              Row() {
                Text('共' + NoteArray[this.index].content.length + '字')
                  .fontColor($r('app.color.placeholderColor_grey'))
                  .fontSize(14)
              }
              .padding({ top: 50 })
              .width('100%')
              .height('41%')
              .justifyContent(FlexAlign.End)
              .alignItems(VerticalAlign.Bottom)
            }
            .padding(16)
            .margin({ bottom: 61 })
            .onClick(() => {
              router.replace({
                url: 'pages/noteEditPage',
                params: {
                  index: this.index,
                },
              })
            })
          }
        }

        Flex({ direction: FlexDirection.Row, justifyContent: FlexAlign.SpaceAround }) {
          clickItem({ imgSrc: $r('app.media.ic_public_share'), text: '分享' })
          clickItem({ imgSrc: $r('app.media.ic_public_delete'), text: '删除', callback: () => {
            this.dialogController.open()
          } })
          clickItem({ imgSrc: $r('app.media.ic_public_more'), text: '更多' })
        }
        .padding({ top: 10, bottom: 10 })
        .backgroundColor($r('app.color.background_white'))
      }
      .height('100%')
      .width('60%')
    }
    .width('100%')
  }
}