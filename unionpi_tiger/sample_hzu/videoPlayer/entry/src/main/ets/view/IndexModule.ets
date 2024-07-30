
import router from '@ohos.router';
import { HorizontalVideoItem } from '../viewmodel/HorizontalVideoItem';
import { HORIZONTAL_VIDEOS } from '../viewmodel/VideoData';
/**
 *索引模块
 */
@Component
export struct VideoModule {
  private moduleName: string = '';

  build() {
    Column() {
      Flex({ direction: FlexDirection.Row}) {
        Text(this.moduleName)
          .fontSize(18)
          .fontWeight(FontWeight.Bolder)
      }
      .margin({
        left: '3.3%',
        bottom: 12,
        top: 12
      })

      List({ space: 12}) {
        ForEach(HORIZONTAL_VIDEOS, (item: HorizontalVideoItem) => {
          ListItem(){
            HorizontalItem({
              imageSrc: item.image,
              source: '网络视频地址*******',
              videoName: item.name
            })
          }
        },(item: HorizontalVideoItem) => JSON.stringify(item))
      }
      .listDirection(Axis.Horizontal)
    }
    .margin({
      left: '3.3%',
      right: '3.3%'
    })
    .width('93.3%')
    .justifyContent(FlexAlign.Center)
    .borderRadius(24)
    .borderStyle(BorderStyle.Solid)
    .borderColor(Color.White)
    .backgroundColor('#FFFFFF')
  }
}

@Component
struct HorizontalItem {
  private imageSrc: Resource = $r('app.string.empty');
  private source: string = '';
  private videoName: string = ''

  build() {
    Column() {
      Image(this.imageSrc)
        .width(132)
        .height(94)
        .onClick(() => {
          router.pushUrl({
            url: 'pages/videoSecondPage',
            params: { source: this.source}
            });
        })
      Text(this.videoName)
        .margin({
          top: 12,
          bottom: 12
        })
        .textAlign(TextAlign.Center)
        .fontSize(16)
    }
    .justifyContent(FlexAlign.Center)
  }
}