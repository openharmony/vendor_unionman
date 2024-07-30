
import router from '@ohos.router'
import { VidePlayer } from '../view/VideoPlayer';
/**
 * 第二页面
 */
@Entry
@Component
struct videoSecondPage{
  private source: string = (router.getParams() as Record<string, Object>).source as string;
  private startIconResource: Resource = $r('app.media.ic_public_play')
  private backIconResource: Resource = $r('app.media.ic_back')
  @Provide isPlay: boolean = false
  @Provide isOpacity: boolean = false
  controller: VideoController = new VideoController();
  @Provide isLoading: boolean = false
  @Provide progressVal: number = 0
  @Provide flag: boolean = false

  aboutToAppear() {
    this.source
  }

  onPageHide() {
    this.controller.pause()
  }

  build() {
    Column() {
      Row() {
        Image(this.backIconResource)
          .width(24)
          .height(24)
          .margin(24)
          .onClick(() => {
            router.back()
          })
        Text('视频')
          .fontColor(Color.White)
          .fontSize(24)
          .fontWeight(500)
          .margin({ left: 12})
      }
      .width('100%')
      .margin({
        left:12,
        top:12
      })
      .justifyContent(FlexAlign.Start)

      Stack() {
        if(!this.isPlay && !this.isLoading) {
          Image(this.startIconResource)
            .width(50)
            .height(50)
            .zIndex(2)
        }
        if(this.isLoading) {
          Progress({
            value: 0,
            total: 100,
            type: ProgressType.ScaleRing
          })
            .color(Color.Grey)
            .value(this.progressVal)
            .width(80)
            .style({
              strokeWidth: 15,
              scaleCount: 15,
              scaleWidth: 5
            })
            .zIndex(1)
        }
        VidePlayer({
          source: this.source,
          controller: this.controller
        })
          .zIndex(0)
      }
    }
    .height('100%')
    .backgroundColor(Color.Black)
  }
}