import pwmtest from '@ohos.pwmtest';
import appRecovery from '@ohos.app.ability.appRecovery';
@CustomDialog
export default struct DoorControl {
  isonValue: Boolean//存放开关状态
  controller: CustomDialogController
  cancel: () => void//取消按钮对应事件
  confirm: (Boolean) => void//确定按钮对应事件

  //舵机转动角度，尾部序号为舵机代号
  @State PwmAngle1: number = 0
  @State PwmAngle2: number = 0

  //默认引脚1
  @State Pex: number = 1
  //默认开门为90°打开,-90°为关闭
  openvalue: number = 90
  closevalue: number = 0


  build() {
    Column() {

      Row({ space : 30 }){

        Column(){
          Text('大门').fontSize('20').fontColor($r( 'app.color.mainPage_selected' )).fontStyle(FontStyle.Italic)
          Toggle({type:ToggleType.Switch,isOn:false})
            .onChange((isOn)=>{
              this.isonValue = isOn
              if (isOn) {
                this.PwmAngle1 = this.openvalue
                pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
                console.info('门On' )
                return
              }
                this.PwmAngle1 = this.closevalue
                pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
                console.info('门Off')
            })
        }

        Column(){
          Text('厕所').fontSize('20').fontColor($r( 'app.color.mainPage_selected' )).fontStyle(FontStyle.Italic)
          Toggle({type:ToggleType.Switch,isOn:false})
            .onChange((isOn)=>{
              this.isonValue = isOn
              if (isOn) {
                this.PwmAngle1 = this.openvalue
                pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
                console.info('门On' )
                return
              }
              this.PwmAngle1 = this.closevalue
              pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
              console.info('门Off')
            })
        }

        Column(){
          Text('浴室').fontSize('20').fontColor($r( 'app.color.mainPage_selected' )).fontStyle(FontStyle.Italic)
          Toggle({type:ToggleType.Switch,isOn:false})
            .onChange((isOn)=>{
              this.isonValue = isOn
              if (isOn) {
                this.PwmAngle1 = this.openvalue
                pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
                console.info('门On' )
                return
              }
              this.PwmAngle1 = this.closevalue
              pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
              console.info('门Off')
            })
        }

        Column(){
          Text('厨房').fontSize('20').fontColor($r( 'app.color.mainPage_selected' )).fontStyle(FontStyle.Italic)
          Toggle({type:ToggleType.Switch,isOn:false})
            .onChange((isOn)=>{
              this.isonValue = isOn
              if (isOn) {
                this.PwmAngle1 = this.openvalue
                pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
                console.info('门On' )
                return
              }
              this.PwmAngle1 = this.closevalue
              pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
              console.info('门Off')
            })
        }

        Column(){
          Text('阳台').fontSize('20').fontColor($r( 'app.color.mainPage_selected' )).fontStyle(FontStyle.Italic)
          Toggle({type:ToggleType.Switch,isOn:false})
            .onChange((isOn)=>{
              this.isonValue = isOn
              if (isOn) {
                this.PwmAngle1 = this.openvalue
                pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
                console.info('门On' )
                return
              }
              this.PwmAngle1 = this.closevalue
              pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
              console.info('门Off')
            })
        }

      }

      Row({space:30}){
        Button({ type: ButtonType.Normal, stateEffect: true }) {
          Stack() {
            Image($r('app.media.curtain'))
              .height(80)
              .width(80)
            Text('门全开')
              .fontSize(25)
              .fontStyle(FontStyle.Italic)
              .fontColor(Color.White)
          }
        }
          .onClick(()=>{
            this.PwmAngle1 = this.openvalue
            pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
            console.info('门On' )
            return
          })
        .height(80)
        .width(80)
        .backgroundColor('#00000000')


        Button({ type: ButtonType.Normal, stateEffect: true }) {
          Stack() {
            Image($r('app.media.curtain'))
              .height(80)
              .width(80)
            Text('门全关')
              .fontSize(25)
              .fontStyle(FontStyle.Italic)
              .fontColor(Color.White)
          }
        }
          .onClick(()=>{
            this.PwmAngle1 = this.closevalue
            pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
            console.info('门Off')
          })
        .height(80)
        .width(80)
        .backgroundColor('#00000000')
      }
      Row({ space : 30 }){

        Column(){
          Toggle({type:ToggleType.Switch,isOn:false})
            .onChange((isOn)=>{
              this.isonValue = isOn
              if (isOn) {
                this.PwmAngle1 = this.openvalue
                pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
                console.info('门On' )
                return
              }
              this.PwmAngle1 = this.closevalue
              pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
              console.info('门Off')
            })
          Text('主卧').fontSize('20').fontColor($r( 'app.color.mainPage_selected' )).fontStyle(FontStyle.Italic)
        }

        Column(){
          Toggle({type:ToggleType.Switch,isOn:false})
            .onChange((isOn)=>{
              this.isonValue = isOn
              if (isOn) {
                this.PwmAngle1 = this.openvalue
                pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
                console.info('门On' )
                return
              }
              this.PwmAngle1 = this.closevalue
              pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
              console.info('门Off')
            })
          Text('次卧').fontSize('20').fontColor($r( 'app.color.mainPage_selected' )).fontStyle(FontStyle.Italic)
        }

        Column(){
          Toggle({type:ToggleType.Switch,isOn:false})
            .onChange((isOn)=>{
              this.isonValue = isOn
              if (isOn) {
                this.PwmAngle1 = this.openvalue
                pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
                console.info('门On' )
                return
              }
              this.PwmAngle1 = this.closevalue
              pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
              console.info('门Off')
            })
          Text('杂物间').fontSize('20').fontColor($r( 'app.color.mainPage_selected' )).fontStyle(FontStyle.Italic)
        }

        Column(){
          Toggle({type:ToggleType.Switch,isOn:false})
            .onChange((isOn)=>{
              this.isonValue = isOn
              if (isOn) {
                this.PwmAngle1 = this.openvalue
                pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
                console.info('门On' )
                return
              }
              this.PwmAngle1 = this.closevalue
              pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
              console.info('门Off')
            })
          Text('浴室').fontSize('20').fontColor($r( 'app.color.mainPage_selected' )).fontStyle(FontStyle.Italic)
        }

        Column(){
          Toggle({type:ToggleType.Switch,isOn:false})
            .onChange((isOn)=>{
              this.isonValue = isOn
              if (isOn) {
                this.PwmAngle1 = this.openvalue
                pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
                console.info('门On' )
                return
              }
              this.PwmAngle1 = this.closevalue
              pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
              console.info('门Off')
            })
          Text('客厅').fontSize('20').fontColor($r( 'app.color.mainPage_selected' )).fontStyle(FontStyle.Italic)
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
      .margin({ top: 16, bottom: 16, left: 32, right: 32 })
    }
    .backgroundImage($r('app.media.backg'))
    .height('80%')
    .width('100%')
    .margin({ left: 16, right: 16 })
    .padding({ top: 24, bottom: 24 })
  }
}