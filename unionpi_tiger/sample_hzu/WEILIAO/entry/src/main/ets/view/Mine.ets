/**
 * @author: Mr.Dai
 */
import CommonConstants from '../common/constants/CommonConstants'

@Component
export default struct WxListPage {
  build() {
    Column() {
      List({
        space: CommonConstants.WXLIST_PAGE_LIST_SPACE
      }) {
        ListItem() {
          headerCustomComponent()
        }.backgroundColor($r('app.color.mineBGC'))
        .width(CommonConstants.FULL_PARENT)
        .height(CommonConstants.MINE_LIST_ITEM_HEIGHT)

        ListItem() {
          RowCustomComponent({ title: CommonConstants.SERVER, icon: $rawfile('images/pay.png') })
        }
        .width(CommonConstants.FULL_PARENT)

        ListItem() {
          Column() {
            RowCustomComponent({ title: CommonConstants.COLLECT, icon: $rawfile('images/collect.png') })
            Divider()
              .strokeWidth(CommonConstants.STROKE_WIDTH)
              .color($r('app.color.dividerCustom'))
              .padding({ left: $r('app.float.dividerCustom_Padding') })
            RowCustomComponent({ title: CommonConstants.CARD_CASE, icon: $rawfile('images/card.png') })
            Divider()
              .strokeWidth(CommonConstants.STROKE_WIDTH)
              .color($r('app.color.dividerCustom'))
              .padding({ left: $r('app.float.dividerCustom_Padding') })

            RowCustomComponent({ title: CommonConstants.PHIZ, icon: $rawfile('images/smile.png') })

          }.backgroundColor($r('app.color.mineBGC'))
        }
        .width(CommonConstants.FULL_PARENT)

        ListItem() {
          RowCustomComponent({ title: CommonConstants.SET, icon: $rawfile('images/set.png') })
        }
        .width(CommonConstants.FULL_PARENT)
      }
    }
    .width(CommonConstants.FULL_PARENT)
    .height(CommonConstants.FULL_PARENT)
    .backgroundColor($r('app.color.MINE_Column_BG'))
  }
}


//自定义组件
@Component
struct headerCustomComponent {
  build() {
    Row() {
      Flex({
        direction: FlexDirection.Row,
        alignItems: ItemAlign.Center,
        justifyContent: FlexAlign.SpaceBetween
      }) {
        Image($rawfile('images/dai.jpg'))
          .width($r('app.float.mineImageWH'))
          .height($r('app.float.mineImageWH'))
          .borderRadius($r('app.float.mineImageBorderRadius'))
        Column({ space: CommonConstants.ROW_CUSTOM_COMPONENT_MARGIN_L }) {
          Text($r('app.string.MINE_hCC_TEXT_ACCOUNT_NAME'))
            .fontSize($r('app.float.MINE_ACCOUNT_NAME_TEXT_FONTSIZE'))
            .fontWeight(FontWeight.Bold)
          Text($r('app.string.MINE_hCC_TEXT_WEILIAO_ACCOUNT'))
            .fontSize($r('app.float.headerCustomComponent_TEXT_FONTSIZE'))
          Button($r('app.string.MINE_BUTTON_TEXT'), { type: ButtonType.Capsule })
            .height($r('app.float.MINE_BUTTON_H'))
            .backgroundColor($r('app.color.MINE_BUTTON_BG'))
            .fontColor($r('app.color.MINE_BUTTON_FONT'))
            .fontSize($r('app.float.MINE_BUTTON_FONTSIZE'))
            .border({
              width: CommonConstants.MINE_BUTTON_BORDER_WIDTH,
              color: $r('app.color.MINE_BUTTON')
            })
        }
        .alignItems(HorizontalAlign.Start)
        .justifyContent(FlexAlign.Center)
        .margin({
          left: $r('app.float.headerCustomComponent_MARGIN_L')
        })
        .flexGrow(CommonConstants.FLEX_GROW)
        .height(CommonConstants.FULL_PARENT)

        QRCode(CommonConstants.QRCODE_TEXT)
          .width($r('app.float.QRCode_WIDTH'))
          .margin({ right: $r('app.float.QRCode_MARGIN_R') })
          .color($r('app.color.QRCode'))
        Image($rawfile('images/go.png'))
          .width($r('app.float.imageCustomArrow_WIDTH'))
          .height($r('app.float.imageCustomArrow_HEIGHT'))
      }
    }
    .width(CommonConstants.FULL_PARENT)
    .padding($r('app.float.headerCustomComponent_PADDING'))
  }
}

//自定义组件
@Component
struct RowCustomComponent {
  private title: string = ''
  private icon: ResourceStr = ''

  build() {
    Row() {
      Row() {
        Image(this.icon)
          .width($r('app.float.rowCustomComponent_WH'))
          .height($r('app.float.rowCustomComponent_WH'))
        Text(this.title)
          .fontSize($r('app.float.rowCustomComponent_TEXT_FONTSIZE'))
          .margin({
            left: $r('app.float.rowCustomComponent_MARGIN_L')
          })
      }

      Image($rawfile('images/go.png'))
        .width($r('app.float.imageCustomArrow_WIDTH'))
        .height($r('app.float.imageCustomArrow_HEIGHT'))
    }
    .justifyContent(FlexAlign.SpaceBetween)
    .backgroundColor($r('app.color.mineBGC'))
    .width(CommonConstants.FULL_PARENT)
    .padding($r('app.float.rowCustomComponent_PADDING'))
  }
}