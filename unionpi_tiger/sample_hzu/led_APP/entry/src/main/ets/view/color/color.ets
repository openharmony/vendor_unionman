import color from '../../dialog/color';

@Preview
@Component
export default struct colorlight {
  private arr: string[] = ['#FF575A5A',
    '#00F465',
    '#001234',
    '#00FFFF',
    '#FF0022FF',
    '#FF292D2D',
  ]
  selectedColor: string = '';
  private brightness1: number = 50; // 初始化光强度
  @Prop   delay: number = 50; // 初始化延迟时间（毫秒）

  controller: CustomDialogController = new CustomDialogController({


    builder:

    color({
      cancel:()=>{},
      confirm:()=>{}

    })
  })


  build() {

    Column({ space: 30 }) {
      // 标题部分
      Text("灯光控制面板")
        .fontSize(36)
        .fontWeight('bold')
        .fontColor('#444')
        .margin({ top: 20, bottom: 20 })


      // 延迟设置部分
      Column({ space: 15 }) {
        Text("调整延迟时间 (毫秒)")
          .fontSize(24)
          .fontColor('#ff291f1f')
          .margin({ bottom: 10 })
        Text(this.delay.toString().concat(" ms"))
          .fontSize(24)
          .fontColor('#ff965e5e')
          .margin({ bottom: 10 })
          .id('delayText')

        // 延迟时间滑动条
        Slider({
          value: this.delay,
          min: 10,
          max: 1000,
          step: 1,

        }).onChange((value)=>{
          this.delay=Math.round(value);
        })
          .width('80%')
          .height(40)
          .margin({ left: 19, right: 19 })
          .backgroundColor('#e0e0e0')
          .borderRadius(20)

      }.alignItems(HorizontalAlign.Center)

      // 颜色选择部分
      Text("灯颜色切换")
        .fontSize(28)
        .fontWeight('bold')
        .fontColor('#333')
        .margin({ top: 30, bottom: 20 })



      Column({ space: 15 }) {
        Grid() {
          ForEach(this.arr, (item: string) => {

            GridItem() {

              Button()
                .backgroundColor(item)
                .width('40%')
                .height(50)
                .borderRadius(20)

                .margin({ bottom: 5 })
            }
            .onClick(() => {
              // 处理点击事件

              this.controller.open()

            })
          })
        }
      }.alignItems(HorizontalAlign.Center)
      .margin({ left: 19, right: 19 })

    }
    .width('100%')
    .height('100%')
    .padding(20)
    .backgroundColor('#f5f5f5')
    .borderRadius(15)


  }
}
