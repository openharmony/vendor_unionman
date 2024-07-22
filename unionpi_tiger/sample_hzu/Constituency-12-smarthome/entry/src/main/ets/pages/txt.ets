import Home from '../view/Home';
import Mine from '../view/Mine';

@Entry
@Component
struct txt {
  @State currentIndex: number = 0

  private tabsController: TabsController = new TabsController()

  @Builder TabBuilder(title: string, index: number, selectedImg: Resource, normalImg: Resource) {
    Row() {
      Image(this.currentIndex === index ? selectedImg : normalImg)
        .width(22)
        .height(22)
      Text(title)
        .margin({ top: 4 })
        .fontSize(18)
        .fontColor(this.currentIndex === index ? $r('app.color.mainPage_selected') : $r('app.color.mainPage_normal'))
    }
    .justifyContent(FlexAlign.Center)
    .height(26)
    .backgroundColor('#00000000')
    .width('100%')
    .onClick(() => {
      this.currentIndex = index
      this.tabsController.changeIndex(this.currentIndex)
    })
  }

  build() {
    Tabs({
      barPosition: BarPosition.End,
      controller: this.tabsController
    }) {
      TabContent() {
        // 首页内容
        Home()
      }
      .backgroundColor('#00000000')
      .tabBar(this.TabBuilder('设备详情', 0, $r('app.media.home_selected'), $r('app.media.home_normal')))

      TabContent() {
        // 我的内容
        Mine()
      }
      .backgroundColor('#00000000')
      .tabBar(this.TabBuilder('个人主页', 1, $r('app.media.mine_selected'), $r('app.media.mine_normal')))
    }
    .width('100%')
    .backgroundColor('#00000000')
    .barHeight(40)
    .barMode(BarMode.Fixed)
    .onChange((index: number) => {
      this.currentIndex = index;
    })
  }
}