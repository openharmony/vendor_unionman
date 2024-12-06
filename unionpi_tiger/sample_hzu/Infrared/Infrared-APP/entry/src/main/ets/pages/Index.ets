import mynapiinfrared from '@ohos.mynapiinfrared';

// 创建一个组件来承载按钮和显示接收信号的区域
@Entry
@Component
struct InfraredControl {
  // 用于存储接收到的红外信号
  @State receivedSignal: number = 0;

  build() {
    Column(){
      Column({space:20}) {
        Button('点我发射红外线')
          .backgroundColor(Color.Black)
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .width(200)
          .height(100)
          .border({color:Color.Red,width:2,style:BorderStyle.Solid})
          .shadow({
            radius:1000,
            color: 0x990000,
          })
          .onClick(
            ()=>{
              //删掉下面代码
              // if(this.receivedSignal==1) {
              //   this.receivedSignal=0
              // }else{
              //   this.receivedSignal=1
              // }
              //调用发送红外信号的函数，这里发送高电平信号示例（可根据实际需求修改）
              mynapiinfrared.sendInfraredSignalWithPromise(1)
                .then(():Promise<number> => {
                  // 发送成功后尝试接收红外信号
                  return mynapiinfrared.receiveInfraredSignalWithPromise();
                })
                .then((signal:number) => {
                  // 更新接收到的信号状态
                  this.receivedSignal = signal;
                });
            }
          )

        // 显示接收到的红外信号
        Row() {
          Text(`接收到的红外信号: `).fontSize(20).fontColor(Color.White)
          Text(`${this.receivedSignal}`).fontColor(Color.Red).fontSize(30)
        }
      }
      .justifyContent(FlexAlign.Center)
      .width('100%')
      .height('80%')
      .backgroundColor(Color.Transparent)
    }.backgroundColor(Color.Black).width('100%').height('100%')
  }
}
@Component
struct dashboard {
  @Prop receivedSignal:number
  build() {
    Row() {
      Column({space:20}) {
        Row().width(2).height(90).backgroundColor(Color.White).margin({ bottom: -20 })
        Row().width(180).backgroundColor(Color.White).height(2).margin({ bottom: -120 })
        Row() {
          Column().width(90).backgroundColor(Color.White).height(2).rotate({ centerX: 90, angle: 45 })
          Column().width(90).backgroundColor(Color.White).height(2).rotate({ centerX: 0, angle: -45 })
        }.width('100%').justifyContent(FlexAlign.Center).margin({ bottom: -200 })

        Row()
          .backgroundColor(Color.Transparent)
          .height(80)
          .width(160)
          .borderRadius({ topLeft: 80, topRight: 80 })
          .margin({ bottom: -70 })
          .border({ style: BorderStyle.Dotted, width: 1, color: 0xffffff })
        Row()
          .backgroundColor(Color.Transparent)
          .height(50)
          .width(100)
          .borderRadius({ topLeft: 50, topRight: 50 })
          .margin({ bottom: -20 })
          .border({ style: BorderStyle.Dotted, width: 1, color: 0xffffff })
        Row() {
          Column()
            .width(100)
            .height(3)
            .backgroundColor(Color.Red)
            .margin({ top: -2 })
            .rotate({
              angle: this.receivedSignal * 180,
              centerX: 100
            })
            .animation({ duration: 1000, curve: Curve.EaseOut })
          Column().width(100).height(1)
        }.width('100%')
        .justifyContent(FlexAlign.Center)
      }
      .justifyContent(FlexAlign.Center)
      .width('100%')
      .backgroundColor(Color.Black)
    }
  }
}

