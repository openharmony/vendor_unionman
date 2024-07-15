/**
 * @author: Mr.Hong
 */
import CommonConstants from '../common/constants/CommonConstants';
import Home from '../view/Home'
import Address_book from '../view/Address_book';
import My from '../view/Mine';
import Find from '../view/Find';

@Entry
@Component
struct MainPage {
  @State currentIndex: number = CommonConstants.HOME_TAB_INDEX;
  private tabsController: TabsController = new TabsController();

  @Builder
  TabBuilder(title: string, index: number, selectedImg: Resource, normalImg: Resource) {
    Column() {
      Image(this.currentIndex === index ? selectedImg : normalImg)
        .width($r('app.float.mainPage_baseTab_size_width'))
        .height($r('app.float.mainPage_baseTab_size_height'))
      Text(title)
        .fontSize($r('app.float.main_tab_fontSize'))
        .fontColor(this.currentIndex === index ? $r('app.color.mainPage_selected') : $r('app.color.mainPage_normal'))
    }
    .justifyContent(FlexAlign.Center)
    .height($r('app.float.mainPage_barHeight'))
    .width(CommonConstants.FULL_PARENT)
    .onClick(() => {
      this.currentIndex = index;
      this.tabsController.changeIndex(this.currentIndex);
    })
  }

  build() {
    Column() {
      Tabs({
        barPosition: BarPosition.End,
        controller: this.tabsController
      }) {
        TabContent() {
          Home()
        }
        .padding({ left: $r('app.float.mainPage_padding'), right: $r('app.float.mainPage_padding') })
        .backgroundColor($r('app.color.mainPage_backgroundColor'))
        .tabBar(this.TabBuilder(CommonConstants.HOME_TITLE, CommonConstants.HOME_TAB_INDEX,
          $r('app.media.Home2'), $r('app.media.Home1')))


        TabContent() {
          Address_book()
        }
        .padding({ left: $r('app.float.mainPage_padding'), right: $r('app.float.mainPage_padding') })
        .backgroundColor($r('app.color.mainPage_backgroundColor'))
        .tabBar(this.TabBuilder(CommonConstants.ADDRESS_BOOK_TITLE, CommonConstants.ADDRESS_BOOK_TAB_INDEX,
          $r('app.media.Address_book2'), $r('app.media.Address_book1')))

        TabContent() {
          Find()
        }
        .padding({ left: $r('app.float.mainPage_padding'), right: $r('app.float.mainPage_padding') })
        .backgroundColor($r('app.color.mainPage_backgroundColor'))
        .tabBar(this.TabBuilder(CommonConstants.FIND_TITLE, CommonConstants.FIND_TAB_INDEX,
          $r('app.media.Find2'), $r('app.media.Find1')))

        TabContent() {
          My()
        }
        .padding({ left: $r('app.float.mainPage_padding'), right: $r('app.float.mainPage_padding') })
        .backgroundColor($r('app.color.mainPage_backgroundColor'))
        .tabBar(this.TabBuilder(CommonConstants.MINE_TITLE, CommonConstants.MINE_TAB_INDEX,
          $r('app.media.Mine2'), $r('app.media.Mine1')))

      }
      .width(CommonConstants.FULL_PARENT)
      .backgroundColor(Color.White)
      .barHeight($r('app.float.mainPage_barHeight'))
      .barMode(BarMode.Fixed)
      .onChange((index: number) => {
        this.currentIndex = index;
      })
    }
  }
}