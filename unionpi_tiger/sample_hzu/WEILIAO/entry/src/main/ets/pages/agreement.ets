/**
 * @author: Mr.Hong
 */
import CommonConstants from '../common/constants/CommonConstants'

@Entry
@Component
struct Agreement {
  @State agreementMessage: string = CommonConstants.AGREEMENT_MASSAGE
  @State agreementTitle: string = CommonConstants.AGREEMENT_TITLE

  build() {
    Row() {
      Navigation() {
        Column({ space: 10 }) {
          Row() {
            Image($r('app.media.icon'))
              .width($r('app.float.agreement_title_width'))
            Text(this.agreementTitle)
              .fontSize($r('app.float.agreement_title_fontSize'))
          }

          Scroll() {
            Text(this.agreementMessage)
              .fontWeight(FontWeight.Bold)
              .fontSize($r('app.float.agreement_message_fontSize'))
              .width(CommonConstants.FULL_PARENT)
          }
        }
        .justifyContent(FlexAlign.Center)
        .width(CommonConstants.FULL_PARENT)
      }
      .titleMode(NavigationTitleMode.Mini)
    }
    .height(CommonConstants.FULL_PARENT)
  }
}