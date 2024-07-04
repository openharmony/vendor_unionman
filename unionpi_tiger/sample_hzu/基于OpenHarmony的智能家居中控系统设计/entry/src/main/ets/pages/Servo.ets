import pwmtest from '@ohos.pwmtest';
import promptAction from '@ohos.promptAction';
@Entry
@Component
struct MarqueeExample {
  // 开关标记位
  @State start: boolean = true
  // 舵机的角度
  @State pwmAngle: number = 0
  // 舵机的引脚参数,默认是引脚1
  @State pex: number = 1
  // 开关按钮显示的文本,ON表示开
  @State power: string = 'ON'
  // 初始选择的舵机引脚
  @State mid: number = 1
  // 舵机的引脚UI显示 PWM_1代表pwm引脚1,PWM_2代表pwm引脚2
  private pwmPex: string[] = ['PWM_1', 'PWM_2']

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {

      Row() {
        Column() {
          Row() {
            // 舵机开关：默认为开
            Button(this.power, { type: ButtonType.Capsule, stateEffect: true })
              .backgroundColor(Color.Red)
              //.margin({ top: 10, right: 20, right:60, bottom: 20 })
              .width(180)
              .height(150)
              .onClick(() => {
                // 开关标记为进行判断
                if (this.start) {
                  try {
                    promptAction.showToast({
                      message: '关闭电源', // 显示文本
                      duration: 2000
                    })
                  } catch (error) {
                    console.error(`showToast args error code is ${error.code}, message is ${error.message}`);
                  }
                  ;
                  this.power = 'OFF'
                } else {
                  try {
                    promptAction.showToast({
                      message: '成功启动', // 显示文本
                      duration: 2000
                    })
                  } catch (error) {
                    console.error(`showToast args error code is ${error.code}, message is ${error.message}`);
                  }
                  ;
                  this.power = 'ON'
                }
                // 开关标记位取反
                this.start = !this.start
                console.info('power: ' + this.power + ' ,start:   ' + this.start)
              })
            // 确定选择引脚

          }//.height('30%')
          // 选择引脚：PWM_1、PWM_2（默认为PWM_1）
          Row(){
            Button('大门')
              .width(180)
              .height(150)
              .margin({ right: 20, left: 60 })
              .onClick(() => {
                this.pex = 1
                try {
                  promptAction.showToast({
                    message: '控制大门', // 显示文本
                    duration: 2000
                  })
                } catch (error) {
                  console.error(`showToast args error code is ${error.code}, message is ${error.message}`);
                }
                ;
              })
           Blank()
            Button('窗户')
              .width(180)
              .height(150)
              .margin({ right: 20 })
              .onClick(() => {
                this.pex = 2
                try {
                  promptAction.showToast({
                    message: '控制窗户', // 显示文本
                    duration: 2000
                  })
                } catch (error) {
                  console.error(`showToast args error code is ${error.code}, message is ${error.message}`);
                }
                ;
              })

          }.margin({ top: 10})


          // 按钮功能提示

        }.width('50%')

        Column() {
          Row() {
            // 根据设置的角度，模拟舵机转动角度
            Button('打开', { type: ButtonType.Capsule, stateEffect: true })
              .backgroundColor(Color.Red)
              .margin({ top: 10, right: 20, left: 60, bottom: 20 })
              .width(180)
              .height(150)
              .onClick(() => {
                if (this.start) {
                  this.pwmAngle = 75
                  console.info('onChange pwmAngle = ' + this.pwmAngle)
                  pwmtest.setPwmStatus(this.pex, this.pwmAngle)
                }

              })
            Button('关闭', { type: ButtonType.Capsule, stateEffect: true })
              .backgroundColor(Color.Red)
              .margin({ top: 10, right: 20,  bottom: 20 })
              .width(180)
              .height(150)
              .onClick(() => {
                if (this.start) {
                  this.pwmAngle = -75
                  console.info('onChange pwmAngle = ' + this.pwmAngle)
                  pwmtest.setPwmStatus(this.pex, this.pwmAngle)
                }

              })
          }
          Row() {
            // 设置舵机的转动角度
            Slider({
              value: this.pwmAngle,
              min: 0,
              max: 180,
              step: 1,
              style: SliderStyle.OutSet
            })
              .width('60%')
              .blockColor(Color.Gray)
              .trackColor(Color.Gray)
              .selectedColor(0x317aff)
              .showSteps(true)
              .showTips(true)
              .onChange((value: number) => {
                // 根据开关的标识位，来选择是否转动舵机
                if (this.start) {
                  this.pwmAngle = value
                  console.info('onChange pwmAngle = ' + this.pwmAngle)
                  pwmtest.setPwmStatus(this.pex, this.pwmAngle)
                }
              })
          }
        }.width('50%')
      }.height('70%')


    }.backgroundImage($r('app.media.bj1')).backgroundImageSize(ImageSize.Cover)
  }
}
