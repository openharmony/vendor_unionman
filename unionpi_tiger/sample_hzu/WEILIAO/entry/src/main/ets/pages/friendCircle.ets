/**
 * @author: Mr.Dai
 */
import router from '@ohos.router';
import CommonConstants from '../common/constants/CommonConstants';


class MomentClass {
  public nickName: string; //昵称
  public content: string; //内容
  public images: ResourceStr[]; //内容图片列表

  constructor(nickName: string, content: string, images: ResourceStr[]) {
    this.nickName = nickName;
    this.content = content;
    this.images = images;
  }
}


@Entry
@Component
struct GridViewPage {
  @State momentList: MomentClass[] = [
    new MomentClass('每日一句', '如果睡觉能赚钱，我大概已经富可敌国，还顺便解决了失眠问题。', []),
    new MomentClass('每日一句', '我们大多数人骨子里头都有一种刚愎任性的意气，尤其是在少不更事和坠入爱河之时。', [$rawfile('images/11.png')]),
    new MomentClass('努力向上', '每一次举重，都是在举起生活的重量；每一次奔跑，都是在追赶更好的自己。', [$rawfile('images/7.png'), $rawfile('images/7.png')]),
    new MomentClass('每日一句', '街角的咖啡香，混合着黄昏的温柔，每一口都是对生活的细腻品味。', [$rawfile('images/9.png'), $rawfile('images/10.png'), $rawfile('images/11.png')]),
    new MomentClass('每日一句', '走过熙熙攘攘的夜市，手中的小吃仿佛藏着人间烟火，每一口都是平凡日子的小确幸。', [$rawfile('images/1.png'), $rawfile('images/2.png'), $rawfile('images/3.png'), $rawfile('images/4.png')]),
    new MomentClass('今日一句', '别人笑我太疯癫，我笑他人看不穿，其实我秋衣穿反了而已。', []),
    new MomentClass('毒鸡汤', '不是所有牛奶都叫特仑苏，就像不是所有努力都有结果，更多时候只是“特能输”。', []),
    new MomentClass('英语一句话', 'He who cannot dance puts the blame on the floor.', []),
    new MomentClass('毒鸡汤', '机会总是留给有准备的人，但问题是，你永远不知道机会在哪个转角玩手机。', []),
    new MomentClass('毒鸡汤', '人生就像心电图，总要有起伏，如果一帆风顺，那说明你挂了。', [])
  ];

  //计算列数
  calcColumnsTemplate(index: number) {
    let result: string = '1fr'
    let length: number = this.momentList[index].images.length || 0
    if (length == 1) {
      result = '1fr'
    } else if (length == 2 || length == 4) {
      result = '1fr 1fr'
    } else {
      result = '1fr 1fr 1fr'
    }
    return result
  }

  //计算行数
  calcRowsTemplate(index: number) {
    let result: string = '1fr'
    let length: number = this.momentList[index].images.length || 0
    if (length == 1) {
      result = '1fr'
    } else if (length >= 2 && length <= 6 && length != 3) {
      result = '1fr 1fr'
    } else {
      result = '1fr 1fr 1fr'
    }
    return result
  }

  //计算宽度
  calcGridWidth(index: number) {
    let result: number = 0
    let length: number = this.momentList[index].images.length || 0
    if (length == 1) {
      result = 70
    } else if (length == 2 || length == 4) {
      result = 145
    } else {
      result = 220
    }
    return result
  }

  //计算高度
  calcGridHeight(index: number) {
    let result: number = 0
    let length: number = this.momentList[index].images.length || 0
    if (length <= 3) {
      result = 70
    } else if (length > 3 && length <= 6) {
      result = 145
    } else {
      result = 220
    }
    return result
  }

  build() {
    Column() {
      List({ space: 20 }) {
        ListItem() {
          Stack({ alignContent: Alignment.BottomEnd }) {
            Navigator({ target: 'pages/MainInterface', type: NavigationType.Back }) {
              Column() {
                Image($rawfile('images/background.jpg'))
                  .width(CommonConstants.FULL_PARENT)
                  .height($r('app.float.friendCircle_picture_height'))
                  .objectFit(ImageFit.Cover)
              }
              .width(CommonConstants.FULL_PARENT)
              .height($r('app.float.friendCircle_Navigator_height'))
            }


            Row({ space: 10 }) {
              Text('单行道')
                .fontSize($r('app.float.friendCircle_nameSize'))
                .fontColor($r('app.color.nameColor'))
                .margin({ bottom: $r('app.float.nameMargin_bottom') })
              Image($rawfile('images/dai.jpg'))
                .width($r('app.float.picture_wOrh'))
                .height($r('app.float.picture_wOrh'))
                .borderRadius($r('app.float.picture_borderRadius'))
                .objectFit(ImageFit.Cover)
            }
            .height($r('app.float.picture_wOrh'))
            .justifyContent(FlexAlign.End)
            .padding($r('app.float.headPadding'))
          }
          .width(CommonConstants.FULL_PARENT)
          .height($r('app.float.friendCircle_Navigator_height'))
        }

        ForEach(this.momentList, (item: MomentClass, index: number) => {
          ListItem() {
            Row({ space: 10 }) {
              Image($r('app.media.icon'))
                .width($r('app.float.friendList_wOrh'))
                .height($r('app.float.friendList_wOrh'))
                .borderRadius($r('app.float.friendList_borderRadius'))
                .objectFit(ImageFit.Cover)

              Column({ space: 10 }) {
                Text(item.content)
                  .fontSize($r('app.float.friendCleMessage_fS'))
                  .lineHeight($r('app.float.friendCleMessage_LH'))


                if (item.images && item.images.length > 0) {
                  Grid() {

                    // 图片列表
                    ForEach(item.images, (img: ResourceStr, index: number) => {
                      GridItem() {
                        Image(img)
                          .height($r('app.float.pictureList_wOrh'))
                          .width($r('app.float.pictureList_wOrh'))
                          .objectFit(ImageFit.Cover)
                          .borderRadius($r('app.float.pictureList_borderRadius'))
                          .onClick(() => {
                            router.pushUrl({
                              url: 'pages/SwiperPage',
                              params: {
                                images: item.images,
                                index: index
                              }
                            });
                          });
                      }

                    })
                  }
                  .columnsTemplate(this.calcColumnsTemplate(index))
                  .rowsTemplate(this.calcRowsTemplate(index))
                  .columnsGap(5)
                  .rowsGap(5)
                  .width(this.calcGridWidth(index))
                  .height(this.calcGridHeight(index))
                }
                Grid() {
                  GridItem() {
                    Row() {
                      Text('2小时前')
                        .fontSize($r('app.float.Time_fs'))
                        .fontColor($r('app.color.Time'))

                      Image($r('app.media.like_white'))
                        .width($r('app.float.LikeImage_wOrh'))
                        .height($r('app.float.LikeImage_wOrh'))
                        .borderRadius($r('app.float.friendList_borderRadius'))
                        .margin({ left: $r('app.float.LikeImage_margin') })
                        .onClick(() => {
                          AlertDialog.show({ message: CommonConstants.LIKE_SUCCESS })
                        })
                    }

                  }

                }

              }
              .layoutWeight(1)
              .alignItems(HorizontalAlign.Start)
              .justifyContent(FlexAlign.Start)

            }
            .width(CommonConstants.FULL_PARENT)
            .alignItems(VerticalAlign.Top)
          }
          .width(CommonConstants.FULL_PARENT)
          .padding($r('app.float.contentPadding'))
        })

      }
      .divider({ strokeWidth: $r('app.float.friendCle_divider') })
      .width(CommonConstants.FULL_PARENT)
    }
    .width(CommonConstants.FULL_PARENT)
    .height(CommonConstants.FULL_PARENT)
  }
}
