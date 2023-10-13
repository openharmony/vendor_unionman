import AbilityStage from '@ohos.app.ability.AbilityStage'
import logger from '../Model/Logger'

const TAG: string = 'UM_BLE MyAbilityStage'

export default class MyAbilityStage extends AbilityStage {
    onCreate() {
        logger.info(TAG, `[Demo] MyAbilityStage onCreate`)
    }
}