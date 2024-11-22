import AbilityConstant from '@ohos.app.ability.AbilityConstant';
import hilog from '@ohos.hilog';
import deviceInfo from '@ohos.deviceInfo';
import abilityAccessCtrl from '@ohos.abilityAccessCtrl';

import UIAbility from '@ohos.app.ability.UIAbility';
import Want from '@ohos.app.ability.Want';
import window from '@ohos.window';
import Logger from '../model/Logger';
import { Constants } from '../common/Constants';

const TAG: string = 'EntryAbility';
export default class EntryAbility extends Ability {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onCreate');
    globalThis.abilityContext = this.context;

  }

  onDestroy(): void {
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onDestroy');
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    // Main window is created, set main page for this ability
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onWindowStageCreate');
    this.requestPermissionsFn();
    AppStorage.SetOrCreate<string>('deviceType', deviceInfo.deviceType);
    if (deviceInfo.deviceType === Constants.TABLET) {
      windowStage.getMainWindow().then((win: window.Window): void => {
        win.setLayoutFullScreen(true).then((): void => {
          win.setSystemBarEnable(['navigation']).then((): void => {
          });
        });
        win.setSystemBarProperties({
          navigationBarColor: '#00000000',
          navigationBarContentColor: '#B3B3B3'
        }).then((): void => {
        });
      });
    }

    windowStage.loadContent('pages/Index', (err, data) => {
      if (err.code) {
        hilog.error(0x0000, 'testTag', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err) ?? '');
        return;
      }
      hilog.info(0x0000, 'testTag', 'Succeeded in loading the content. Data: %{public}s', JSON.stringify(data) ?? '');
    });
  }

  onWindowStageDestroy(): void {
    // Main window is destroyed, release UI related resources
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onWindowStageDestroy');
  }

  onForeground(): void {
    // Ability has brought to foreground
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onForeground');
  }

  onBackground(): void {
    // Ability has back to background
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onBackground');
  }

  requestPermissionsFn(): void {
    let atManager = abilityAccessCtrl.createAtManager();
    atManager.requestPermissionsFromUser(globalThis.abilityContext, [
      'ohos.permission.CAMERA',
      'ohos.permission.MICROPHONE',
      'ohos.permission.READ_MEDIA',
      'ohos.permission.WRITE_MEDIA'
    ]).then((): void => {
      AppStorage.SetOrCreate<boolean>('isShow', true);
      Logger.info(TAG, 'request Permissions success!');
    }).catch((error: {code: number}): void => {
      Logger.info(TAG, `requestPermissionsFromUser call Failed! error: ${error.code}`);
    });
  }

}
