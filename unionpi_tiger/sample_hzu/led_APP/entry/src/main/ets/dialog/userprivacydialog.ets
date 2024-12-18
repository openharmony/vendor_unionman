import { DEFAULT } from '@ohos/hypium'

@Preview
@CustomDialog
export  default   struct  userprivacydialog{
  controller: CustomDialogController =new CustomDialogController({builder:''})
  cancel: Function = () =>{}
  confirm: Function = () =>{}
  build() {
  Column({space: 10}){
Text("感谢使用")
  .fontSize(20)
  .fontWeight(FontWeight.Bolder)
  .margin({top: 8, bottom: 5})
   Text(){
     Span("在使用我们的服务之前，请知悉并同意以下条款：")


   }

    Text(){
      Span("我们尊重并保护您的隐私。")
      Span("我们可能会收集必要的数据以提供和改进服务。")
       Span("您同意我们使用cookies以优化您的体验。")
      Span("我们不会将您的信息出售给第三方。")
    }
    Text(){
      Span("点击“同意”继续使用应用，即表示您接受我们的隐私政策和服务条款。")

    }
   Button('同意')
     .fontColor(Color.White)
     .backgroundColor("#ff06ae27")
     .width(150)
     .onClick(() => {
       this.confirm()
       this.controller.close()
     })
    Button('不同意')
      .fontColor("#d09d9d9d")
      .backgroundColor("#412ef558")
      .width(150)
      .onClick(() => {
        this.confirm()
        this.controller.close()
      })
  }
.width('88%')
.height('75%')
}
}