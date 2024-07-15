/**
 * @author: Mr.Hong
 */

/**
 * 聊天界面待完善：
 * 1.右边头像应该由登录账号决定     √
 * 2.左边还未将获取的消息展示，并附带头像     √
 * 3.聊天数据的存储和获取     √
 */


import router from '@ohos.router'
import socket from '@ohos.net.socket'
import wifiManager from '@ohos.wifiManager';
import { BusinessError } from '@ohos.base';
import util from '@ohos.util';
import ChatTable from '../common/database/tables/ChatTable'
import ChatData from '../viewmodel/ChatData'
import CommonConstants from '../common/constants/CommonConstants';

//获取传入的用户相关信息
//设置传入参数-----------------------------------------------------------
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
const aimIp: string = params.ip;
const aimPort: number = params.port;


//socket-----------------------------------------------------------------
let ipAddress = wifiManager.getIpInfo().ipAddress; //获取本地ip地址
let udp = socket.constructUDPSocketInstance(); //创建UDPSocket连接

class SocketInfo {
  public message: ArrayBuffer = new ArrayBuffer(1);
  public remoteInfo: socket.SocketRemoteInfo = {} as socket.SocketRemoteInfo;
}

//ArrayBuffer转utf8字符串
function buf2String(buf: ArrayBuffer) {
  let msgArray = new Uint8Array(buf);
  let textDecoder = util.TextDecoder.create('utf-8');
  return textDecoder.decodeWithStream(msgArray)
}

let tooTmp: NavigationMenuItem = { 'value': '', 'icon': './resources/rawfile/images/threePoint.png', 'action': () => {
  router.pushUrl({
    url: 'pages/chatInformation',
    //传参
    params: params
  }, router.RouterMode.Single)
} }

@Entry
@Component
struct Index {
  @State textInput: string = ''
  //初始化sql
  private chatTable = new ChatTable(() => {
  })
  //聊天数据
  @State chatData: string = '' //聊天信息
  @State isMe: number = 0 //标记
  //socket连接
  @State sendMessage: string = '' //要发送的信息
  @State getMessage: string = '' //接收到的信息
  @State remoteIp: string = aimIp //目标ip地址
  @State remotePort: number = aimPort //目标端口号
  private localIp: string = (ipAddress >>> 24) + '.' + (ipAddress >> 16 & 0xFF) + '.' + (ipAddress >> 8 & 0xFF) + '.' + (ipAddress & 0xFF);
  private localPort: number = CommonConstants.LOCAL_POST //本地端口号
  private scroller: Scroller = new Scroller()
  @State private chatMessage: ChatData[] = [] //聊天记录

  Bind() {

    console.log('本地IP信息地址为:', this.localIp);

    //订阅事件message、close
    udp.on('message', (value: SocketInfo) => {

      let msg = buf2String(value.message);
      this.chatData = buf2String(value.message); //保存到中间变量
      this.isMe = 0; //从其他设备收到的消息标记为0

      const newUser: ChatData = { id: 0, who: username, chatData: this.chatData, isMe: this.isMe }
      this.chatTable.insertData(newUser, (id: number) => {
        newUser.id = id
      })

      this.chatMessage.push(newUser) //将接收到的数据进行追加

      console.log('on message: ' + JSON.stringify(msg));
      console.log('remoteInfo: ' + JSON.stringify(value.remoteInfo));
    });
    udp.on('listening', () => {
      console.log('on listening success');
    });
    udp.on('close', () => {
      console.log('on close success');
    });

    //绑定本地IP地址和端口。
    let bindAddress: socket.NetAddress = {
      address: this.localIp, //本地ip地址
      port: this.localPort, // 本地端口号
    };
    udp.bind(bindAddress, (err: BusinessError) => { //bind方法:绑定 IP 地址和端口。
      if (err) {
        console.log('bind fail');
        return;
      }
      console.log('bind success');
    });
  };

  Send() {
    let remoteAddress: socket.NetAddress = {
      address: this.remoteIp, // 目标ip地址
      port: this.remotePort, // 目标端口号
    };
    // 发送数据
    let sendOptions: socket.UDPSendOptions = {
    } as socket.UDPSendOptions; //声明变量sendSendOptions，类型为socket.UDPSendOptions，并将其初始化为空对象
    sendOptions.data = this.sendMessage; //要传输的数据的内容

    this.chatData = this.sendMessage; //保存到中间变量
    this.isMe = 1; //自己设备发送的消息标记为1
    const newUser: ChatData = { id: 0, who: username, chatData: this.chatData, isMe: this.isMe }
    this.chatMessage.push(newUser) //将要发送的数据进行追加
    this.chatTable.insertData(newUser, (id: number) => {
      newUser.id = id
    })

    sendOptions.address = remoteAddress;
    udp.send(sendOptions, (err: BusinessError) => { //用send方法传输数据“sendSendOptions”
      if (err) {
        console.log('send fail');
        return;
      }
      console.log('send success');
    })
  }

  Close() {
    // 连接使用完毕后，主动关闭。取消相关事件的订阅。
    setTimeout(() => {
      udp.close((err: BusinessError) => { //使用close方法关闭Tcp连接
        if (err) {
          console.log('close fail');
          return;
        }
        console.log('close success.');
      });
      udp.off('message'); //依次取消各事件的订阅
      udp.off('close');
    });
  }

  aboutToAppear() {
    this.Bind()

    //调用chatTable中的query方法给字符串数组arr和整形数组sign赋值
    this.chatTable.getRdbStore(() => {
      this.chatTable.query((result: ChatData[]) => {
        this.chatMessage = result
      })
    })
  }

  aboutToDisappear() {
    this.Close()
  }

  @Builder
  inputAndSend() {
    Row() {
      //文本编辑栏
      TextInput({ placeholder: $r('app.string.textInput_hint'), text: this.textInput,
        controller: this.controller })
        .width(CommonConstants.TEXT_INPUT_BOX_WIDTH)
        .height($r('app.float.chatPage_textInput_height'))
        .margin({ left: $r('app.float.chatPage_textInput_margin') })
        .backgroundColor($r('app.color.chatTextArea'))
        .fontSize($r('app.float.chatPage_textInput_fontSize'))
        .fontColor($r('app.color.chatPage_textInput_fontColor'))
        .onChange((value: string) => {
          this.textInput = value
        })
      Text($r('app.string.send'))
        .borderRadius($r('app.float.chatPage_sendButton_borderRadius'))
        .backgroundColor($r('app.color.chatPage_sendButton_blackGround'))
        .width(CommonConstants.SEND_BUTTON_WIDTH)
        .height($r('app.float.chatPage_sendButton_height'))
        .margin({ left: $r('app.float.chatPage_sendButton_margin') })
        .textAlign(TextAlign.Center)
        .fontSize($r('app.float.chatPage_sendHint_fontSize'))
        .fontColor($r('app.color.chatPage_sendHint_fontColor'))
        .onClick(() => {
          //处理发送消息
          if (this.textInput != '') {
            //设置内容到文本域里
            this.sendMessage = this.textInput
            this.Send()
            //清空TextArea里的文本
            this.textInput = ''
          }
        })
    }
    .width(CommonConstants.FULL_PARENT)
    .margin({ top: 0, bottom: 0 })
    .backgroundColor($r('app.color.white'))
  }

  controller: TextInputController = new TextInputController();

  build() {
    Navigation() {
      List({ scroller: this.scroller }) {
        ForEach(this.chatMessage, (item: ChatData, index: number) => {
          //判断是什么数据再展示
          if (item.isMe == 1 && item.who == username) { //自己发送的数据
            ListItem() {
              Row({ space: 5 }) {
                Text(item.chatData)
                  .copyOption(CopyOptions.InApp)
                  .enabled(false)
                  .flexShrink(1)
                  .backgroundColor($r('app.color.chatMessage'))
                  .borderRadius($r('app.float.chatPage_chatMessage_borderRadius'))
                  .fontSize($r('app.float.chatPage_chatMessage_fontSize'))
                  .fontColor($r('app.color.chatPage_chatMessage_fontColor'))
                  .textAlign(TextAlign.Start)
                  .margin({ top: $r('app.float.chatPage_chatMessage_margin') })
                  .padding({
                    left: $r('app.float.chatPage_chatMessage_padding_left'),
                    top: $r('app.float.chatPage_chatMessage_padding_top'),
                    bottom: $r('app.float.chatPage_chatMessage_padding_bottom'),
                    right: $r('app.float.chatPage_chatMessage_padding_right')
                  })
                Button()
                  .backgroundImage($rawfile('images/dai.jpg'))
                  .backgroundImageSize(ImageSize.Contain)
                  .align(Alignment.Center)
                  .padding({ right: $r('app.float.chatPage_head_padding') })
                  .margin({
                    top: $r('app.float.chatPage_head_margin_top'),
                    right: $r('app.float.chatPage_head_margin_right')
                  })
                  .width($r('app.float.headWidthAndHeight'))
                  .height($r('app.float.headWidthAndHeight'))
              }
              .alignItems(VerticalAlign.Top)
              .justifyContent(FlexAlign.End)
              .width(CommonConstants.FULL_PARENT)
              .margin({
                top: $r('app.float.chatPage_ListItem_margin_top'),
                right: $r('app.float.chatPage_ListItem_margin_right')
              })
              .padding({ right: $r('app.float.chatPage_ListItem_padding') })
            }
            .width(CommonConstants.FULL_PARENT)
          } else if (item.isMe == 0 && item.who == username) {
            ListItem() {
              Row({ space: 5 }) {
                Button()
                  .backgroundImage(icon)
                  .backgroundImageSize(ImageSize.Contain)
                  .align(Alignment.Center)
                  .padding({ right: $r('app.float.chatPage_head_padding') })
                  .margin({
                    top: $r('app.float.chatPage_head_margin_top'),
                    right: $r('app.float.chatPage_head_margin_right')
                  })
                  .width($r('app.float.headWidthAndHeight'))
                  .height($r('app.float.headWidthAndHeight'))
                Text(item.chatData)
                  .copyOption(CopyOptions.InApp)
                  .enabled(false)
                  .flexShrink(1)
                  .backgroundColor($r('app.color.chatMessage'))
                  .borderRadius($r('app.float.chatPage_chatMessage_borderRadius'))
                  .fontSize($r('app.float.chatPage_chatMessage_fontSize'))
                  .fontColor($r('app.color.chatPage_chatMessage_fontColor'))
                  .textAlign(TextAlign.Start)
                  .margin({ top: $r('app.float.chatPage_chatMessage_margin') })
                  .padding({
                    left: $r('app.float.chatPage_chatMessage_padding_left'),
                    top: $r('app.float.chatPage_chatMessage_padding_top'),
                    bottom: $r('app.float.chatPage_chatMessage_padding_bottom'),
                    right: $r('app.float.chatPage_chatMessage_padding_right')
                  })
              }
              .alignItems(VerticalAlign.Top)
              .justifyContent(FlexAlign.Start)
              .width(CommonConstants.FULL_PARENT)
              .margin({
                top: $r('app.float.chatPage_ListItem_margin_top'),
                right: $r('app.float.chatPage_ListItem_margin_right')
              })
              .padding({ right: $r('app.float.chatPage_ListItem_padding') })
            }
            .width(CommonConstants.FULL_PARENT)
          }
        })
      }
      .onScrollIndex(() => {
        this.scroller.scrollToIndex(Alignment.Bottom)
      })
    }
    //应用params.username
    .title(username)
    .titleMode(NavigationTitleMode.Mini)
    .hideTitleBar(false)
    .menus([tooTmp])
    .toolbarConfiguration(this.inputAndSend())
    .navBarWidth(CommonConstants.FULL_PARENT)
  }
}
