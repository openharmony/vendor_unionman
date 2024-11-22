@CustomDialog
export struct CustomDialogExample {
  // 双向绑定传值
  @Prop title: string
  @Prop inputValue: string
  // 弹窗控制器，控制打开/关闭，必须传入，且名称必须为：controller
  controller: CustomDialogController
  // 弹窗中的按钮事件
  cancel: () => void=()=>{}
  confirm: () => void=()=>{}

  private is_timeout:boolean = false
  private timer:number = 0
  aboutToAppear() {
    this.timer = setTimeout(()=>{
      this.controller.close()
      this.cancel()
      this.is_timeout = true
    }, 30*1000)
  }
  aboutToDisappear(){
    clearTimeout(this.timer)
  }

  // 弹窗中的内容描述
  build() {
    Column() {
      Text(this.title || "是否修改文本框内容？")
        .fontSize(16)
        .margin(24)
        .textAlign(TextAlign.Center)
        .width("100%")
      Text(this.inputValue)
        .height(60)
        .textAlign(TextAlign.Center)
        .width('90%')
      Flex({ justifyContent: FlexAlign.SpaceAround }) {
        Button('取消')
          .onClick(() => {
            this.controller.close()
            this.cancel()
          }).backgroundColor(0xffffff).fontColor(Color.Black)
        Button('确定')
          .onClick(() => {
            this.controller.close()
            this.confirm()
          }).backgroundColor(0xffffff).fontColor(Color.Red)
      }.margin({ bottom: 10 })
    }
  }
}