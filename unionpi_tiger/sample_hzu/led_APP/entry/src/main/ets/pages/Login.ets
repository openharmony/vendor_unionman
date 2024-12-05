import promptAction from '@ohos.promptAction';
import Constant from '../common/Constant'
import router from '@ohos.router';

@Entry
@Component
struct Login {
  @State username: string = ''
  @State password: string = ''
  @State flag: boolean = false

  build() {

    Column({ space: 20 }) {
      // Column({space: 10}){// this
      //本地图片,位于resource/*/media目录
      Image($r("app.media.img_avatar"))
        .width(100)
        .height(100)
      Text('登录界面')
        .fontSize(Constant.LOGIN_SIZE)
        .fontStyle(FontStyle.Normal)
        .fontWeight(FontWeight.Bold)
        .fontFamily('Arial')
      Text('登录账号可以使用更多服务')
        .fontSize(Constant.LOGIN_LITTLE_SIZE)
        .fontColor(Color.Gray)
        .fontWeight(FontWeight.Medium)
      // }// this

      Column({ space: 10 }) { // this
        TextInput({ placeholder: '请输入电子邮箱' })
          .maxLength(20)
          .margin({ top: 10 })
          .width(300)
          .height(60)
          .type(InputType.Email)
          .onChange((value: string) => {
            // 只允许英文字母、数字、下划线、英文句号、以及中划线组成
            let reg: RegExp = new RegExp('^[a-zA-Z0-9_-]+@[a-zA-Z0-9_-]+(\\.[a-zA-Z0-9_-]+)+');

            if (!reg.test(value)) {
              console.log("邮箱格式错误：" + value);
              this.username = '';
            }
            else
              this.username = value

          })
        TextInput({ placeholder: '请输入密码' })
          .margin({ top: 10 })
          .width(300)
          .height(60)
          .type(InputType.Password)
          .onChange((value: string) => {
            this.password = value
          })

        Row({ space: 200 }) {
          Text("短信验证码登录")
            .fontSize(13)
            .fontColor('#007DFF')
          Text("忘记密码")
            .fontSize(13)
            .fontColor('#007DFF')
        }
      } // this

      // Column({space:10}){// this
      Button($r('app.string.login_text'), { type: ButtonType.Capsule })
        .width($r('app.float.button_width'))
        .height($r('app.float.button_height'))
        .fontSize($r('app.float.login_fontSize'))
        .fontWeight(FontWeight.Medium)
        .backgroundColor($r('app.color.button_color'))
        .onClick(() => {
          AlertDialog.show(
            {
              title: '登录',
              message: '确认登录?',
              autoCancel: true, // 点击遮障层时，是否关闭弹窗。
              alignment: DialogAlignment.Bottom,
              offset: { dx: 0, dy: -20 },
              confirm: { //通过AlertDialog构建只包含一个操作按钮的确认弹窗，使用confirm响应点击确认时的回调。
                value: '确认',
                action: () => {
                  if (this.username === '') {
                    promptAction.showToast({ // 消息弹窗
                      message: "邮箱格式有误，请重新输入",
                      duration: 2000, // 停留时长,单位ms
                      bottom: 200 // 弹窗到底部的距离
                    });
                  } else if (this.password === '') {
                    promptAction.showToast({ // 消息弹窗
                      message: "密码有误",
                      duration: 2000, // 停留时长,单位ms
                      bottom: 200 // 弹窗到底部的距离
                    });
                  } else {  // 没用到数据库，模拟登录，邮箱格式正确，密码非空即视为登录成功
                    animateTo({ duration: 300 }, () => this.flag = true)
                    promptAction.showToast({ // 消息弹窗
                      message: "登录成功~ 您的电子邮箱为:" + this.username,
                      duration: 2000, // 停留时长,单位ms
                      bottom: 200 // 弹窗到底部的距离
                    });
                    // 登录成功跳转首页，同时将账号密码传参过去
                    router.pushUrl({
                      url: 'pages/Page',
                      params: {
                        username: this.username,
                        password: this.password
                      }
                    });
                  }
                }
              },
              cancel: () => { // 点击遮障层关闭dialog时的回调
                console.info('Closed callbacks')
              }
            })
        })
      Flex({justifyContent: FlexAlign.Center}){
        Text($r('app.string.register_text'))
        Text($r('app.string.register'))
          .fontSize($r('app.float.login_fontSize'))
          .fontColor($r('app.color.button_color'))
          .onClick(() => {
            router.pushUrl({
              url: 'pages/Register'
            })
          })
      }
      .margin(15)
      // }// this

      // 进度条
      if (this.flag) {
        LoadingProgress()
          .color(Color.Green)
          .height($r('app.float.LoadingProgress_height'))
          .width($r('app.float.LoadingProgress_width'))
          .transition({ type: TransitionType.All, opacity: 0 })// 透明度设置,配合animateTo使用
          .onClick(() => {
            animateTo({ duration: 300 }, () => this.flag = false)
          })
      }
      // Column({space:10}){   // this
      if (!this.flag) {
        Text("其它登录方式")
          .fontSize(13)
          .fontColor(Color.Gray)
        Row({ space: 30 }) {
          Button('方式1', { type: ButtonType.Circle })
            .width('90%')
            .height(60)
            .fontSize(10)
            .backgroundColor(Color.Gray)
          Button('方式2', { type: ButtonType.Circle })
            .width('90%')
            .height(60)
            .fontSize(10)
            .backgroundColor(Color.Gray)
          Button('方式3', { type: ButtonType.Circle })
            .width('90%')
            .height(60)
            .fontSize(10)
            .backgroundColor(Color.Gray)
        }
      }
      // }// this
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
  }
}
