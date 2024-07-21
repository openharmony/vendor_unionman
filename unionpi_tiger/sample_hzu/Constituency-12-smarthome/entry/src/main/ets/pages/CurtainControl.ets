import pwmtest from '@ohos.pwmtest';
@CustomDialog
export default struct CurtainControl {
  isonValue: Boolean//存放开关状态
  controller: CustomDialogController
  cancel: () => void//取消按钮对应事件
  confirm: (Boolean) => void//确定按钮对应事件

  //默认引脚1
  @State Pex: number = 1
  pin : number = 386

  //舵机转动角度，尾部序号为舵机代号
  @State PwmAngle1: number = 0
  @State PwmAngle2: number = 0


  build() {
    Column() {
      Row(){
        Column(){
          Text('滑动以控制窗帘')
          Slider({
            value: this.PwmAngle1,
            min: 0,
            max: 180,
            step: 1,
            style: SliderStyle.OutSet
          })
            .width('80%')
            .blockColor(Color.Gray)
            .trackColor(Color.Gray)
            .selectedColor(0x217aff)
            .showSteps(true)
            .showTips(true)
            .onChange((value: number) => {
                this.PwmAngle1 = value
                console.info('onChange pwmAngle = ' + this.PwmAngle1)
                pwmtest.setPwmStatus(this.Pex, this.PwmAngle1)
            })
        }
      }
      .margin({ top: 16 })

      Row() {
        Button('取消', { type: ButtonType.Capsule ,stateEffect: true})
          .onClick(() => {
            this.cancel()
          })
          .backgroundColor('#00000000')
          .fontColor($r('app.color.font_color'))
          .width('40%')


        Button('确定', { type: ButtonType.Capsule ,stateEffect: true})
          .onClick(() => {
            this.confirm(this.isonValue)
          })
          .backgroundColor('#00000000')
          .fontColor($r('app.color.font_color'))
          .width('40%')
      }
      .width('100%')
      .margin({ top: 16, bottom: 16, left: 32, right: 32 })
    }
    .margin({ left: 16, right: 16 })
    .padding({ top: 16, bottom: 16 })
    .backgroundImage($r('app.media.new'))
    .height('40%')
    .width('100%')
  }
}