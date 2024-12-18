import home from '../view/home/home';
import color from '../view/color/color';
import person from '../view/person/person';

@Entry
@Component
struct Page {
  @State message: string = 'Hello World';
  @State S: number = 0

  @Builder
  TabBarBuilder(index: number, normalicon: ResourceStr, selIcon: ResourceStr, text: string) {
    Column() {
      Image(this.S === index ? selIcon : normalicon)
        .width(25)
        .height(25)
      Text(text)
        .fontColor(this.S === index ? '#0491d1' : '#8a8a8a')
        .fontWeight(FontWeight.Medium)
    }
  }

  build() {

    Tabs({
      barPosition: BarPosition.End,
      index: this.S

    }) {
      TabContent() {
        // 首页
        home()

      }
      .tabBar(this.TabBarBuilder(
        0,
        $r('app.media.icon_home'),
        $r('app.media.icon_home_selected'),

        "首页"
      ))

      TabContent() {
        // 灯光设置页
        color()
      }
      .tabBar(this.TabBarBuilder(
        1,
        $r('app.media.img_dark'),
        $r('app.media.img_light'),

        "灯光设置"
      ))

      TabContent() {
        // 个人页
        person()
      }
      .tabBar(this.TabBarBuilder(
        2,
        $r('app.media.icon_mine'),
        $r('app.media.icon_mine_selected'),

        "个人"
      ))
    }
    .onChange((index: number) => {
      this.S = index

    })
  }
}