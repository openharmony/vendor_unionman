import gpioLed from '@ohos.gpioled'
@CustomDialog
export default struct LedControl {
  isonValue: Boolean//存放开关状态
  controller: CustomDialogController
  cancel: () => void//取消按钮对应事件
  confirm: (Boolean) => void//确定按钮对应事件

   pin : number = 385

  Open(pin: number ) {
    gpioLed.setLedStatus(pin, gpioled.LED_ON)
  }

  Close(pin: number) {
    gpioLed.setLedStatus(pin, gpioled.LED_OFF)
  }

  build() {
    Column() {
      Row({ space: 30 }){
        Column(){
          Text('大厅').fontSize('20').fontColor(Color.Black).fontStyle(FontStyle.Italic)
          Toggle({type:ToggleType.Switch,isOn:false})
            .onChange((isOn)=>{
              this.isonValue = isOn
              if (isOn) {
                this.Open(389)
                console.info('灯On' )
                return
              }
              this.Close(389)
              console.info('灯Off')
            })
        }

        Column(){
          Text('客厅').fontSize('20').fontColor(Color.Black).fontStyle(FontStyle.Italic)
          Toggle({type:ToggleType.Switch,isOn:false})
            .onChange((isOn)=>{
              this.isonValue = isOn
              if (isOn) {
                this.Open(388)
                console.info('灯On' )
                return
              }
              this.Close(388)
              console.info('灯Off')
            })
        }

        Column(){
          Text('大门').fontSize('20').fontColor(Color.Black).fontStyle(FontStyle.Italic)
          Toggle({type:ToggleType.Switch,isOn:false})
            .onChange((isOn)=>{
              this.isonValue = isOn
              if (isOn) {
                this.Open(387)
                console.info('灯On' )
                return
              }
              this.Close(387)
              console.info('灯Off')
            })
        }

        Column(){
          Text('浴室').fontSize('20').fontColor(Color.Black).fontStyle(FontStyle.Italic)
          Toggle({type:ToggleType.Switch,isOn:false})
            .onChange((isOn)=>{
              this.isonValue = isOn
              if (isOn) {
                this.Open(386)
                console.info('灯On' )
                return
              }
              this.Close(386)
              console.info('灯Off')
            })
        }

        Column(){
          Text('厨房').fontSize('20').fontColor(Color.Black).fontStyle(FontStyle.Italic)
          Toggle({type:ToggleType.Switch,isOn:false})
            .onChange((isOn)=>{
              this.isonValue = isOn
              if (isOn) {
                this.Open(385)
                console.info('灯On' )
                return
              }
              this.Close(385)
              console.info('灯Off')
            })
        }

      }

      Row({space:30}) {
        Button({ type: ButtonType.Normal, stateEffect: true }) {
          Stack() {
            Image($r('app.media.led'))
              .height(120)
              .width(120)
            Text('灯全开')
              .fontSize(25)
              .fontStyle(FontStyle.Italic)
              .fontColor(Color.White)
          }
        }
        .onClick(() => {
          this.Open(389)
          this.Open(388)
          this.Open(387)
          this.Open(386)
          this.Open(385)
          this.Open(384)
          this.Open(383)
          this.Open(382)
          this.Open(381)
          this.Open(380)
        })
        .height(80)
        .width(80)
        .backgroundColor('#00000000')


        Button({ type: ButtonType.Normal, stateEffect: true }) {
          Stack() {
            Image($r('app.media.led'))
              .height(120)
              .width(120)
            Text('灯全关')
              .fontSize(25)
              .fontStyle(FontStyle.Italic)
              .fontColor(Color.White)
          }
        }
        .onClick(() => {
          this.Close(389)
          this.Close(388)
          this.Close(387)
          this.Close(386)
          this.Close(385)
          this.Close(384)
          this.Close(383)
          this.Close(382)
          this.Close(381)
          this.Close(380)
        })
        .height(80)
        .width(80)
        .backgroundColor('#00000000')
      }

      Row({ space: 30 }){

        Column(){
          Toggle({type:ToggleType.Switch,isOn:false})
            .onChange((isOn)=>{
              this.isonValue = isOn
              if (isOn) {
                this.Open(384)
                console.info('灯On' )
                return
              }
              this.Close(384)
              console.info('灯Off')
            })
          Text('阳台').fontSize('20').fontColor(Color.Black).fontStyle(FontStyle.Italic)
        }

        Column(){
          Toggle({type:ToggleType.Switch,isOn:false})
            .onChange((isOn)=>{
              this.isonValue = isOn
              if (isOn) {
                this.Open(383)
                console.info('灯On' )
                return
              }
              this.Close(383)
              console.info('灯Off')
            })
          Text('厕所').fontSize('20').fontColor(Color.Black).fontStyle(FontStyle.Italic)
        }

        Column(){
          Toggle({type:ToggleType.Switch,isOn:false})
            .onChange((isOn)=>{
              this.isonValue = isOn
              if (isOn) {
                this.Open(382)
                console.info('灯On' )
                return
              }
              this.Close(382)
              console.info('灯Off')
            })
          Text('主卧').fontSize('20').fontColor(Color.Black).fontStyle(FontStyle.Italic)
        }

        Column(){
          Toggle({type:ToggleType.Switch,isOn:false})
            .onChange((isOn)=>{
              this.isonValue = isOn
              if (isOn) {
                this.Open(381)
                console.info('灯On' )
                return
              }
              this.Close(381)
              console.info('灯Off')
            })
          Text('次卧').fontSize('20').fontColor(Color.Black).fontStyle(FontStyle.Italic)
        }

        Column(){
          Toggle({type:ToggleType.Switch,isOn:false})
            .onChange((isOn)=>{
              this.isonValue = isOn
              if (isOn) {
                this.Open(380)
                console.info('灯On' )
                return
              }
              this.Close(380)
              console.info('灯Off')
            })
          Text('杂物间').fontSize('20').fontColor(Color.Black).fontStyle(FontStyle.Italic)
        }

      }

      Row() {
        Button('取消', { type: ButtonType.Capsule ,stateEffect: true})
          .onClick(() => {
            this.cancel()
          })
          .backgroundColor($r('app.color.mainPage_normal'))
          .fontColor($r('app.color.font_color'))
          .width('40%')

        Blank()

        Button('确定', { type: ButtonType.Capsule ,stateEffect: true})
          .onClick(() => {
            this.confirm(this.isonValue)
          })
          .backgroundColor($r('app.color.mainPage_normal'))
          .fontColor($r('app.color.font_color'))
          .width('40%')
      }
      .width('100%')
      .margin({ top:20, bottom: 16, left: 32, right: 32 })

    }
    .backgroundImage($r('app.media.brand'))
    .height('80%')
    .width('100%')
    .margin({ left: 16, right: 16 })
    .padding({ top: 16, bottom: 16 })
  }
}