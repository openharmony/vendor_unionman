import StaticSubscriberExtensionAbility from '@ohos.application.StaticSubscriberExtensionAbility'
import Logger from '../components/Logger'
import ServiceModel from '../components/ServiceModel'

export class StaticSubscriber extends StaticSubscriberExtensionAbility {
  private serviceModel = new ServiceModel(globalThis.serviceExtensionContext);

  onReceiveEvent(event) {
    Logger.info(`onReceiveEvent, event:  ${event.event}`);
    this.serviceModel.startServiceExtAbility(function (code) {
      if (code === 1) {
        Logger.info('服务启动成功');
      } else {
        Logger.info('服务启动失败');
      }
    })
  }
}