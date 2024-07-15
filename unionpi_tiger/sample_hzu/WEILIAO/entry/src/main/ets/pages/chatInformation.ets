/**
 * @author: Mr.Hong
 */
import router from '@ohos.router';
import CommonConstants from '../common/constants/CommonConstants';

const alphabets = ['→', '※', 'A', 'B', 'C', 'D', 'E', 'F', 'H', 'I',
  'G', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
  'W', 'X', 'Y', 'Z', '#'];

interface CarItemInterface {
  image: ResourceStr
  title: string
}

interface SwipeInterface {
  username: string,
  info: string,
  date: string,
  noRead: number,
  icon: ResourceStr,
}

const params: SwipeInterface = router.getParams() as SwipeInterface;
const username: string = params.username;
const icon: ResourceStr = params.icon;

@Entry
@Component
struct chatInformation {
  private scroller: Scroller = new Scroller()

  build() {
    Column() {
      Navigation() {
        List({
          space: 10
        }) {
          ListItem() {
            Row({ space: 20 }) {
              Column() {
                //这里按钮跳转到好友个人信息界面----------->暂不实现
                Button() {
                  Image(icon)
                    .width($r('app.float.headWidthAndHeight'))
                    .height($r('app.float.headWidthAndHeight'))
                    .borderRadius($r('app.float.chatInformation_borderRadius'))
                }
                .width($r('app.float.headWidthAndHeight'))
                .height($r('app.float.headWidthAndHeight'))
                .borderRadius($r('app.float.chatInformation_borderRadius'))

                Text(username)
                  .fontSize($r('app.float.chatInformation_usernameSize'))
              }

              Column() {
                Button() {
                  Image($r('app.media.add_box'))
                    .width($r('app.float.headWidthAndHeight'))
                    .height($r('app.float.headWidthAndHeight'))
                }
                .backgroundColor($r('app.color.listBackGround'))
                .width($r('app.float.headWidthAndHeight'))
                .height($r('app.float.headWidthAndHeight'))

                Text('')
                  .fontSize($r('app.float.chatInformation_usernameSize'))
              }
              .backgroundColor($r('app.color.listBackGround'))
            }
            .alignItems(VerticalAlign.Top)
            .backgroundColor($r('app.color.listBackGround'))
            .width(CommonConstants.FULL_PARENT)
            .padding($r('app.float.chatInformation_list_padding'))
          }

          ListItem() {
            RowCustomComponent({ title: '查看聊天记录' })
          }
          .width(CommonConstants.FULL_PARENT)
          .onClick(() => {
            router.pushUrl({
              url: 'pages/chatRecord',
              params: params
            }, router.RouterMode.Single)
          })

          ListItem() {
            Column() {
              RowCustomComponentCheck({ title: '消息免打扰' })
              Divider()
                .strokeWidth(1)
                .color('#eeeeee')
                .padding({ left: 54 })
              RowCustomComponentCheck({ title: '置顶聊天' })
              Divider()
                .strokeWidth(1)
                .color('#eeeeee')
                .padding({ left: 54 })
            }.backgroundColor($r('app.color.listBackGround'))
          }
          .width(CommonConstants.FULL_PARENT)

          ListItem() {
            RowCustomComponent({ title: '设置当前聊天背景' })
          }
          .width(CommonConstants.FULL_PARENT)

          ListItem() {
            RowCustomComponent({ title: '清空聊天记录' })
          }
          .width(CommonConstants.FULL_PARENT)

          ListItem() {
            RowCustomComponent({ title: '投诉' })
          }
          .width(CommonConstants.FULL_PARENT)
        }
        .onScrollIndex(() => {
          this.scroller.scrollToIndex(Alignment.Bottom)
        })
      }
      .title('聊天信息')
      .titleMode(NavigationTitleMode.Mini)
      .hideTitleBar(false)
      .navBarWidth(CommonConstants.FULL_PARENT)
    }
    .width(CommonConstants.FULL_PARENT)
    .height(CommonConstants.FULL_PARENT)
    .backgroundColor('#eeeeee')
    .height(CommonConstants.FULL_PARENT)
  }
}

//自定义组件
@Component
struct RowCustomComponent {
  private title: string = ''

  build() {
    Row() {
      Row() {
        Text(this.title)
          .fontSize($r('app.float.chatInformation_title_fontSize'))
          .margin({
            left: $r('app.float.chatInformation_title_margin')
          })
      }

      Image($rawfile('images/go.png'))
        .width(13)
        .height(23)
    }
    .justifyContent(FlexAlign.SpaceBetween)
    .backgroundColor($r('app.color.listBackGround'))
    .width(CommonConstants.FULL_PARENT)
    .padding($r('app.float.chatInformation_list_padding'))
  }
}

@Component
struct RowCustomComponentCheck {
  private title: string = ''

  build() {
    Row() {
      Row() {
        Text(this.title)
          .fontSize($r('app.float.chatInformation_title_fontSize'))
          .margin({
            left: $r('app.float.chatInformation_title_margin')
          })
      }

      Toggle({ type: ToggleType.Switch, isOn: false })
        .selectedColor($r('app.color.chatInformation_toggle_selectedColor'))
        .switchPointColor($r('app.color.chatInformation_toggle_switchPointColor'))
        .onChange((isOn: boolean) => {
          console.info('Component status:' + isOn)
        })
    }
    .justifyContent(FlexAlign.SpaceBetween)
    .backgroundColor($r('app.color.listBackGround'))
    .width(CommonConstants.FULL_PARENT)
    .padding($r('app.float.chatInformation_list_padding'))
  }
}