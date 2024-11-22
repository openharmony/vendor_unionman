
import http from '@ohos.net.http';
import webSocket from '@ohos.net.webSocket';
import wifiManager from '@ohos.wifiManager';
import ethernet from '@ohos.net.ethernet';
import renderNapi from 'libentry.so'
import common from '@ohos.app.ability.common'
import { CameraHelper } from './CameraHelper';
import { BusinessError } from '@ohos.base';
import {
  CloseMsg,
  ReceiveMessageBase,
  ReceiveRequestAckAgreeMsg,
  ReceiveRequestMsg,
  ReceiveRequestAckDeniedMsg,
  ReceiveRequestAckMsg
} from '../module/ReceiveServiceMessage';
import { SendCloseMsg, SendRequestAckMsg, SendRequestMsg } from '../module/SendServiceMessage';
import util from '@ohos.util';

import {PreferenceModel} from '../module/db';
import { CustomDialogExample } from '../module/CustomDialogExample'
const DOMAIN = 0xff81;

@Entry
@Component
struct Index {
  @StorageLink('isShow') isShow: boolean = false;
  @State showPreview: boolean = false;
  @State btnConnectText:string="连接";
  @State enableInput: boolean = true;
  @State enableButton: boolean = true;

  context: common.UIAbilityContext = getContext(this) as common.UIAbilityContext
  defaultPlayUrl: string = '192.168.1.146:1985'
  defaultSrsUrl: string = '192.168.1.146:1985'
  defaultServerIp: string = "192.168.1.179"
  private playUrl: string = ''
  @State srsUrl: string = ''
  private pushUrl: string = "";
  playUrlController: TextInputController = new TextInputController()
  srsUrlController: TextInputController = new TextInputController()
  xcomponentController1: XComponentController = new XComponentController()
  xcomponentController2: XComponentController = new XComponentController()

  @State serverIp: string = "";
  private ip: string = "";
  @State onlineList: string[] = [];
  @State logList: string[] = [];
  private wsCon: webSocket.WebSocket = webSocket.createWebSocket();
  private isCon: boolean = false;
  private callUser: string = "";
  private isCalling: boolean = false;
  private isPush: boolean = false;
  private isPlaying: boolean = false;

  private isPreviewLoad = false;
  private mPreviewController = new XComponentController();
  private mCameraHelper: CameraHelper | null = null;

  private db: PreferenceModel = new PreferenceModel();
  flushOnlineList() {
    let httpReq = http.createHttp();
    httpReq.request(
      "http://" + this.serverIp + ":5555/getOnlineList",
      {
        method: http.RequestMethod.GET,
        header: [{
          'Content-Type': 'application/json'
        }]
      }, (err: BusinessError, data: http.HttpResponse) => {
        if (!err) {
          let list: string[] = JSON.parse(data.result as string);
          let selfIndex = list.indexOf(this.ip, 0);
          if (selfIndex != -1) {
            list[selfIndex] = list[selfIndex].concat("(本机ip)")
            //list.splice(selfIndex, 1);
          }
          this.onlineList = list;
          this.logList.push("成功获取在线列表")
        } else {
          this.logList.push(err.message);
          this.btnConnectText = "连接";
          this.enableInput = true;
          this.enableButton = true;
        }
    });
  }

  async getLocalIp(): Promise<string> {
    try {
      let isActive = await ethernet.isIfaceActive("eth0");
      if (isActive == 1) {
        let ipConfig = await ethernet.getIfaceConfig("eth0");
        console.log("sssk ipConfig.ipAddr==>"+ipConfig.ipAddr);

        let ip = ""
        ipConfig.ipAddr.split(",").forEach((item, index, array) => {
          if (item.indexOf(".") != -1) {
            console.log(item.trim());
            ip = item.trim();
          }
        });
        if(ip !== ""){
          return ip
        }
      }
    } catch (e) {
      console.log("sssk==>"+e.message);
    }

    try {
      let ipNumber = wifiManager.getIpInfo().ipAddress;
      return (ipNumber >>> 24) + '.' + (ipNumber >> 16 & 0xFF) + '.' + (ipNumber >> 8 & 0xFF) + '.' + (ipNumber & 0xFF);
    } catch (e) {
      console.log(e.message);
      return "";
    }
  }

  private websocketTimer:number = -1;
  setWebSocketEvent() {
    this.wsCon.on("open", (err: BusinessError, value: Object) => {
      this.enableButton = true
      if (!err) {
        this.isCon = true;
        this.logList.push("成功连接中转服务器")
        setTimeout(()=>{
          this.btnConnectText = "断开"
        })
        this.websocketTimer = setInterval(() => {
          if(this.isCon) {
            this.flushOnlineList();
            this.wsCon.send("ping")
          }
        }, 2000)
      }
    })
    this.wsCon.on("message", (err: BusinessError, value: string | ArrayBuffer) => {
      if (!err) {
        let str = value as string;
        if (str.charAt(0) === '{') {
          let msg: ReceiveMessageBase = JSON.parse(str);
          this.processHandler(msg);
        }
      }
    })
    this.wsCon.on("close", (err: BusinessError, result: webSocket.CloseResult) => {
      this.enableButton = true
      if (!err) {
        if(this.isCon) {
          clearInterval(this.websocketTimer) // 先关闭定时器
        }

        setTimeout(() => {
          this.btnConnectText = "连接";
        }, 1000);
        this.isCon = false;
        this.logList.length = 0;
        this.logList.push("与服务器断开连接");
        this.onlineList.length = 0;
        this.handUp()
        this.closePlayAndPush()
        this.callUser = "";
      }
    })
  }

  processHandler(msg: ReceiveMessageBase) {
    switch (msg.type) {
      case "request":
        this.onRequest(msg as ReceiveRequestMsg);
        break;
      case "requestAck":
        this.onRequestAck(msg as ReceiveRequestAckMsg);
        break;
      case "close":
        this.onClose(msg as CloseMsg);
        break;
      default:
        return;
    }
  }
  private dialogController: CustomDialogController | null = null;

  onRequest(msg: ReceiveRequestMsg) {
    let sendMsg: SendRequestAckMsg = new SendRequestAckMsg();
    sendMsg.target = msg.source;
    // 定义自定义弹窗的Controller，传入参数和回调函数
    this.dialogController = new CustomDialogController({
      builder: CustomDialogExample({
        title: "来电",
        cancel: ()=>{
          sendMsg.opinion = "denied";
          this.wsCon.send(JSON.stringify(sendMsg));
          this.logList.push("已拒绝" + msg.source + "的通话请求");
          this.dialogController = null
        },
        confirm: ()=>{
          sendMsg.opinion = "agree";
          this.wsCon.send(JSON.stringify(sendMsg));
          this.isCalling = true;
          this.callUser = msg.source;
          this.logList.push("已同意" + msg.source + "的通话请求", msg.source + "的推流地址" + msg.url);

          this.initPush(this.ip);

          if(this.srsUrl == '') {
            this.srsUrl = this.defaultSrsUrl;
          }
          this.playUrl = "webrtc://" + this.srsUrl + "/live/" + this.ipToBase64(msg.source);
          console.log("AnyChat request playUrl:" + this.playUrl);
          let playFile = "";//this.context.filesDir+"/play.pcm";
          renderNapi.setXCoponentUrl("xcomponent2", this.playUrl, playFile);
          this.isPlaying = true;
          this.dialogController = null
        },
        inputValue: msg.source + "用户请求通话",
      }),
      cancel: ()=>{},
      autoCancel: true,
      alignment: DialogAlignment.Bottom,
      offset: { dx: 0, dy: -20 },
      gridCount: 4,
      customStyle: false
    })
    this.dialogController.open()
  }

  onRequestAck(msg: ReceiveRequestAckMsg) {
    switch (msg.opinion) {
      case "agree":
        this.onRequestAckAgree(msg as ReceiveRequestAckAgreeMsg);
        break;
      case "denied":
        this.onRequestAckDenied(msg as ReceiveRequestAckDeniedMsg);
        break;
      default:
        return;
    }
  }

  onRequestAckAgree(msg: ReceiveRequestAckAgreeMsg) {
    this.isCalling = true;
    this.callUser = msg.source;
    this.logList.push(msg.source + "已接受连接请求!", "推流地址" + msg.url);

    this.initPush(this.ip);

    if(this.srsUrl == '') {
      this.srsUrl = this.defaultSrsUrl;
    }
    this.playUrl = "webrtc://" + this.srsUrl + "/live/" + this.ipToBase64(msg.source);
    console.log("AnyChat onClick playUrl:" + this.playUrl);
    let playFile = "";//this.context.filesDir+"/play.pcm";
    renderNapi.setXCoponentUrl("xcomponent2", this.playUrl, playFile);
    this.isPlaying = true;
  }

  onRequestAckDenied(msg: ReceiveRequestAckDeniedMsg) {
    this.logList.push(msg.source + "拒绝连接请求!");
  }

  onClose(msg: CloseMsg) {
    this.closePlayAndPush();
    this.isCalling = false;
    this.callUser = "";
    this.logList.push(msg.source + "已结束视频通话");
  }

  saveParam(){
    this.db.writeData('srs_url', this.srsUrl);
    this.db.writeData('mqtt_ip', this.serverIp);
  }
  build() {
    Row() {
      Column({space: 20}) {
        Row() {
          Column({}) {
            Row() {
              GridRow({ columns: { sm: 4, md: 8, lg: 12 } }) {
                GridCol({
                  span: { sm: 1, md: 2, lg: 3 }
                }) {
                  Text("SRS Url:")
                    .fontColor(Color.Black)
                    .height(40)
                }

                GridCol({
                  span: { sm: 2, md: 4, lg: 6 }
                }) {
                  TextInput({ text: this.srsUrl })
                    .placeholderColor(Color.Grey)
                    .caretColor(Color.Blue)
                    .enabled(this.enableInput)
                    .fontColor(Color.Black)
                    .height(40)
                    .onChange((value: string) => {
                      this.srsUrl = value
                    })
                }
              }
            } //Text+TextInput
            Row() {
              GridRow({ columns: { sm: 4, md: 8, lg: 12 } }) {
                GridCol({
                  span: { sm: 1, md: 2, lg: 3 }
                }) {
                  Text("MQTT ip:").fontSize(14).fontColor(Color.Black)
                }

                GridCol({
                  span: { sm: 2, md: 4, lg: 6 }
                }) {
                  TextInput({ text: this.serverIp })
                    .onChange((value: string) => {
                      console.info("[AnyChat] MQTT ip value:" + value);
                      this.serverIp = value;
                    })
                    .placeholderColor(Color.Grey)
                    .placeholderFont({ size: 14 })
                    .caretColor(Color.Blue)
                    .height(36)
                    .enabled(this.enableInput)
                    .fontSize(14)
                    .fontColor(Color.Black)
                }

                GridCol({
                  span: { sm: 1, md: 2, lg: 3 }
                }) {
                  Button(this.btnConnectText)
                    .height(36)
                    .enabled(this.enableButton)
                    .onClick(async () => {
                      console.info("AnyChat Button click. this.isCon:" + this.isCon);
                      if (this.isCon) {
                        this.wsCon.close()
                        this.enableInput = true
                        this.enableButton = false
                      } else {
                        this.enableInput = false
                        this.enableButton = false
                        this.ip = await this.getLocalIp();
                        if (this.ip == "" || this.ip == "0.0.0.0") {
                          this.logList.push("连接服务器失败，网络未连接")
                          this.enableButton = true
                          this.enableInput = true
                        } else {

                          this.saveParam()

                          console.info("[AnyChat] serverIp:" + this.serverIp);
                          this.logList.length = 0;
                          this.logList.push("成功获取本机ip: " + this.ip)
                          // this.flushOnlineList();

                          this.setWebSocketEvent();
                          this.wsCon.connect("ws://" + this.serverIp + ":5555/online?url=" + this.pushUrl, (err: BusinessError, value: boolean) => {
                            if (!err) {
                              console.log("Connected successfully");
                            } else {
                              this.logList.push("连接服务器失败，请重试")
                              console.log("Connection failed. Err:" + JSON.stringify(err));
                              this.enableButton = true
                              this.enableInput = true
                              this.wsCon = webSocket.createWebSocket();
                            }
                          });
                          this.initPush(this.ip); //ssk test
                        }
                      }
                    })
                }
              }
            } //Text+TextInput+Button
          }.alignItems(HorizontalAlign.Start)

        }

        Column() {
          GridRow({ columns: { sm: 1, md: 7, lg: 12 } }) {
            GridCol({
              span: { sm: 1, md: 1, lg: 2 }
            }) {
              Text("日志").fontSize(20).padding(HorizontalAlign.Start)
            }
            GridCol({
              span: { sm: 1, md: 2, lg: 3 }
            }) {
              List({ space: 5, initialIndex: 0 }) {
                ForEach(this.logList, (item: string) => {
                  ListItem() {
                    Text(item).fontSize(15)
                  }
                }, (item: string) => item)
              }.height('10%').alignListItem(ListItemAlign.Start).backgroundColor(Color.Gray)
            }

            GridCol({
              span: { sm: 1, md: 1, lg: 2 }
            }) {
              Text("联系人").fontSize(20)
            }
            GridCol({
              span: { sm: 1, md: 2, lg: 3 }
            }) {
              List({ space: 5, initialIndex: 0 }) {
                ForEach(this.onlineList, (item: string) => {
                  ListItem() {
                    Text(item).fontSize(30)
                      .onClick(() => {
                        console.log("AnyChat user list click. item:" + item);
                        if (item.indexOf("(本机ip)") == -1) {
                          console.log("AnyChat user list click. this.isCalling:" + this.isCalling);
                          if (this.isCalling == false) {
                            let sendMsg: SendRequestMsg = new SendRequestMsg();
                            sendMsg.target = item;
                            console.log("AnyChat send pushUrl:" + this.pushUrl);
                            sendMsg.url = this.pushUrl;
                            this.wsCon.send(JSON.stringify(sendMsg));
                            this.logList.push("成功向" + item + "发送通话请求")

                          }
                        }
                      })
                  }
                }, (item: string) => item)
              }
              // .width(200)
              .height('10%')
              .backgroundColor(Color.Gray)
              .alignListItem(ListItemAlign.Center)
              .divider({ strokeWidth: 1, startMargin: 10, endMargin: 10, color: '#ffe9f0f0' })
            }
          }.alignItems(ItemAlign.Center)
        }

        Blank(5)

        GridRow({ columns: { sm: 2, md: 2, lg: 2 } }) {
          GridCol({
            span: { sm: 1, md: 1, lg: 1 }
          }) {
            if (this.isShow && this.showPreview) {
            // if (1) {
              XComponent({
                id: 'cameraPreview',
                type: 'surface',
                controller: this.mPreviewController
              })
                .onLoad(async () => {
                  this.isPreviewLoad = true;
                  console.info("[AnyChat] preview onLoad");
                  this.cameraHelpInit();
                })
                .onDestroy(() => {
                  console.info("[AnyChat] preview onDestroy");

                  this.handUp()
                  this.closePlayAndPush()

                  if (this.mCameraHelper != null) {
                    this.mCameraHelper.Release()
                    this.mCameraHelper = null;
                  }

                  if (this.isCon) {
                    this.wsCon.close()
                  }
                })
                .width('350px')
                .height('375px')
                .zIndex(2)
            }
          }

          GridCol({
            span: { sm: 1, md: 1, lg: 1 }
          }) {
            Column() {
              XComponent({
                id: 'xcomponent2',
                type: 'surface',
                libraryname: 'entry',
                controller: this.xcomponentController2
              })
                .width('350px')
                .height('375px')

              Image($r("app.media.handup")).onClick(() => {
                this.handUp();
                this.closePlayAndPush();
              }).height(30).width(30)
            }
          }
        }
      }
    }.height('100%').alignItems(VerticalAlign.Center)
  }

  aboutToDisappear() {
    renderNapi.DisconnectPush();
  }

  cameraHelpInit(){
    this.mCameraHelper = new CameraHelper();

    let renderId = this.mPreviewController.getXComponentSurfaceId();
    let context = getContext(this) as common.UIAbilityContext
    this.mCameraHelper.Init(context, renderId, 640, 480)
  }
  //页面显示
  onPageShow(){
    this.showPreview = true
    //打印出页面显示的提醒
    console.log('This Page is onPageShow')
    // if (this.isPreviewLoad) {
    //   this.cameraHelpInit();
    // }

    //this.loadParam();

  }
  //页面隐藏
  onPageHide(){
    this.showPreview = false;
    //打印出页面隐藏的提醒
    console.log('This Page is onPageHide')
    // if (this.isPreviewLoad && this.mCameraHelper != null) {
    //   this.mCameraHelper.Release()
    //   this.mCameraHelper = null;
    // }
  }
  //页面返回：手机按钮上的返回
  onBackPress(){
    //打印出页面返回的提醒
    console.log('This Page is onBackPress')
  }

  async loadParam(){

    this.srsUrl = await this.db.getPreference('srs_url', this.defaultSrsUrl) as string;
    this.serverIp = await this.db.getPreference('mqtt_ip', this.defaultServerIp) as string;
  }
  //aboutToAppear和aboutToDisappear也可以放在入口组件中，后者表示程序被sha死时调用的函数
  //组件即将出现时回调该接⼝
  aboutToAppear(){
    console.log('This Page is 入口aboutToAppear')

    this.loadParam();

  }

  hexStringToBytes(hexString: string): Uint8Array {
    if (hexString == null || hexString === "") {
      return new Uint8Array([]);
    }
    hexString = hexString.toUpperCase();
    let length = hexString.length / 2;
    let cardByte: Uint8Array = new Uint8Array(length);
    for (let i = 0; i < length; i++) {
      let pos = i * 2;
      cardByte[i] = (hexString.charCodeAt(pos) << 4 | hexString.charCodeAt(pos + 1));
    }
    return cardByte;
  }

  ipToBase64(ip:string){
    return ip.substr(ip.lastIndexOf(".") + 1,3); //截取最后一个点号后4个字符

    // let that = new util.Base64Helper();
    // let array = new Uint8Array(this.hexStringToBytes(ip));
    // let result = that.encodeToStringSync(array);
    //
    // return result
  }

  initPush(ip: string) {
    if(ip == '') {
      ip = "192.168.31.231"
    }
    if(this.srsUrl == '') {
      this.srsUrl = this.defaultSrsUrl;
    }

    this.pushUrl = "webrtc://" + this.srsUrl + "/live/" + this.ipToBase64(ip);
    console.log("AnyChat push url:" + this.pushUrl);
    renderNapi.initPush(this.pushUrl);
    this.mCameraHelper!!.setPushFlag(true)

    // let dstRecordFile = "/data/app/el2/100/base/com.example.metartc/haps/entry/files/record.pcm";
    let dstRecordFile = "";//this.context.filesDir+"/record.pcm";

    renderNapi.startAudioRecord(dstRecordFile);
  }

  handUp() {
    if(this.isCalling) {
      console.log("AnyChat handUp start");
      let msg: SendCloseMsg = new SendCloseMsg();
      msg.target = this.callUser;
      this.wsCon.send(JSON.stringify(msg));
      this.logList.push("挂断通话")
      console.log("AnyChat handUp end");
      this.isCalling = false;

    }
  }
  
  closePlayAndPush() {
    console.log("AnyChat closePlayAndPush start. this.isCalling:" + this.isCalling);
    if(this.mCameraHelper != null && this.mCameraHelper.getPushFlag()){
      this.mCameraHelper!!.setPushFlag(false)
      renderNapi.stopAudioRecord()
      renderNapi.deInitPush()
    }
    if(this.isPlaying){
      this.isPlaying = false;
      renderNapi.deInitPlay("xcomponent2")
    }
  }

}