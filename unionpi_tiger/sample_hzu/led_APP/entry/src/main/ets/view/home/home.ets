import DateDialog from '../../../../../../../LED/entry/src/main/ets/dialog/DateDialog'
import DateUtil from '../../../../../../../LED/entry/src/main/ets/utils/DateUtil'

interface list {

  Consume: number;
  num: number;
  target: number;
  pre: string;

}

@Preview
@Component
export default struct HomeContent {
  @StorageProp('date') date: number = DateUtil.beginTimeOfDay(new Date())
  @State arr: list[] = [
    {

      Consume: 1,
      num: 23,
      target: 1,
      pre: '2024-11-30'
    },
    {

      Consume: 1,
      num: 19,
      target: 1,
      pre: '2024-11-11'
    },
    {
      Consume: 1,
      num: 14000,
      target: 1,
      pre: '2024-11-29'
    },
  ]
  controller: CustomDialogController = new CustomDialogController({
    builder: DateDialog({ date: new Date(this.date) })
  })

  build() {
    Column() {
      // Column() {
      Text('LED')
        .fontSize(25)
        .fontWeight(600)
        .margin({ top: 10, bottom: 100, left: 20 })

      // 轮播图
      Swiper() {
        Image($r('app.media.img_harmony'))
          .width('90%')
          .height(200)

        Image($r('app.media.Hello'))
          .width('90%')
          .height('50%')

        Image($r('app.media.img_dark'))
          .width('90%')
          .height(200)
      }
      .loop(true) //可循环切换,头尾相接
      .autoPlay(true)
      .height("23%")
      .width("90%")
      .position({ x: 17, y: 50 })
      .layoutWeight(1)


      Row() {

        Text(DateUtil.formatDate(this.date))
          .fontSize(15)
          .fontWeight(500)
        Image($r('app.media.icon1'))
          .width(20)
      }
      .onClick(() => {
        this.controller.open()

      })
      .width('90%')
      .height(50)
      .backgroundColor(Color.White)
      .margin({ left: 19, top: 90 })
      .borderRadius(20)
      .justifyContent(FlexAlign.Center)

      // }
      // // .backgroundImage($r('app.media.Hello'))
      // .backgroundImageSize({ width: '100%', height: '100%' })
      //
      // .width('100%')
      // .height('40%')
      // .alignItems(HorizontalAlign.Start)
      // .borderRadius({ bottomLeft: 20, bottomRight: 20 })

      Stack() {
        Column() {
          Text('历史温度' + DateUtil.getDate(this.date))
            .fontSize(13)
            .fontWeight(700)
            .margin({ left: 20, top: 20, bottom: 10 })
          if (this.arr.length !== 0) {
            Column() {
              List({ space: 10 }) {
                ForEach(this.arr, (item: list) => {
                  if (item.pre === DateUtil.getDate(this.date)) {
                    ListItem() {
                      Row() {

                        Image($r('app.media.icon1'))
                          .width(50)
                          .height(50)

                        if (item.num > 20 && item.num < 50) {
                          Text("适宜")
                            .fontSize(23)
                            .fontWeight(600)
                            .opacity(0.8)
                        }
                        if (item.num <= 20) {
                          Text("不适宜")

                            .fontSize(23)
                            .fontWeight(600)
                            .opacity(0.8)
                        }

                        if (item.num > 50) {
                          Text("异常")
                            .fontColor(Color.Red)
                            .fontSize(23)
                            .fontWeight(600)
                            .opacity(0.8)
                        }
                        Blank()
                        if (item.num) {
                          Text('温度：' + item.num + '摄氏度 时间：' + item.pre)
                            .fontSize(13)
                            .fontWeight(600)
                            .margin({ right: 10 })
                            .fontColor($r('app.color.app_color_bar_normal'))

                        } else {
                          Text("无法显示")
                            .fontSize(13)
                            .fontWeight(600)
                            .margin({ right: 10 })
                            .fontColor($r('app.color.app_color_bar_normal'))
                        }
                      }
                      .width('100%')
                      .backgroundColor(Color.White)
                      .borderRadius(15)
                    }
                    .width('90%')
                  }
                })
              }
              .width('100%')
              .alignListItem(ListItemAlign.Center)
            }
            .width('100%')
          }
          else {
            Column({ space: 8 }) {
              Text("没有任务")
                .fontSize(16)
                .opacity(0.4)

            }
            .margin({ top: 68, left: 110 })
          }

        }
        .width('100%')
        .height('100%')
        .alignItems(HorizontalAlign.Start)
      }
      .width('100%')
      .height('100%')
    }
    .backgroundColor('#f5f5f5')
  }
}