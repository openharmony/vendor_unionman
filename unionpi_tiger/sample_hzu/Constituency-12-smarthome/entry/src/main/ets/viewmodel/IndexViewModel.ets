
import ItemData from '../bean/ItemData';

/**
 * 将数据绑定到组件并提供接口
 */
export class IndexViewModel {

  /**
   * 获取设置列表数据
   */
  getSettingListData(): Array<ItemData> {
    let settingListData: ItemData[] = [
      new ItemData('推送通知', $r('app.media.news'),'开关'),
      new ItemData('数据管理', $r('app.media.data'), null),
      new ItemData('菜单设置', $r('app.media.menu'), null),
      new ItemData('关于', $r('app.media.about'), null),
      new ItemData('清除缓存', $r('app.media.storage'), null),
      new ItemData('用户协议', $r('app.media.privacy'), null)
    ]
    return settingListData
  }
}

export default new IndexViewModel()