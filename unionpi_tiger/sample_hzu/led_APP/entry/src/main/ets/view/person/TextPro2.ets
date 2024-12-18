/*
 * 自定义输入框
 * */
import Constant from '../../common/Constant'

@Component
export default struct TextPro {
  private inputImage?: Resource;  // 输入框图片
  private email?: string;

  build() {
    Row() {
      Image(this.inputImage !== undefined ? this.inputImage : '')
        .width(Constant.INPUT_IMAGE_SIZE)
        .height(Constant.INPUT_IMAGE_SIZE)
        .margin({ left: 12 })
      Text(this.email)
        .fontSize(16)
        .padding({ left: 16 })
        .backgroundColor(Color.White)
        .fontWeight(FontWeight.Normal)
        .fontStyle(FontStyle.Normal)
        .fontColor(Color.Black)
        .margin({ right: 32 })
        .layoutWeight(1)
        .height(48)
    }
    .margin({ top: 24 })
    .borderRadius(24)
    .backgroundColor(Color.White)
    .width('93.3%')
    .height(64)
  }
}