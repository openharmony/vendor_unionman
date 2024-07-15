/**
 * @author: Mr.Hong
 */
import router from '@ohos.router'
import promptAction from '@ohos.promptAction'
import AccountTable from '../common/database/tables/AccountTable'
import AccountData from '../viewmodel/AccountData'
import CommonConstants from '../common/constants/CommonConstants'

@Entry
@Component
struct Index {
  @State account: string = ''
  @State passwork: string = ''
  @State agreeCheck: Boolean = false
  @State tmp: AccountData = { id: 0, account: '', password: '' }
  //数据库初始化
  private accountTable = new AccountTable(() => {
  })
  //向数据库中查找用户
  queryUser = () => {
    const newUser: AccountData = { id: 0, account: this.account, password: this.passwork }
    this.accountTable.query(newUser, (result: AccountData) => {
      this.tmp = result
    })
  }

  //暂定项目软件名称----‘微聊’----WEILIAO
  build() {
    Stack() {
      Image($r('app.media.weiliao'))
        .width("100%")
        .objectFit(ImageFit.Cover)
      Column({ "space": 10 }) {
        Text($r('app.string.OpenHarmony'))
          .fontColor(Color.Pink)
          .fontSize(35)
          .fontStyle(FontStyle.Italic)
          .fontWeight(FontWeight.Bold)
          .fontFamily('Arial')
        Text($r('app.string.WEILIAO'))
          .fontColor(Color.White)
        TextInput({ placeholder: $r('app.string.Get_account_number') })
          .onChange((value: string) => {
            this.account = value
          })
          .fontColor(Color.White)
          .type(InputType.Number)
          .opacity($r('app.float.transparency_0_8'))
          .backgroundColor(Color.Gray)
          .width($r('app.float.textInputOfWidth'))
          .height($r('app.float.textInputOfHeight'))
        TextInput({ placeholder: $r('app.string.Get_password') })
          .onChange((value: string) => {
            this.passwork = value
          })
          .fontColor(Color.Black)
          .type(InputType.Password)
          .opacity($r('app.float.transparency_0_8'))
          .backgroundColor(Color.Gray)
          .width($r('app.float.textInputOfWidth'))
          .height($r('app.float.textInputOfHeight'))
        Row({ "space": 15 }) {
          Button() {
            Text($r('app.string.login'))
          }
          .width($r('app.float.indexButtonWidth'))
          .height($r('app.float.indexButtonHeight'))
          .onClick(() => {
            if (this.agreeCheck == false) {
              promptAction.showToast({
                message: '请阅读并同意《用户需知》',
                duration: 1000
              })
              AlertDialog.show(
                {
                  title: '请阅读并同意用户需知',
                  message: '《用户需知》',
                  autoCancel: true,
                  alignment: DialogAlignment.Center,
                  gridCount: 4,
                  offset: { dx: 0, dy: -20 },
                  buttonDirection: DialogButtonDirection.HORIZONTAL,
                  buttons: [
                    {
                      value: '阅读',
                      action: () => {
                        router.pushUrl({ url: 'pages/agreement' }, router.RouterMode.Single)
                      }
                    },
                    {
                      value: '同意',
                      action: () => {
                        this.agreeCheck = true
                      }
                    },
                    {
                      value: '取消',
                      action: () => {
                      }
                    },
                  ],
                  cancel: () => {
                    console.info('Closed callbacks')
                  }
                }
              )
            } else {
              this.queryUser() //查询是否存在

              if (this.tmp.account == this.account && this.tmp.password == this.passwork) {
                this.tmp.account = ''
                this.tmp.password = ''
                router.pushUrl({ url: 'pages/MainInterface' }, router.RouterMode.Single)
              } else {
                promptAction.showToast({
                  message: $r('app.string.Error_login'),
                  duration: 1000
                })
              }

            }
          })

          Button() {
            Text($r('app.string.register'))
          }
          .width($r('app.float.indexButtonWidth'))
          .height($r('app.float.indexButtonHeight'))
          .onClick(() => {
            router.pushUrl({ url: 'pages/registerPage' }, router.RouterMode.Single)
          })
        }
        .justifyContent(FlexAlign.Center)

        //用户需知协议跳转栏，暂不做选定判断和跳转详情
        Row() {
          Checkbox({ name: 'userKnow' })
            .select(this.agreeCheck.valueOf())
            .onClick(() => {
              if (this.agreeCheck == false) {
                this.agreeCheck = true
                promptAction.showToast({
                  message: '已同意《用户需知》',
                  duration: 1000
                })
              } else {
                this.agreeCheck = false
                promptAction.showToast({
                  message: '取消同意《用户需知》',
                  duration: 1000
                })
              }
            })
          Text('《用户需知》')
            .fontSize($r('app.float.userKnowFontSize'))
            .fontColor($r('app.color.userKnow'))
            .margin({ right: $r('app.float.userKnowRight') })
            .fontStyle(FontStyle.Italic)
            .onClick(() => {
              router.pushUrl({ url: 'pages/agreement' }, router.RouterMode.Single)
            })
        }
        // .margin({top: "10%"})
        .justifyContent(FlexAlign.End)
      }
      .width(CommonConstants.FULL_PARENT)
      .height(CommonConstants.FULL_PARENT)
      .padding({ top: '0%' })
      .justifyContent(FlexAlign.Center)
    }
    .width(CommonConstants.FULL_PARENT)
    .height(CommonConstants.FULL_PARENT)
  }
}