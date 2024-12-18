import { DEFAULT } from '@ohos/hypium'

@Preview
@CustomDialog

export default struct MyDialog {
  controller: CustomDialogController =new CustomDialogController({builder:''})
  cancel: Function = () =>{}
  confirm: Function = () =>{}
  build() {
    Column() {

      Text('确认选择该颜色')
        .fontSize(20)
        .margin({ top: 20, bottom: 40 })
      Row() {
        Button('确认')
          .onClick(() => {
            this.controller.close()
            this.confirm()
          })
        Button('取消')
          .onClick(() => {
            this.controller.close()
            this.cancel()
          })
      }.width('75%').justifyContent(FlexAlign.SpaceAround).margin({ bottom: 20 })
    }
  }
}