//场景选择页面
import router from '@ohos.router';
@Entry
@Component
struct SliderExample {
  build() {
    Column() {
    }
    .backgroundImage($r('app.media.secondbackground3'))
    .backgroundImageSize(ImageSize.Cover)
    .width('100%')
    .height('100%')
    .backgroundColor(Color.Orange)
    .justifyContent(FlexAlign.SpaceEvenly); // 主轴对齐
  }
}