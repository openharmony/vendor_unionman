@CustomDialog
export default struct OkCancelInputDialog {
  inputValue: string//存放输入内容的变量
  controller: CustomDialogController
  cancel: () => void//取消按钮对应事件
  confirm: (string) => void//确定按钮对应事件


  build() {
    Column() {
      TextInput({ placeholder: '请输入内容' }) //输入框
        .onChange((str) => {
          this.inputValue=str
        })
        .margin({ left: 16, right: 16 })

      Row() {
        Button('取消', { type: ButtonType.Capsule ,stateEffect: true})
          .onClick(() => {
            this.cancel()
          })
          .backgroundColor(Color.White)
          .fontColor(Color.Blue)
          .width('45%')

        Blank()

        Button('确定', { type: ButtonType.Capsule ,stateEffect: true})
          .onClick(() => {
            this.confirm(this.inputValue)
          })
          .backgroundColor(Color.Blue)
          .fontColor(Color.White)
          .width('45%')
      }
      .width('100%')
      .margin({ top: 16, bottom: 16, left: 32, right: 32 })
    }
    .margin({ left: 16, right: 16 })
    .padding({ top: 24, bottom: 24 })
  }
}