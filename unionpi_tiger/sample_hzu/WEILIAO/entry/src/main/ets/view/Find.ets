/**
 * @author: Mr.Hong
 */
import router from '@ohos.router'
import CommonConstants from '../common/constants/CommonConstants'

@Component
export default struct Find {
  build() {
    Column({ space: 5 }) {
      // 发现
      Row() {
        Stack() {
          Text(CommonConstants.FIND_TITLE)
            .width(CommonConstants.FULL_PARENT)
            .lineHeight($r('app.float.findTitleLineHeight'))
            .fontSize($r('app.float.findTitleFontSize'))
            .fontWeight(FontWeight.Bold)
            .textAlign(TextAlign.Center)
          Row({ space: 20 }) {
            Image($rawfile('images/search.png'))
              .width($r('app.float.findImageLength')).height($r('app.float.findImageLength'))
            Image($rawfile('images/add.png'))
              .width($r('app.float.findImageLength')).height($r('app.float.findImageLength'))
          }.width(CommonConstants.FULL_PARENT).justifyContent(FlexAlign.End)
        }
      }
      .width(CommonConstants.FULL_PARENT)
      .backgroundColor($r('app.color.findTitleBackground_color'))
      .padding($r('app.float.findTitlePadding'))

      // 朋友圈
      Row({ space: 5 }) {

        Image($rawfile('images/circle.png'))
          .width($r('app.float.findImageLength'))
          .height($r('app.float.findImageLength'))
          .padding($r('app.float.friendCircleImagePadding'))
          .margin({ left: $r('app.float.friendCircleImageMargin') })

        Text($r('app.string.FriendCircle'))
          .fontWeight(FontWeight.Regular)
          .fontSize($r('app.float.friendCircleTextFonSize'))
          .fontColor($r('app.color.friendCircleText_color'))
          .margin({ left: $r('app.float.friendCircleTextMargin') })


        Row()
          .width($r('app.float.friendCircleRedDot_length'))
          .height($r('app.float.friendCircleRedDot_length'))
          .borderRadius($r('app.float.friendCircleRedDot_borderRadius'))
          .backgroundColor($r('app.color.friendCircleRedDotBackground_color'))
          .margin({ right: $r('app.float.friendCircleRedDot_margin') })


        Image($rawfile('images/dai.jpg'))
          .width($r('app.float.findImageLength'))
          .height($r('app.float.findImageLength'))
          .objectFit(ImageFit.Cover)
          .borderRadius($r('app.float.friendCircleImageDai_borderRadius'))
          .margin({ left: $r('app.float.friendCircleImageDai_margin') })

        Image($rawfile('images/go.png'))
          .width($r('app.float.goImageLength'))
          .height($r('app.float.friendCircleImageGo_height'))
          .margin({ right: $r('app.float.friendCircleImageGo_margin') })
      }
      .onClick(() => {
        router.pushUrl({ url: 'pages/friendCircle' }, router.RouterMode.Single)
      })
      .justifyContent(FlexAlign.SpaceBetween)
      .width(CommonConstants.FULL_PARENT)
      .height($r('app.float.rowFriendCircle_height'))
      .backgroundColor(Color.White)
    }
    .width(CommonConstants.FULL_PARENT)
    .height(CommonConstants.FULL_PARENT)
  }
}