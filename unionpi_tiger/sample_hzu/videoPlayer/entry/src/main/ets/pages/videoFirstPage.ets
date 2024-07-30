
import { VideoModule } from '../view/IndexModule'
import { SwiperVideo } from '../view/IndexSwiper'
/**
 * 首页
 */
@Entry
@Component
struct videoFirstPage {
  @State LIST:any[] = [
    {LIST:'最近播放'},
    {LIST:'为你推荐'}
  ]

  build() {
    Column({ space: 24}) {
      SwiperVideo()
      List() {
        ForEach(this.LIST, (item: string) => {
          ListItem() {
            VideoModule()
              .margin({ top: 12})
          }
        }, (item: string) => JSON.stringify(item))
      }
      .listDirection(Axis.Vertical)
      .margin({ top: 18})
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5')
  }
}