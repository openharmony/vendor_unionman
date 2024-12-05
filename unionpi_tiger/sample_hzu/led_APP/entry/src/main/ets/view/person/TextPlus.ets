/*
 * 自定义选择框
 * */
import Constant from '../../common/Constant'

@Component
export default struct TextPro {
  private inputImage?: Resource; // 输入框图片
  private title?: string; // 标题
  @Prop private content: string;
  onItemClick = () => {
  };

  build() {
    Row() {
      Image(this.inputImage !== undefined ? this.inputImage : '')
        .width(Constant.INPUT_IMAGE_SIZE)
        .height(Constant.INPUT_IMAGE_SIZE)
        .margin({ left: 12 })

      Text(this.title)
        .fontSize(16)
        .margin({ left: 12 })
        .height('100%')

      Text(this.content)
        .fontSize(16)
        .textAlign(TextAlign.End)
        .textOverflow({ overflow: TextOverflow.Ellipsis })
        .maxLines(1)
        .margin({
          left: 16,
          right: 7
        })
        .layoutWeight(1)
        .width('100%')

      Image($r('app.media.arrow'))
        .width(19)
        .height(24)
        .margin({ right: 14 })
    }
    .margin({ top: 24 })
    .borderRadius(24)
    .backgroundColor(Color.White)
    .width('93.3%')
    .height(64)
    .onClick(this.onItemClick)
  }
}