/**
 * @author: Mr.Hong
 */

import router from '@ohos.router';
import promptAction from '@ohos.promptAction';
import CommonConstants from '../common/constants/CommonConstants';

interface SwipeInterface {
  username: string,
  info: string,
  date: string,
  noRead: number,
  icon: ResourceStr,
  ip: string
  port: number
}

const params: SwipeInterface = router.getParams() as SwipeInterface;
const username: string = params.username;
const icon: ResourceStr = params.icon;

@Entry
@Component
struct ConnectPage {
  //启动四个连接框
  @State box1: number = -1
  @State box2: number = -1
  @State box3: number = -1
  @State box4: number = -1
  @State port: number = CommonConstants.LOCAL_POST //默认端口6666

  build() {
    Navigation() {
      Column({ space: 10 }) {
        Image(icon)
          .width($r('app.float.connectPage_icon_width'))
          .borderRadius($r('app.float.connectPage_icon_borderRadius'))
        Text(username)
          .fontSize($r('app.float.connectPage_userName_fontSize'))
        Text($r('app.string.roomIp'))
          .fontSize($r('app.float.connectPage_roomIp_fontSize'))
        Row({ space: 3 }) {
          TextInput()
            .type(InputType.Number)
            .width($r('app.float.connectPage_TextInput_width'))
            .onChange((value: string) => {
              this.box1 = Number.parseInt(value, 10)
            })
          Text('.').fontColor($r('app.color.connectPage_dotColor')).fontSize($r('app.float.connectPage_dot_fontSize'))
          TextInput()
            .type(InputType.Number)
            .width($r('app.float.connectPage_TextInput_width'))
            .onChange((value: string) => {
              this.box2 = Number.parseInt(value, 10)
            })
          Text('.').fontColor($r('app.color.connectPage_dotColor')).fontSize($r('app.float.connectPage_dot_fontSize'))
          TextInput()
            .type(InputType.Number)
            .width($r('app.float.connectPage_TextInput_width'))
            .onChange((value: string) => {
              this.box3 = Number.parseInt(value, 10)
            })
          Text('.').fontColor($r('app.color.connectPage_dotColor')).fontSize($r('app.float.connectPage_dot_fontSize'))
          TextInput()
            .type(InputType.Number)
            .width($r('app.float.connectPage_TextInput_width'))
            .onChange((value: string) => {
              this.box4 = Number.parseInt(value, 10)
            })
        }

        Text($r('app.string.roomNumber'))
          .fontSize($r('app.float.connectPage_roomPort_fontSize'))
        TextInput({ placeholder: '6666' })//默认端口号6666
          .type(InputType.Number)
          .textAlign(TextAlign.Center)
          .width($r('app.float.connectPage_roomPort_TextInput_width'))
          .onChange((value: string) => {
            this.port = Number.parseInt(value, 10)
          })
        Row({ space: 10 }) {
          Button($r('app.string.joinRoom'))
            .fontSize($r('app.float.joinRoom_fontSize'))
            .backgroundColor($r('app.color.joinRoom'))
            .onClick(() => {
              if (this.box1 != -1 && this.box2 != -1 && this.box3 != -1 && this.box4 != -1 &&
                this.box1 < 1000 && this.box2 < 1000 && this.box3 < 1000 && this.box4 < 1000) {
                params.ip = this.box1.toString() + '.' +
                this.box2.toString() + '.' +
                this.box3.toString() + '.' +
                this.box4.toString()
                params.port = this.port
                router.pushUrl({
                  url: 'pages/chatPage',
                  params: params
                }, router.RouterMode.Single)
              } else {
                promptAction.showToast({
                  message: CommonConstants.ERROR_MESSAGE,
                  duration: 1000
                })
                //开发板显示专用提示信息
                AlertDialog.show(
                  {
                    message: CommonConstants.ERROR_MESSAGE,
                    autoCancel: true,
                    alignment: DialogAlignment.Center,
                    gridCount: 4,
                    offset: { dx: 0, dy: -20 }
                  })
              }
            })
            .width($r('app.float.connectPage_joinRoom_width'))
            .height($r('app.float.connectPage_joinRoom_height'))
          Button($r('app.string.chatMessage'))
            .fontSize($r('app.float.chatRecord_Button_fontSize'))
            .backgroundColor($r('app.color.chatRecord'))
            .onClick(() => {
              router.pushUrl({
                url: 'pages/chatRecord',
                params: params
              }, router.RouterMode.Single)
            })
            .width($r('app.float.chatRecord_Button_width'))
            .height($r('app.float.chatRecord_Button_height'))
        }
      }
      .justifyContent(FlexAlign.Start)
    }
    .title($r('app.string.buildRoom'))
    .titleMode(NavigationTitleMode.Mini)
    .hideTitleBar(false)
    .navBarWidth(CommonConstants.FULL_PARENT)
  }
}