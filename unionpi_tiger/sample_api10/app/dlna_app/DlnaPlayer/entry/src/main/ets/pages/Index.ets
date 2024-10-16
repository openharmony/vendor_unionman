
import entry from "libentry.so"
import AvPlayManage from '../videomanager/AvPlayManager';
import media from '@ohos.multimedia.media';
import { VideoOperate } from '../components/VideoOperate';
import wifiManager from '@ohos.wifiManager';
import ethernet from '@ohos.net.ethernet';

const PANEL_TIME_OUT = 8000;

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';
  xcomponent: XComponentController = new XComponentController();
  private data:string = "";
  surfaceid: string = "";
  @State playFlag: boolean = true;
  @State avPlayManage: AvPlayManage = new AvPlayManage();
  @State durationTime: number = 0; // 视频总时长
  @State currentTime: number = 0; // 视频当前时间
  @State seekPos: string = "";
  @State isScreenClick: boolean = false;
  private panelTimerId: number = 0;

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

  setPanelTimer(): void {
    this.panelTimerId = setTimeout(() => {
      this.isScreenClick = false;
    }, PANEL_TIME_OUT)
  }

  build() {
    Stack() {
        Column() {
          XComponent({
            "id": "dlnaplayer",
            "type": "surface",
            "libraryname": "libentry",
            "controller": this.xcomponent
          }).width("100%")
            .height("100%")
            .onLoad(async () => {
              this.surfaceid = this.xcomponent.getXComponentSurfaceId();
              let ip: string =  await this.getLocalIp();
              console.log("tt==> ip:" + ip);
              entry.startDmr(ip, (data: string) => {
                console.log("this is callback. data:" + data);
                if(data == "PLAY") {
                  this.avPlayManage.videoPlay();
                } else if (data == "PAUSE") {
                  this.avPlayManage.videoPause();
                } else if (data == "STOP") {
                  this.avPlayManage.videoReset();
                } else if (data.startsWith("http")) {
                  this.currentTime = 0;
                  this.durationTime = 0;
                  this.avPlayManage.initPlayer(this.surfaceid, data, (avPlayer: media.AVPlayer) => {
                    this.durationTime = this.avPlayManage.getDurationTime();
                    console.info("get durationTime:" + this.durationTime);
                    setInterval(() => {
                      this.currentTime = this.avPlayManage.getCurrentTime();
                      entry.notifyPositionChanged(this.currentTime, this.durationTime);
                    }, 500)
                  })
                } else {
                  this.seekPos = data;
                  console.info("tt==> seek POSITION:" + this.seekPos);
                  this.avPlayManage.videoSeek(Number(this.seekPos)*1000);
                }
              })
            })
        }.align(Alignment.TopStart)
        .margin({ top: $r('app.float.size_80') })
        .id('Video')
        .justifyContent(FlexAlign.Center)

      Column() {
        Blank()

        Column() {
          VideoOperate({
            flag: $playFlag,
            avPlayManage: $avPlayManage,
            currentTime: $currentTime,
            durationTime: $durationTime
          })
            .width('100%')
        }
        .backgroundColor("#8f555888")
        .justifyContent(FlexAlign.End)
      }
      .width('100%')
      .height('100%')
      .visibility(this.isScreenClick ? Visibility.Visible : Visibility.Hidden)
    }.onClick(() => {
      this.isScreenClick = !this.isScreenClick;
      if (this.isScreenClick) {
        this.setPanelTimer();
      }
    })
  }
}