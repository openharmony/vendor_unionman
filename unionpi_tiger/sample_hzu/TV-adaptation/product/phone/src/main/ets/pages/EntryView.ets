import prompt from '@ohos.prompt';
import bundle from '@ohos.bundle';
import innerBundleManager from '@ohos.bundle.innerBundleManager';
import account_osAccount from '@ohos.account.osAccount';
import { KeyCode } from '@ohos.multimodalInput.keyCode'
import StyleConstants from '../common/constants/StyleConstants';
import { AppItem } from '../common/Appitem'
import { localEventManager } from '@ohos/common';
import { EventConstants } from '@ohos/common';
import display from '@ohos.display';
import router from '@ohos.router';

@Entry
@Component
struct Index {
  @State message: string = 'Hello World'
  @State appLabel: Array<string> = new Array<string>();
  @State errorappLabel: Array<string> = new Array<string>();
  @State appIcon: Array<PixelMap> = new Array<PixelMap>();
  @State onKeyIndex: number = 0;
  @State pages: Array<number> = new Array<number>();

  //屏幕宽高
  private screenHeight;
  private screenWidth;

  //不显示的应用
  private noShowBundleName = ["com.ohos.adminprovisioning","com.ohos.contacts"]
  private noShowAbilityName = ["com.ohos.adminprovisioning.MainAbility","com.ohos.contacts.MainAbility"]
  private pageItem = new Array(12).fill(null).map((_, i) => i + 1);
  private deviceId: number = 100;
  private appItemInfos: Array<any> = new Array<any>();
  private useID: number = 100;
  private bundleName: Array<string> = new Array<string>();
  private abilityName: Array<string> = new Array<string>();
  private moduleName: Array<string> = new Array<string>();
  private appSize: number = 250;
  private swiperController: SwiperController = new SwiperController();
  private readonly mLauncherAbilityChangeListeners: any[] = [];
  private readonly mInstallationListener;
  private noToShowApp(bundleName,abilityName){
    for(let i=0;i<this.noShowBundleName.length;i++){
      if(bundleName == this.noShowBundleName[i]&&abilityName==this.noShowAbilityName[i]){
        return true;
      }
    }
    return false;
  };
  private readonly mBundleStatusCallback: any = {
    add: (bundleName, userId) => {
      this.updateScreenInfo()
    },
    remove: (bundleName, userId) => {

      this.updateScreenInfo()
    },
    update: (bundleName, userId) => {

      this.updateScreenInfo()
    }
  };

  onPageHide() {
    this.message = 'onPageHide'
  }

  onPageShow() {
    this.message = 'onPageShow'
  }
  private updateScreenInfo(): void {
    console.debug('updateScreenInfo');
    globalThis.SmartDockStyleConfig.initConfig();
  }
  async aboutToAppear() {

    //初始化useId
    const accountManager = account_osAccount.getAccountManager();
    await accountManager.getOsAccountLocalIdFromProcess().then((accountID) => {
      this.useID = accountID;
      console.log('getOsAccountLocalIdFromProcess, accountID: ' + accountID);
    }).catch((err) => {
      console.log("getOsAccountLocalIdFromProcess err: " + JSON.stringify(err));
    });
    //得到所有应用信息
    let abilityList = null;
    await innerBundleManager.getAllLauncherAbilityInfos(this.useID)
      .then((res) => {
        abilityList = res;

      })
      .catch((err) => {
        console.error(`getLauncherAbilityList error: ${JSON.stringify(err)}`);
      });
    if (abilityList == null) {
      console.debug("getLauncherAbilityList Empty");
    }

    for (let i = 0; i < abilityList.length; i++) {

      let bundleName = abilityList[i].elementName.bundleName
      let abilityName = abilityList[i].elementName.abilityName
      //判断是否显示app
      if(this.noToShowApp(bundleName,abilityName)){
        continue;
      }
      //初始化appLabel
      await bundle.getAbilityLabel(bundleName, abilityName)
        .then((data) => {
          this.bundleName.push(abilityList[i].elementName.bundleName)
          this.abilityName.push(abilityList[i].elementName.abilityName)
          this.moduleName.push(abilityList[i].elementName.moduleName)
          this.appLabel.push(data)
          console.info('Operation successful. Data: ' + JSON.stringify(data));
        }).catch((error) => {
          console.error('Operation failed. Cause: ' + JSON.stringify(error));
          this.errorappLabel.push("i:" + i + error)
        })

      //初始化appIcon
      await bundle.getAbilityIcon(bundleName, abilityName)
        .then((data) => {
          this.appIcon.push(data)
          console.info('Operation successful. Data: ' + JSON.stringify(data));
        }).catch((error) => {
          console.error('Operation failed. Cause: ' + JSON.stringify(error));
        })
    }
    //监控应用安装
    innerBundleManager.on("BundleStatusChange", this.mBundleStatusCallback).then(data => {
      console.info("XXX registerCallback success: " + JSON.stringify(data));
    }).catch(err => {
      console.error("XXX registerCallback fail:" + JSON.stringify(err));
    });
    //初始化页面数(0,12,24...)
    for (let i = 0; i < this.appIcon.length / 12; i++) {
      this.pages.push(i * 12)
    }
    //自适应appSize;
    display.getAllDisplay((err, data) => {
      this.screenHeight = data[0].height;
      this.screenWidth = data[0].width;
      this.appSize = this.screenWidth/8;
      if (err.code) {
        console.error('Failed to obtain all the display objects. Code: ' + JSON.stringify(err));
        return;
      }
      console.info('Succeeded in obtaining all the display objects. Data: ' + JSON.stringify(data));
    });

  }

  build() {

    Column() {
      Row(){
        Button( { type: ButtonType.Circle, stateEffect: false })
          .backgroundImage($r('app.media.b3'))
          .backgroundImageSize(ImageSize.Cover)
          .width('80%')
          .height('80%')
          .fontSize(80)
          .onClick(() => {
            console.info(`Succeeded in clicking the 'Next' button.`)
            try {
              router.pushUrl({ url: 'pages/Index1' })
              console.info('Succeeded in jumping to the second page.')
            } catch (err) {
              console.error(`Failed to jump to the second page.Code is ${err.code}, message is ${err.message}`)
            }
          })

        Button( { type: ButtonType.Circle, stateEffect: false })
          .backgroundImage($r('app.media.b2'))
          .backgroundImageSize(ImageSize.Cover)
          .width('80%')
          .height('80%')
          .fontSize(80)
          .fontColor('white')
          .onClick(() => {
            console.info(`Succeeded in clicking the 'Next' button.`)
            try {
              router.pushUrl({ url: 'pages/Index2' })
              console.info('Succeeded in jumping to the second page.')
            } catch (err) {
              console.error(`Failed to jump to the second page.Code is ${err.code}, message is ${err.message}`)
            }
          })
        Button( { type: ButtonType.Circle, stateEffect: false })
          .backgroundImage($r('app.media.b4'))
          .backgroundImageSize(ImageSize.Cover)
          .width('80%')
          .height('80%')
          .fontSize(80)
          .onClick(() => {
            console.info(`Succeeded in clicking the 'Next' button.`)
            try {
              router.pushUrl({ url: 'pages/Index3' })
              console.info('Succeeded in jumping to the second page.')
            } catch (err) {
              console.error(`Failed to jump to the second page.Code is ${err.code}, message is ${err.message}`)
            }
          })
      }
      .height('50%')
      Row(){
        Button({ type: ButtonType.Circle, stateEffect: false })
          .backgroundImage($r('app.media.b5'))
          .backgroundImageSize(ImageSize.Cover)
          .width('80%')
          .height('80%')
          .fontSize(80)
          .onClick(() => {
            console.info(`Succeeded in clicking the 'Next' button.`)
            try {
              router.pushUrl({ url: 'pages/Index4' })
              console.info('Succeeded in jumping to the second page.')
            } catch (err) {
              console.error(`Failed to jump to the second page.Code is ${err.code}, message is ${err.message}`)
            }
          })

        Button( { type: ButtonType.Circle, stateEffect: false })
          .backgroundImage($r('app.media.b7'))
          .backgroundImageSize(ImageSize.Cover)
          .width('80%')
          .height('80%')
          .fontSize(60)
          .fontColor('white')
          .onClick(() => {
            console.info(`Succeeded in clicking the 'Next' button.`)
            try {
              router.pushUrl({ url: 'pages/Index5' })
              console.info('Succeeded in jumping to the second page.')
            } catch (err) {
              console.error(`Failed to jump to the second page.Code is ${err.code}, message is ${err.message}`)
            }
          })
        Button({ type: ButtonType.Circle, stateEffect: false })
          .backgroundImage($r('app.media.b6'))
          .backgroundImageSize(ImageSize.Cover)
          .width('80%')
          .height('80%')
          .fontSize(80)
          .fontColor('white')

          .onClick(() => {
            console.info(`Succeeded in clicking the 'Next' button.`)
            try {
              router.pushUrl({ url: 'pages/Index6' })
              console.info('Succeeded in jumping to the second page.')
            } catch (err) {
              console.error(`Failed to jump to the second page.Code is ${err.code}, message is ${err.message}`)
            }
          })
      }
      .height('50%')
    }
    .width('100%')
    .backgroundImage($r('app.media.b1'))
    .backgroundImageSize(1)
  }

}