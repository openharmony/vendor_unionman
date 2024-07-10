/**
 * @author: Mr.Hong
 */

import router from '@ohos.router';
import CommonConstants from '../common/constants/CommonConstants';
import ChatTable from '../common/database/tables/ChatTable';
import ChatData from '../viewmodel/ChatData';

interface SwipeInterface {
  username: string,
  icon: ResourceStr,
}

const params: SwipeInterface = router.getParams() as SwipeInterface;
const username: string = params.username;
const icon: ResourceStr = params.icon;


@Entry
@Component
struct ChatRecord {
  private chatTable = new ChatTable(() => {
  })
  private scroller: Scroller = new Scroller()
  @State private chatMessage: ChatData[] = [] //聊天记录

  aboutToAppear() {
    this.chatTable.getRdbStore(() => {
      this.chatTable.query((result: ChatData[]) => {
        this.chatMessage = result
      })
    })
  }

  build() {
    Navigation() {
      List({ scroller: this.scroller }) {
        ForEach(this.chatMessage, (item: ChatData, index: number) => {
          //判断是什么数据再展示
          if (item.isMe == 1) { //自己发送的数据
            ListItem() {
              Row({ space: 5 }) {
                Text(item.chatData)
                  .copyOption(CopyOptions.InApp)
                  .enabled(false)
                  .width(CommonConstants.HALF_PARENT)
                  .backgroundColor($r('app.color.chatMessage'))
                  .borderRadius($r('app.float.chatRecord_chatMessage_borderRadius'))
                  .fontSize($r('app.float.chatRecord_chatMessage_fontSize'))
                  .fontColor($r('app.color.chatRecord_chatMessage_fontColor'))
                  .textAlign(TextAlign.Start)
                  .margin({ top: $r('app.float.chatRecord_chatMessage_margin') })
                  .padding({
                    left: $r('app.float.chatRecord_chatMessage_padding_left'),
                    top: $r('app.float.chatRecord_chatMessage_padding_top'),
                    bottom: $r('app.float.chatRecord_chatMessage_padding_bottom'),
                    right: $r('app.float.chatRecord_chatMessage_padding_right')
                  })
                Button()
                  .backgroundImage($rawfile('images/dai.jpg'))
                  .backgroundImageSize(ImageSize.Contain)
                  .align(Alignment.Center)
                  .padding({ right: $r('app.float.chatRecord_head_padding') })
                  .margin({
                    top: $r('app.float.chatRecord_head_margin_top'),
                    right: $r('app.float.chatRecord_head_margin_right')
                  })
                  .width($r('app.float.headWidthAndHeight'))
                  .height($r('app.float.headWidthAndHeight'))
              }
              .alignItems(VerticalAlign.Top)
              .justifyContent(FlexAlign.End)
              .width(CommonConstants.FULL_PARENT)
              .margin({
                top: $r('app.float.chatRecord_ListItem_margin_top'),
                right: $r('app.float.chatRecord_ListItem_margin_right')
              })
              .padding({ right: $r('app.float.chatRecord_ListItem_padding') })
            }
            .width(CommonConstants.FULL_PARENT)
          } else {
            ListItem() {
              Row({ space: 5 }) {
                Button()
                  .backgroundImage(icon)
                  .backgroundImageSize(ImageSize.Contain)
                  .align(Alignment.Center)
                  .padding({ right: $r('app.float.chatRecord_head_padding') })
                  .margin({
                    top: $r('app.float.chatRecord_head_margin_top'),
                    right: $r('app.float.chatRecord_head_margin_right')
                  })
                  .width($r('app.float.headWidthAndHeight'))
                  .height($r('app.float.headWidthAndHeight'))
                Text(item.chatData)
                  .copyOption(CopyOptions.InApp)
                  .enabled(false)
                  .width(CommonConstants.HALF_PARENT)
                  .backgroundColor($r('app.color.chatMessage'))
                  .borderRadius($r('app.float.chatRecord_chatMessage_borderRadius'))
                  .fontSize($r('app.float.chatRecord_chatMessage_fontSize'))
                  .fontColor($r('app.color.chatRecord_chatMessage_fontColor'))
                  .textAlign(TextAlign.Start)
                  .margin({ top: $r('app.float.chatRecord_chatMessage_margin') })
                  .padding({
                    left: $r('app.float.chatRecord_chatMessage_padding_left'),
                    top: $r('app.float.chatRecord_chatMessage_padding_top'),
                    bottom: $r('app.float.chatRecord_chatMessage_padding_bottom'),
                    right: $r('app.float.chatRecord_chatMessage_padding_right')
                  })
              }
              .alignItems(VerticalAlign.Top)
              .justifyContent(FlexAlign.Start)
              .width(CommonConstants.FULL_PARENT)
              .margin({
                top: $r('app.float.chatRecord_ListItem_margin_top'),
                right: $r('app.float.chatRecord_ListItem_margin_right')
              })
              .padding({ right: $r('app.float.chatRecord_ListItem_padding') })
            }
            .width(CommonConstants.FULL_PARENT)
          }
        })
      }
      .onScrollIndex(() => {
        this.scroller.scrollToIndex(Alignment.Bottom)
      })
    }
    .title('与 ' + username + ' 的聊天记录')
    .titleMode(NavigationTitleMode.Mini)
    .hideTitleBar(false)
  }
}