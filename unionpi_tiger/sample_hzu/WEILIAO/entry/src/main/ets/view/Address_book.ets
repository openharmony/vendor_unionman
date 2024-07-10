/**
 * @author: Mr.Hong
 */
import router from '@ohos.router';
import CommonConstants from '../common/constants/CommonConstants';


const alphabets = ['→', '※', 'A', 'B', 'C', 'D', 'E', 'F', 'H', 'I',
  'G', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
  'W', 'X', 'Y', 'Z', '#'];

interface SwipeInterface {
  icon: ResourceStr
  username: string
}

interface CarInterface {
  alphabet: string,
  carItem: SwipeInterface[]
}

@Component
export default struct Address_book {
  @State selectedIndex: number = 0
  private listScroller: Scroller = new Scroller()
  @State carList: CarInterface[] = [{
    alphabet: '星标朋友',
    carItem: [
      {
        icon: $r('app.media.comprehensivePracticalTraining'),
        username: '综合实训'
      },

    ]
  },
    {
      alphabet: '我的企业及联系人',
      carItem: [
        {
          icon: $rawfile('images/qiye.png'),
          username: '企业微信联系人'
        },
      ]
    },
    {
      alphabet: 'C',
      carItem: [
        {
          icon: $rawfile('images/chen.jpg'),
          username: '陈zs'
        },

      ]
    },
    {
      alphabet: 'D',
      carItem: [
        {
          icon: $rawfile('images/dai.jpg'),
          username: '戴sh'
        },
      ]
    },

    {
      alphabet: 'F',
      carItem: [
        {
          icon: $rawfile('images/feng.jpg'),
          username: '冯zp'
        },
      ]
    },
    {
      alphabet: 'H',
      carItem: [
        {
          icon: $rawfile('images/hong.jpg'),
          username: '洪zx'
        },
      ]
    },
    {
      alphabet: 'X',
      carItem: [
        {
          icon: $rawfile('images/xie.jpg'),
          username: '谢jr'
        },
      ]
    },


  ]

  //自定义组件内自定义构建函数
  @Builder
  itemHead(text: string) {
    Text(text)
      .fontSize($r('app.float.AddressBookItemHead_FontSize'))
      .backgroundColor($r('app.color.addressBookItemHead_color'))
      .width(CommonConstants.FULL_PARENT)
      .padding($r('app.float.AddressBookItemHead_Padding'))
  }

  build() {
    Column() {
      Stack({ alignContent: Alignment.End }) {
        Column() {
          Row() {
            Stack() {
              Text(CommonConstants.ADDRESS_BOOK_TITLE)
                .width(CommonConstants.FULL_PARENT)
                .lineHeight($r('app.float.addressBook_titleLineHeight'))
                .fontSize($r('app.float.addressBook_titleFontSize'))
                .fontWeight(FontWeight.Bold)
                .textAlign(TextAlign.Center)
              Row({ space: 20 }) {
                Image($rawfile('images/search.png'))
                  .width($r('app.float.addressBook_imageLength')).height($r('app.float.addressBook_imageLength'))
                Image($rawfile('images/add.png'))
                  .width($r('app.float.addressBook_imageLength')).height($r('app.float.addressBook_imageLength'))
              }.width(CommonConstants.FULL_PARENT).justifyContent(FlexAlign.End)
            }
          }
          .width(CommonConstants.FULL_PARENT)
          .backgroundColor($r('app.color.addressBookTitle_color'))
          .padding($r('app.float.addressBookTitle_padding'))

          Row() {
            Image($r('app.media.newFriends'))
              .width($r('app.float.addressBookRowImage_length'))
              .height($r('app.float.addressBookRowImage_length'))
            Text(CommonConstants.NEW_FRIENDS)
              .fontSize($r('app.float.addressBookRowText_fontSize'))
              .padding($r('app.float.addressBookRowText_padding'))
              .width(CommonConstants.HALF_PARENT)
          }.width(CommonConstants.FULL_PARENT).justifyContent(FlexAlign.Start)

          Row() {
            Image($r('app.media.friendsOfOnlyChat'))
              .width($r('app.float.addressBookRowImage_length'))
              .height($r('app.float.addressBookRowImage_length'))
            Text(CommonConstants.CHAT_ONLY_WITH_FRIENDS)
              .fontSize($r('app.float.addressBookRowText_fontSize'))
              .padding($r('app.float.addressBookRowText_padding'))
              .width(CommonConstants.HALF_PARENT)
          }.width(CommonConstants.FULL_PARENT).justifyContent(FlexAlign.Start)

          Row() {
            Image($r('app.media.groupChat'))
              .width($r('app.float.addressBookRowImage_length'))
              .height($r('app.float.addressBookRowImage_length'))
            Text(CommonConstants.GROUP_CHATS)
              .fontSize($r('app.float.addressBookRowText_fontSize'))
              .padding($r('app.float.addressBookRowText_padding'))
              .width(CommonConstants.HALF_PARENT)
          }.width(CommonConstants.FULL_PARENT).justifyContent(FlexAlign.Start)

          Row() {
            Image($r('app.media.label'))
              .width($r('app.float.addressBookRowImage_length'))
              .height($r('app.float.addressBookRowImage_length'))
            Text(CommonConstants.LABEL)
              .fontSize($r('app.float.addressBookRowText_fontSize'))
              .padding($r('app.float.addressBookRowText_padding'))
              .width(CommonConstants.HALF_PARENT)
          }.width(CommonConstants.FULL_PARENT).justifyContent(FlexAlign.Start)

          Row() {
            Image($r('app.media.publicAccount'))
              .width($r('app.float.addressBookRowImage_length'))
              .height($r('app.float.addressBookRowImage_length'))
            Text(CommonConstants.OFFICIAL_ACCOUNT)
              .fontSize($r('app.float.addressBookRowText_fontSize'))
              .padding($r('app.float.addressBookRowText_padding'))
              .width(CommonConstants.HALF_PARENT)
          }.width(CommonConstants.FULL_PARENT).justifyContent(FlexAlign.Start)

          List({ scroller: this.listScroller }) {
            ForEach(this.carList, (item: CarInterface) => {
              ListItemGroup({ header: this.itemHead(item.alphabet) }) {
                ForEach(item.carItem, (pro: SwipeInterface) => {
                  ListItem() {
                    Row() {
                      Image(pro.icon)
                        .width($r('app.float.addressBookRowImage_length'))
                        .height($r('app.float.addressBookRowImage_length'))
                      Text(pro.username)
                        .fontSize($r('app.float.addressBookRowText_fontSize'))
                        .padding($r('app.float.addressBookRowText_padding'))
                        .width(CommonConstants.HALF_PARENT)
                    }
                    .onClick(() => {
                      //点击后跳转chatPage页面，传送用户信息进行展示
                      router.pushUrl({
                        //修改为跳转到中心连接对话界面---connectPage
                        url: 'pages/connectPage',
                        //传参
                        params: pro
                      }, router.RouterMode.Single)
                    })
                  }
                })
              }
            })
            ListItem() {
              Row() {
                Text()
                  .width(CommonConstants.FULL_PARENT)
                  .height(CommonConstants.TRIPLE_PARENT)
                  .backgroundColor(Color.White)
              }
            }
          }
          .onScrollIndex((index: number) => {
            this.selectedIndex = index

          })
          .sticky(StickyStyle.Header)
        }.height(CommonConstants.FULL_PARENT)
        .width(CommonConstants.FULL_PARENT)

        AlphabetIndexer({ arrayValue: alphabets, selected: 0 })
          .selected(this.selectedIndex)
          .onSelect((index) => {
            this.listScroller.scrollToIndex(index)
          })
      }
    }
    .width(CommonConstants.FULL_PARENT)
    .height(CommonConstants.FULL_PARENT)
  }
}
