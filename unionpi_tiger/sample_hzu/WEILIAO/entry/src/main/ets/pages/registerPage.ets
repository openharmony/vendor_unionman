/**
 * @author: Mr.Hong
 */
import router from '@ohos.router'
import promptAction from '@ohos.promptAction'
import AccountTable from '../common/database/tables/AccountTable'
import AccountData from '../viewmodel/AccountData';
import CommonConstants from '../common/constants/CommonConstants';

@Entry
@Component
struct RegisterPage {
  @State account: string = ''
  @State numCheck: string = 'CLICK'
  @State tInumGet: string = ''
  @State arr1: string[] = ['账号：', '请输入账号', '200vp']
  @State arr2: string[] = ['密码：', '请输入密码', '200vp']
  @State arr3: string[] = ['确认密码：', '请再次输入密码', '200vp']
  @State arr4: string[] = ['人机验证：', '验证码', '100vp']
  //数据库初始化
  private accountTable = new AccountTable(() => {
  })
  //向数据库中存放新用户
  createUser = () => {

    const newUser: AccountData = { id: 0, account: this.arr1[1], password: this.arr2[1] }
    this.accountTable.insertData(newUser, (id: number) => {
      newUser.id = id
    })
  }

  build() {
    Navigation() {
      Column({ space: 10 }) {
        Image($r('app.media.icon'))
          .width($r('app.float.registerPageIconSize'))
          .height($r('app.float.registerPageIconSize'))
        RowComponent({ items: $arr1, inputType: InputType.Number })
        RowComponent({ items: $arr2, inputType: InputType.Password })
        RowComponent({ items: $arr3, inputType: InputType.Password })
        Row({ space: 10 }) {
          RowComponent({ items: $arr4, inputType: InputType.Number })
          Text(this.numCheck)
            .fontSize($r('app.float.registerPageTextFontSize'))
            .fontWeight(FontWeight.Bold)
            .width($r('app.float.registerPageTextWidth'))
            .height($r('app.float.registerPageTextHeight'))
            .backgroundColor(Color.Pink)
            .textAlign(TextAlign.Center)
            .borderRadius($r('app.float.registerPageTextBorderRadius'))
            .onClick(() => {
              this.numCheck = Math.floor((Math.random() * 8999) + 1000).toString()
            })
        }

        Button($r('app.string.register'))
          .fontSize($r('app.float.registerPageButtonFontSize'))
          .backgroundColor($r('app.color.register'))
          .onClick(() => {
            //判断注册信息的完整和正确性和人机验证是否正确
            if (this.arr1[1] != '请输入账号' && this.arr2[1] != '请输入密码' && this.arr3[1] != '请再次输入密码') {
              if (this.arr2[1] == this.arr3[1]) {
                if (this.numCheck == this.arr4[1]) {

                  this.createUser()
                  promptAction.showToast({
                    message: '注册成功！',
                    duration: 1000
                  })
                  router.pushUrl({
                    url: 'pages/Index'
                  }, router.RouterMode.Single)
                } else
                  promptAction.showToast({
                    message: '来个人填写',
                    duration: 1000
                  })
              } else
                promptAction.showToast({
                  message: '两次密码不一致',
                  duration: 1000
                })
            } else
              promptAction.showToast({
                message: '请填写完整信息',
                duration: 1000
              })
          })
          .width($r('app.float.registerPageButtonWidth'))
          .height($r('app.float.registerPageButtonHeight'))
      }
      .width(CommonConstants.FULL_PARENT)
      .height(CommonConstants.FULL_PARENT)
      .backgroundColor($r('app.color.registerPageBG'))
      .justifyContent(FlexAlign.Center)
    }
    .title($r('app.string.register'))
    .titleMode(NavigationTitleMode.Mini)
    .hideTitleBar(false)
    .navBarWidth(CommonConstants.FULL_PARENT)
  }
}

@Component
struct RowComponent {
  @Link items: string[];
  private inputType: InputType = InputType.Normal

  build() {
    Row() {
      Text(this.items[0])
        .height($r('app.float.rowComponentTextHeight'))
        .fontSize($r('app.float.rowComponentTextFontSize'))
      TextInput({ placeholder: this.items[1] })
        .type(this.inputType)
        .fontSize($r('app.float.rowComponentTextFontSize'))
        .width(this.items[2])
        .height($r('app.float.rowComponentInputHeight'))
        .onChange((value: string) => {
          this.items[1] = value
        })
    }
    .justifyContent(FlexAlign.Center)
  }
}