
import { ParamItem } from '../viewmodel/ParamItem'
import { SwiperVideoItem } from '../viewmodel/SwiperVideoItem'
import { SWIPER_VIDEOS } from '../viewmodel/VideoData'
/**
 * 图片转盘组件
 */
@Component
export struct SwiperVideo {
  build() {
    Column() {
      Swiper() {
        ForEach(SWIPER_VIDEOS, (item: SwiperVideoItem) => {
          SwiperItem({ imageSrc: item.image, source: $rawfile('videoTest.mp4')})
        }, (item: SwiperVideoItem) => JSON.stringify(item))
      }
      .autoPlay(true)
    }
    .height('24.1%')
    .width('93.3%')
    .backgroundColor('#FFFFFF')
    .margin({
      left: '3.3%',
      right: '3.3%',
      top: 24
    })
  }
}

@Component
struct SwiperItem {
  private imageSrc: Resource = $r('app.string.empty')
  private source: Resource = $r('app.string.empty')
  private paramItem: ParamItem = new ParamItem()

  aboutToAppear() {
    this.paramItem.source = this.source
  }

  build() {
    Navigator({ target: 'pages/videoSecondPage', type: NavigationType.Push }) {
      Image(this.imageSrc)
        .borderRadius(12)
    }
    .params(this.paramItem)
  }
}