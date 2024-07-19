import router from '@ohos.router';
import common from '@ohos.app.ability.common';
import { TitleBar } from '../common/TitleBar';
import { KvStoreModel } from '../model/KvStoreModel';
import { RemoteDeviceModel } from '../model/RemoteDeviceModel';
import abilityAccessCtrl from '@ohos.abilityAccessCtrl';
import Logger from '../util/Logger';
import Want from '@ohos.app.ability.Want';

const TAG = 'Index' // 日志标签
const NOTES_CHANGE = 'notesChange' // 笔记变更事件
const EXIT = 'exit' // 退出事件
let kvStoreModel: KvStoreModel = new KvStoreModel() // 实例化键值存储模型

@Entry
@Component
struct Index {
  @State isDistributed: boolean = false // 是否分布式
  private remoteDeviceModel: RemoteDeviceModel = new RemoteDeviceModel() // 远程设备模型

  aboutToAppear() {   // 获取上下文和权限管理器
    let context = getContext(this) as common.UIAbilityContext
    let atManager = abilityAccessCtrl.createAtManager()
    try {    // 请求分布式数据同步权限
      atManager.requestPermissionsFromUser(context,['ohos.permission.DISTRIBUTED_DATASYNC']).then((data) => {
        Logger.info(TAG, `data: ${JSON.stringify(data)}`) // 日志记录请求结果
      }).catch((err: object) => {
        Logger.info(TAG, `err: ${JSON.stringify(err)}`) // 日志记录错误
      })
    } catch (err) {
      Logger.info(TAG, `catch err->${JSON.stringify(err)}`); // 日志记录异常
    }
    let want = JSON.parse(AppStorage.Get('wantMsg')) as Want;  // 解析启动参数
    Logger.info(TAG,`getWant =${JSON.stringify(want)}`);
    if(want.parameters != undefined) {     // 检查是否处于分布式阶段
      if (want.parameters.isStage === 'Stage') {
        this.isDistributed = true
      }
    }
    kvStoreModel.setOnMessageReceivedListener(NOTES_CHANGE , (value) => {  // 设置键值存储消息监听器
      Logger.info(TAG, `NOTES_CHANGE${value}`);  // 记录日志，显示接收到NOTES_CHANGE消息的值
      if (this.isDistributed) {  // 在分布式环境下处理笔记变更事件
        if (value.search(EXIT) !== -1) {  // 如果接收到的值中包含EXIT字符串
          Logger.info(TAG, `[json]EXIT${EXIT}`);  // 记录日志，显示检测到EXIT关键词
          context.terminateSelf((error) => {  // 调用context对象的terminateSelf方法，结束当前进程，并处理可能的错误
            Logger.info(TAG, `terminateSelf finished, error=${error}`);
          });
        } else { }
      }
    })
  }
  startAbilityCallBack = (key: string) => {  // 启动能力回调函数
    Logger.info(TAG,`startAbilityCallBack${key}`);
    if (NOTES_CHANGE === key) {  // 根据事件类型更新键值存储
    }
    if (EXIT === key) {
      kvStoreModel.put(NOTES_CHANGE, EXIT)
    }
  }

  build() {
    Column() {
      TitleBar({
        startAbilityCallBack: this.startAbilityCallBack,
        remoteDeviceModel: this.remoteDeviceModel,
        isDistributed: $isDistributed
      })
      Row() {
        Column()
        {
          Text('Welcome')
            .fontColor('#545454')
            .fontSize(36)
            .fontWeight(FontWeight.Medium)
            .fontStyle(FontStyle.Italic)
            .margin({left: 20})
          Text('控制面板')
            .fontColor('#545454')
            .fontSize(34)
            .fontWeight(FontWeight.Bold)
            .margin({top: 15, left: 70})
        }
        Image($r('app.media.SmartHome'))
          .height(64)
          .width(64)
          .margin({left: 50, right: 40})
      }
      .margin({top: 20})
      Row() {
        Text('设备')
          .fontColor('#474747')
          .fontSize(20)
          .fontWeight(FontWeight.Medium)
        Text('Device')
          .fontColor('#BABABA')
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .margin({left: 10})
      }
      .margin({top: 50, left: 20})
      Row() {
        Column() {
          Row() {
            Image($r('app.media.temperature'))
              .height(40)
              .width(40)
            Image($r('app.media.humidity'))
              .height(40)
              .width(40)
              .margin({left: 10})
          }
          .margin({top: 15})
          Text('温度 / 湿度')
            .fontColor('#474747')
            .fontSize(20)
            .fontWeight(FontWeight.Medium)
            .margin({top: 20})
        }
        .backgroundColor('#FAFAFA')
        .margin({left: 20, top: 40})
        .borderRadius(12)
        .height(105)
        .width(165)
        .onClick(() => {
          router.pushUrl({url: 'pages/Tem_Hum'})
        })
        Column() {
          Row() {
            Image($r('app.media.lamp'))
              .height(40)
              .width(40)
          }
          .margin({top: 15})
          Text('灯光')
            .fontColor('#474747')
            .fontSize(20)
            .fontWeight(FontWeight.Medium)
            .margin({top: 20})
        }
        .backgroundColor('#FAFAFA')
        .margin({left: 20, top: 40})
        .borderRadius(12)
        .height(105)
        .width(165)
        .onClick(() => {
          router.pushUrl({url: 'pages/Lamp'})
        })
        Column() {
          Row() {
            Image($r('app.media.camera'))
              .height(40)
              .width(40)
          }
          .margin({top: 15})
          Text('摄像头')
            .fontColor('#474747')
            .fontSize(20)
            .fontWeight(FontWeight.Medium)
            .margin({top: 20})
        }
        .backgroundColor('#FAFAFA')
        .margin({left: 20, top: 40})
        .borderRadius(12)
        .height(105)
        .width(165)
        .onClick(() => {
          router.pushUrl({url: 'pages/camera'})
        })
      }
    }
    .alignItems(HorizontalAlign.Start)
    .height('100%')
    .width('100%')
    .backgroundColor('#F2F2F2')
  }
}