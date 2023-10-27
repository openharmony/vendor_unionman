import UIAbility from '@ohos.app.ability.UIAbility';
import window from '@ohos.window';
import abilityAccessCtrl from '@ohos.abilityAccessCtrl';

export default class EntryAbility extends UIAbility {
  onCreate(want, launchParam) {
    if (want.parameters !== undefined) {
      AppStorage.SetOrCreate('isStage', want.parameters.isStage)
      AppStorage.SetOrCreate('deviceID', want.parameters.deviceID)
    }
    globalThis.context = this.context;
  }

  onWindowStageCreate(windowStage: window.WindowStage) {
    let AtManager = abilityAccessCtrl.createAtManager();
    AtManager.requestPermissionsFromUser(this.context, ["ohos.permission.DISTRIBUTED_DATASYNC", "ohos.permission.READ_MEDIA", "ohos.permission.WRITE_MEDIA"])
    windowStage.getMainWindowSync().setWindowSystemBarEnable([])

    windowStage.loadContent('pages/Index', null);
  }
}
