import router from '@ohos.router'
import Constant from '../common/Constant'

@Entry
@Component
struct Register {
  @State message: string = '请注册 ~'
  @State username: string = ''
  @State password: string = ''
  @State email: string = ''

  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize(Constant.LOGIN_SIZE)
          .fontStyle(FontStyle.Normal)
          .fontWeight(FontWeight.Bold)
          .fontFamily('Arial')
        TextInput({ placeholder: '用户名' })
          .margin({ top: 20 })
          .width(Constant.INPUT_WIDTH)
          .height(Constant.INPUT_HEIGHT)
          .onChange((value: string) => {
            this.username = value
          })
        TextInput({ placeholder: '电子邮箱' })
          .margin({ top: 20 })
          .width(Constant.INPUT_WIDTH)
          .height(Constant.INPUT_HEIGHT)
          .onChange((value: string) => {
            this.email = value
          })
        TextInput({ placeholder: '密码' })
          .type(InputType.Password)
          .margin({ top: 20 })
          .width(Constant.INPUT_WIDTH)
          .height(Constant.INPUT_HEIGHT)
          .onChange((value: string) => {
            this.password = value
          })
        Button('注册')
          .width(Constant.BUTTON_WIDTH)
          .height(Constant.BUTTON_HEIGHT)
          .margin({ top: 30 })
          .backgroundColor(Constant.BUTTON_COLOR)
          .onClick(() => {
            router.pushUrl({
              url: 'pages/Login',
              params: {
                username: this.username,
                password: this.password,
                email: this.email
              }
            })
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
