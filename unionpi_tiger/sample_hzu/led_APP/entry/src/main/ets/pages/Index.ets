import userprivacydialog from '../dialog/userprivacydialog'
import router from '@ohos.router'
import common from '@ohos.app.ability.common'
import data_preferences from '@ohos.data.preferences';

const H: string = 'LED'
const I: string = 'isPrivacy'

@Entry
@Component
struct Index {
  context: common.UIAbilityContext = getContext(this) as common.UIAbilityContext;
  dialogController: CustomDialogController = new CustomDialogController({
    builder: userprivacydialog({
      cancel: () => {
        this.exitApp()
      },
      confirm: () => {
        this.jumpToMain()
      }

    })
  })

  onConfirm() {
    let preferences = data_preferences.getPreferences(this.context, H)
    preferences.then((res) => {
      res.put(I, true).then(() => {
        res.flush();
        this.jumpToMain()
      })
        .catch((err: Error) => {

        })
    })
  }

  exitApp() {
    this.context.terminateSelf()
  }

  aboutToDisappear() {
    clearTimeout()
  }

  aboutToAppear() {
    let preferences = data_preferences.getPreferences(this.context, H)

    this.dialogController.open()
    preferences.then((res) => {
      res.get(I, true).then((isprivate) => {
        if (I === 'isPrivacy') {
          this.dialogController.open()
          this.jumpToMain()
        }
        else {
          this.dialogController.open()
        }
      })
    })

  }

  jumpToMain() {
    setTimeout(() => {
      router.replaceUrl({ url: "pages/Login" })
    }, 2000)
  }

  build() {

    Column() {
      Image($r('app.media.app_icon'))
        .width(110)
        .margin({ top: 150 })
      Text("欢迎进入xxx灯光应用")
        .fontSize(15)
        .fontColor('#182431')
        .fontWeight(700)
        .letterSpacing(0.1)
        .opacity(0.6)
        .margin({ top: 20, bottom: 140 })

    }
    .width('100%')
    .height('100%')
    .backgroundImage($r('app.media.Hello'))
    .backgroundImageSize({ width: '100%', height: '100%' })
    .backgroundImagePosition({ x: 0, y: 0 })
  }
}