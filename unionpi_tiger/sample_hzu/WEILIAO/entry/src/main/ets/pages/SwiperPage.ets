/**
 * @author: Mr.Dai
 */
import router from '@ohos.router';
import CommonConstants from '../common/constants/CommonConstants';

@Entry
@Component
struct SwiperPage {
  @State images: ResourceStr[] = []
  @State index: number = 0

  aboutToAppear() {
    let params: object = router.getParams()
    this.images = params['images']
    this.index = params['index']
  }

  build() {
    Column() {
      Swiper() {
        ForEach(this.images, (item: ResourceStr) => {
          Image(item)
            .width(CommonConstants.FULL_PARENT)
            .objectFit(ImageFit.Auto)
        })
      }
      .height(CommonConstants.FULL_PARENT)
      .index(this.index)
      .indicatorStyle({
        color: $r('app.color.swiperPage_color')
      })
    }
    .width(CommonConstants.FULL_PARENT)
    .height(CommonConstants.FULL_PARENT)
    .backgroundColor(Color.Black)
    .justifyContent(FlexAlign.Center)
    .onClick(() => {
      // 点击返回至上一页面
      router.back()
    })
  }
}

