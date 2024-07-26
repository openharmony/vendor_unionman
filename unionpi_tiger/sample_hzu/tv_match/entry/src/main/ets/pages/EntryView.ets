import image from '@ohos.multimedia.image';
import router from '@ohos.router';
// @ts-ignore
import { BusinessError } from '@ohos.base';

import { quanvideoSrc, setquan } from '../compon/hea';
import '../compon/hea'


class Dmzi {
  public tsrc: Resource = $r('app.media.icon');
  public videoSrc: Resource = $rawfile('videotest.mp4')
  public sname: string = '视频'

  constructor(tsrc: Resource, videSrc: Resource, sname: string) {
    this.tsrc = tsrc
    this.videoSrc = videSrc
    this.sname = sname
  }
}


class Fqzi {
  public tsrc: Resource = $r('app.media.icon');
  public srcc: string = 'pages/phb'
  public sname: string = '排行榜'

  constructor(tsrc: Resource, srcc: string, sname: string) {
    this.tsrc = tsrc
    this.srcc = srcc
    this.sname = sname
  }
}

let dmsz: Dmzi[] = [
  new Dmzi($r('app.media.dm1'), $rawfile('cs1.mp4'), '视频'),
  new Dmzi($r('app.media.dm2'), $rawfile('videotest.mp4'), '视频1'),
  new Dmzi($r('app.media.dm3'), $rawfile('videotest.mp4'), '视频2'),
]

let dssz: Dmzi[] = [
  new Dmzi($r('app.media.ds1'), $rawfile('cs1.mp4'), '视频')
]

let dysz: Dmzi[] = [
  new Dmzi($r('app.media.dy1'), $rawfile('cs1.mp4'), '视频')
]

let fqsz: Fqzi[] = [
  new Fqzi($r('app.media.icon'), 'pages/phb', '排行榜')
]

@Component
struct shipin {
  private dm: Dmzi = new Dmzi($r('app.media.icon'), $rawfile('videotest.mp4'), '视频')
  @State bw: number = 0
  @State bcolor: string = '#00e4eeea'

  build() {
    Stack() {
      Row() {
        Image(this.dm.tsrc)
          .width(190)
          .height(105)
          .borderRadius(10)
          .focusable(true)
          .border({ width: this.bw })
          .borderColor(this.bcolor)
          .onFocus(
            () => {
              this.bw = 3
              this.bcolor = '#ff0c27d4'
            }
          )
          .onBlur(
            () => {
              this.bw = 0
              this.bcolor = '#00e4eeea'
            }

          )
          .onKeyEvent(
            (event: KeyEvent) => {
              if (event) {
                if (event.keyCode === 2016) {
                  console.info(`Succeeded in clicking the 'Next' button.`)
                  setquan(this.dm.videoSrc)
                  // 跳转到第二页
                  router.pushUrl({ url: 'pages/videoview' }).then(() => {
                    console.info('Succeeded in jumping to the second page.')
                  }).catch((err: BusinessError) => {
                    console.error(`Failed to jump to the second page.Code is ${err.code}, message is ${err.message}`)
                  })
                }

              }
            }
          )

      }

      Row() {
        Text(this.dm.sname)
          .fontColor(Color.White)
          .fontSize(20)
          .margin({ top: 80 })
          .backgroundColor('#4d3c3530')
          .width('100%')
          .textAlign(TextAlign.Center)
          .borderRadius(8)
      }
      .width(190)
      .height(25)
      .justifyContent(FlexAlign.Center)
    }
    .width(190)
    .height(105)
  }
}

@Component
struct fqti {
  private dm: Fqzi = new Fqzi($r('app.media.icon'), 'pages/phb', 'phb')
  private w: Length = 120
  private h: Length = 105
  @State bw: number = 0
  @State bcolor: string = '#00e4eeea'

  build() {
    Column() {
      Stack() {
        Image(this.dm.tsrc)
          .width(this.w)
          .height(this.h)
          .focusable(true)
          .border({ width: this.bw })
          .borderColor(this.bcolor)
          .onFocus(
            () => {
              this.bw = 3
              this.bcolor = '#ff0c27d4'
            }
          )
          .onBlur(
            () => {
              this.bw = 0
              this.bcolor = '#00e4eeea'
            }

          )
          .borderRadius(10)
          .onKeyEvent(
            (event: KeyEvent) => {
              if (event) {
                if (event.keyCode === 2016) {
                  console.info(`Succeeded in clicking the 'Next' button.`)
                  // 跳转到第二页
                  router.pushUrl({ url: this.dm.srcc }).then(() => {
                    console.info('Succeeded in jumping to the second page.')
                  }).catch((err: BusinessError) => {
                    console.error(`Failed to jump to the second page.Code is ${err.code}, message is ${err.message}`)
                  })
                }
              }
            }
          )


        Text(this.dm.sname)
          .fontColor(Color.White)
          .fontSize(20)
          .margin({ top: 80 })
          .backgroundColor('#4d3c3530')
          .width(this.w)
          .textAlign(TextAlign.Center)
          .borderRadius(12)

      }

    }
  }
}


@Component
struct tjti {
  private dm: Fqzi = new Fqzi($r('app.media.icon'), 'pages/phb', 'phb')
  @State bw: number = 0
  @State bcolor: string = '#00e4eeea'

  build() {
    Stack() {
      Row() {
        Image(this.dm.tsrc)
          .width('100%')
          .height('100%')
          .focusable(true)
          .border({ width: this.bw })
          .borderColor(this.bcolor)
          .onFocus(
            () => {
              this.bw = 3
              this.bcolor = '#ff0c27d4'
            }
          )
          .onBlur(
            () => {
              this.bw = 0
              this.bcolor = '#00e4eeea'
            }

          )
          .onKeyEvent(
            (event: KeyEvent) => {
              if (event) {
                if (event.keyCode === 2016) {
                  console.info(`Succeeded in clicking the 'Next' button.`)
                  // 跳转到第二页
                  router.pushUrl({ url: this.dm.srcc }).then(() => {
                    console.info('Succeeded in jumping to the second page.')
                  }).catch((err: BusinessError) => {
                    console.error(`Failed to jump to the second page.Code is ${err.code}, message is ${err.message}`)
                  })
                }
              }
            }
          )
      }
      .width('100%')
      .height('100%')

      Text('     ' + this.dm.sname)
        .fontColor(Color.White)
        .fontSize(25)
        .margin({ top: 200 })
        .backgroundColor('#4d3c3530')
        .width('100%')
        .height(30)


    }
  }
}


@Component
struct zhuti {
  private ab: Dmzi[] = dmsz

  build() {
    Row() {
      List({ space: 10 }) {
        ListItem() {
        }
        .margin({ left: 20 })

        ForEach(
          this.ab,
          (item, index) => {
            if (index % 2 === 0) {
              ListItem() {

                Column() {

                  Row() {
                    shipin({ dm: this.ab[index] })
                  }

                  if (index + 1 <= this.ab.length - 1) {

                    Row() {
                      shipin({ dm: this.ab[index + 1] })
                    }
                    .margin({ top: 30 })
                  }

                }
              }

            }

          }
        )
      }
      .listDirection(Axis.Horizontal)
      .height('100%')

    }
    .margin({ top: 20 })
    .width('100%')
  }
}

@Component
struct fqzhuti {
  build() {
    Row() {
      List({ space: 10 }) {
        ListItem() {
        }
        .margin({ left: 20 })

        ForEach(
          fqsz,
          (item, index) => {
            if (index % 2 === 0) {
              ListItem() {
                Column() {
                  Row() {
                    fqti({ dm: fqsz[index] })
                  }
                  .margin({ top: 15 })

                  if (index + 1 <= fqsz.length - 1) {
                    Row() {
                      fqti({ dm: fqsz[index+1] })
                    }
                    .margin({ top: 15 })
                  }
                }
              }
            }
          }
        )

      }
      .listDirection(Axis.Horizontal)
      .height('100%')
    }
    .width('100%')
  }
}


@Component
struct wdzhuti {
  build() {
    Row() {
      Column() {
        fqti({ dm: new Fqzi($r('app.media.icon'), 'pages/shoucang', '我的收藏') })
      }
      .margin({ top: 13, left: 10 })
      .justifyContent(FlexAlign.Start)

      /*Column(){
        fqti({dm: new Fqzi($r('app.media.icon'), 'pages/shoucang', '我的收藏')})
      }
      .justifyContent(FlexAlign.Start)
      .margin({top:13,left:10})*/
    }
    .width('100%')
    .height('100%')
    .alignItems(VerticalAlign.Top)
  }
}


@Component
struct tuizhuti {
  build() {
    Row() {
      Swiper() {
        tjti({ dm: new Fqzi($r('app.media.lunbo1'), 'pages/videoview', '视频1') })
        tjti({ dm: new Fqzi($r('app.media.lunbo2'), 'pages/videoview', '视频2') })
        tjti({ dm: new Fqzi($r('app.media.lunbo3'), 'pages/videoview', '视频3') })
      }
      .width('100%')
      .height('100%')
      .autoPlay(true)
      .interval(4000)
      .borderRadius(15)
      .indicatorStyle({ left: 1330 })
    }
    .height('70%')
    .width('80%')
    .justifyContent(FlexAlign.Center)
    .margin({ bottom: 70, top: 30 })
  }
}


@Entry
@Component
struct ImageExample3 {
  private imageOne: Resource = $r('app.media.icon');
  private imageTwo: Resource = $r('app.media.icon');
  @State src: Resource = this.imageOne
  @State currentIndex: number = 0

  @Builder
  curr(title: string, index: number) {
    Row() {
      Column({ space: 4 }) {
        Text(title).fontColor(this.currentIndex === index ? Color.White : Color.Gray)
          .fontSize(25)

      }
      .justifyContent(FlexAlign.Center)
      .width('100%')
      .height('100%')
      .margin({ top: 35, bottom: 10 })
    }
  }

  build() {

    Row() {

      Column() {
        //为图片添加点击事件，点击完成后加载特定图片
        Tabs() {
          TabContent() {
            wdzhuti()
          }
          .tabBar(this.curr('我的', 0))


          TabContent() {
            tuizhuti()

          }
          .tabBar(this.curr('推荐', 1))

          TabContent() {
            fqzhuti()
          }
          .tabBar(this.curr('分区', 2))

          TabContent() {
            zhuti()
          }
          .tabBar(this.curr('热门', 3))

          TabContent() {
            zhuti({ ab: dssz })
          }
          .tabBar(this.curr('电视剧', 4))

          TabContent() {
            zhuti({ ab: dysz })
          }
          .tabBar(this.curr('电影', 5))

          TabContent() {
            zhuti()
          }
          .tabBar(this.curr('动漫', 6))

        }
        .vertical(false)
        .onChange(
          index => {
            this.currentIndex = index
          }
        )
      }

    }
    .width('100%').height('100%')
    .backgroundImage($r('app.media.bj1'))

  }
}
