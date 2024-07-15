/**
 * @author: Mr.Hong
 */
import router from '@ohos.router'
import CommonConstants from '../common/constants/CommonConstants'

interface SwipeInterface {
  username: string,
  info: string,
  date: string,
  noRead: number,
  icon: ResourceStr,
}

// 全局自定义构建函数
@Builder
function msgList(item: SwipeInterface) {
  Row() {
    Badge({
      count: item.noRead,
      position: BadgePosition.RightTop,
      style: {}
    }) {
      Image(item.icon)
        .width($r('app.float.homeIcon_width'))
        .height(($r('app.float.homeIcon_height')))
        .borderRadius($r('app.float.homeIcon_borderRadius'))
    }

    Column() {
      Text(item.username)
        .fontSize($r('app.float.homeUsername_fontSize'))
        .fontWeight(FontWeight.Bold)
      Text(item.info)
        .fontSize($r('app.float.homeInfo_fontSize'))
        .fontColor($r('app.color.home_info_color'))
        .maxLines(1)
        .textOverflow({ overflow: TextOverflow.Ellipsis })
    }
    .justifyContent(FlexAlign.SpaceBetween)
    .height(40)
    .margin({
      left: 10
    })
    .alignItems(HorizontalAlign.Start)
    .layoutWeight(1)

    Text(item.date)
      .fontSize($r('app.float.homeDDate_fontSize'))
      .fontColor($r('app.color.home_date_color'))
      .width($r('app.float.homeDDate_width'))
      .textAlign(TextAlign.End)
  }
  .padding(10)
}


@Component
export default struct Home {
  //SwipeInterface需获取数据库在聊信息后进行追加


  @State list: SwipeInterface[] = [
    {
      username: '戴sh',
      info: '哈哈哈哈',
      date: '10:23',
      noRead: 0,
      icon: $rawfile('images/dai.jpg')
    },
    {
      username: '洪zx',
      info: '奥奥奥',
      date: '10:22',
      noRead: 2,
      icon: $rawfile('images/hong.jpg')
    },
    {
      username: '谢jr',
      info: '嗯嗯嗯',
      date: '昨天',
      noRead: 0,
      icon: $rawfile('images/xie.jpg')
    },
    {
      username: '陈zs',
      info: '嗨嗨嗨',
      date: '昨天',
      noRead: 5,
      icon: $rawfile('images/chen.jpg')
    },
    {
      username: '冯zp',
      info: '哦哦哦',
      date: '星期三',
      noRead: 0,
      icon: $rawfile('images/feng.jpg')
    },

  ]
  @State changeValue: string = ''

  getMsglist() {
    return this.list.filter(item => {
      return item.username.includes(this.changeValue) || item.info.includes(this.changeValue)
    })
  }

  //自定义组件内自定义构建函数
  @Builder
  actionItem(index: number) {
    Row() {
      Text(CommonConstants.MARK_UNREAD)
        .fontSize($r('app.float.home_actionItemText_fontSize'))
        .fontColor($r('app.color.home_actionItemText_color'))
        .height($r('app.float.home_actionItemText_height'))
        .width(80)
        .backgroundColor('#409EFF')
        .onClick(() => {
          this.list[index].noRead = 1
          this.list = [...this.list];
        })
      Text(CommonConstants.NOT_SHOWN)
        .fontSize($r('app.float.home_actionItemText_fontSize'))
        .fontColor($r('app.color.home_actionItemText_color'))
        .height($r('app.float.home_actionItemText_height'))
        .width(70)
        .backgroundColor('#E6A23C')
        .onClick(() => {
          this.list.splice(index, 1)
        })
      Text(CommonConstants.DELETE)
        .fontSize($r('app.float.home_actionItemText_fontSize'))
        .fontColor($r('app.color.home_actionItemText_color'))
        .height($r('app.float.home_actionItemText_height'))
        .width(60)
        .backgroundColor('#F56C6C')
        .onClick(() => {
          this.list.splice(index, 1)
        })
    }
    .margin({ left: 10 })
  }

  build() {
    Column() {
      Row() {
        Search({ placeholder: CommonConstants.SEARCH })
          .backgroundColor('#ffffff')
          .onChange((value: string) => {
            this.changeValue = value
          })
          .margin(20)
      }.width(CommonConstants.FULL_PARENT)

      List() {
        ForEach(this.getMsglist(), (item: SwipeInterface, key) => {
          ListItem() {
            msgList(item)
          }
          .onClick(() => {
            //点击后跳转chatPage页面，传送用户信息进行展示
            router.pushUrl({
              //修改为跳转到中心连接对话界面---connectPage
              url: 'pages/connectPage',
              //传参
              params: item
            }, router.RouterMode.Single)

          })

          .width(CommonConstants.FULL_PARENT)
          .height('60')
          .backgroundColor(Color.White)

          .swipeAction({
            end: this.actionItem(key)
          })
        })
      }
      .divider({
        strokeWidth: 1,
        color: $r('app.color.home_divider_color')
      })
      .backgroundColor($r('app.color.home_divider_backgroundColor'))
      .layoutWeight(1)
    }
    .width(CommonConstants.FULL_PARENT)
    .height(CommonConstants.FULL_PARENT)
    .backgroundColor($r('app.color.home_backgroundColor'));
  }
}