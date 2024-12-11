import AbilityConstant from '@ohos.app.ability.AbilityConstant';
import hilog from '@ohos.hilog';
import UIAbility from '@ohos.app.ability.UIAbility';
import Want from '@ohos.app.ability.Want';
import window from '@ohos.window';
import { GlobalThis } from '../utils/globalThis'
import DistributedDataObject from '@ohos.data.distributedDataObject';
import { BusinessError } from '@ohos.base';
import { Hero } from '../object/hero';


export class Source {
  mode?: number = 0
  hero?: Hero = new Hero()
  name?: string = ''
  status?: number = 0
  generate?: number = 0
  generateBig?: number = 0

  constructor() {
  }
}

export default class EntryAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {

    let sessionId: string = DistributedDataObject.genSessionId()
    let funcAbilityWant: Want = want
    console.info('want' + JSON.stringify(funcAbilityWant?.parameters))
    console.info('want' + JSON.stringify(funcAbilityWant?.parameters?.info))
    if (funcAbilityWant?.parameters?.info === 1) {

      GlobalThis.getInstance().setObject('remoteObject', DistributedDataObject.create(this.context, {
        name: undefined,
        status: 2,
        hero: undefined,
        generate: undefined,
        generateBig: undefined,
        mode: undefined,
      }))

      let object: DistributedDataObject.DataObject = GlobalThis.getInstance()
        .getObject('remoteObject') as DistributedDataObject.DataObject

      object.setSessionId(sessionId).then(() => {
        console.info('join session')
        console.info(JSON.stringify(object))
      }).catch((err: BusinessError) => {
        console.info(err.code + err.message)
      })

      object.on('change', (sessionId: string, changeData: string[]) => {
        console.info(JSON.stringify(object))
        console.info(`change: ${sessionId}`);
        if (changeData !== null && changeData !== undefined) {
          changeData.forEach(element => {

          })
        }
      })

    }

    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onCreate');
  }

  onDestroy(): void {
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onDestroy');
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    // Main window is created, set main page for this ability
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onWindowStageCreate');

    windowStage.loadContent('pages/Index', (err) => {
      if (err.code) {
        hilog.error(0x0000, 'testTag', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err) ?? '');
        return;
      }
      hilog.info(0x0000, 'testTag', 'Succeeded in loading the content.');
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
}
