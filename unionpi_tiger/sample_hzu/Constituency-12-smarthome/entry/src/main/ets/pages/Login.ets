import promptAction from '@ohos.promptAction';
import router from '@ohos.router';
import  axios  from '@ohos/axios';

/*
  输入框的通用样式;用@Extend拓展原生组件,
 * 用法如@Extend(UIComponentName) function functionName { ... }
      //UIComponentName表示原生组件，如Text、Button...
      //functionName为样式函数名
 */

@Extend(TextInput) function inputStyle() {
  .placeholderColor($r('app.color.font_color'))
  .height(45)
  .fontSize(20)
  .backgroundColor('#00000000')
  .width('50%')
  .padding({ left: 0 })
  .margin({ top: 5 })
}

@Extend(Line) function lineStyle() {
  .width('50%')
  .height(3)
  .margin({ top : 30 })
  .backgroundColor(Color.Black)
}

@Extend(Text) function blueTextStyle() {
  .fontColor($r('app.color.login_blue_text_color'))
  .fontSize(20)
  .fontWeight(FontWeight.Medium)
}

@Entry
@Component
struct LoGin {
  @State account: string = '';
  @State password: string = '';
  @State account_find: string = '';
  @State password_find: string = '';

  //是否显示加载条
  @State isShowProgress: boolean = false;

  //超时标识
  private timeOutId: number = -1;

  /**
   * 登录判定
   */
  login(): void {
    if (this.account === '' || this.password === '') {
      //显示函数promptAction
      promptAction.showToast({ message: '账号或密码为空' })
      return
    }

    //内容不为空则显示加载进度条
    this.isShowProgress = true;

    //使用get到数据库中查询
    axios({
      method: "get",
      url: 'http://ip:端口/find/'+this.account+ '/' + this.password,
    }).then(res => {
      // console.info('result:' + JSON.stringify(res.data));
      console.info('result:' + JSON.stringify(res.data));

      promptAction.showToast({ message: '登录成功' })

      if (this.timeOutId === -1) {
        //设置超时处理，两秒后执行页面跳转到主页
        this.timeOutId = setTimeout(() => {
          this.isShowProgress = false;
          this.timeOutId = -1;
          router.pushUrl({
            url: 'pages/txt',
            params: {
              account: this.account
            }
          });
        }, 2000);
      }
    }).catch(error => {
      this.timeOutId = setTimeout(() => {
        this.isShowProgress = false;
        this.timeOutId = -1;
      },2000)
      promptAction.showToast({ message: '账号或密码错误' })
      console.error(error);
    })

  }

  /**
   * 组件的生命周期，组件销毁时执行
   */
  aboutToAppear() {
    console.log('aboutToAppear')
  }

  /**
   * 组件的生命周期，组件销毁时执行
   */
  aboutToDisappear() {
    console.log('aboutToDisappear')
    clearTimeout(this.timeOutId);
    this.timeOutId = -1;
  }

  build() {

    Column() {
      //Logo
      Image($r('app.media.home_selected'))
        .width(78)
        .height(78)
        .margin({ top: 5, bottom: 16 })
      Text('智能家居')
        .fontSize(20)
        .fontWeight(FontWeight.Medium)
        .fontColor($r('app.color.title_text_color'))

      Column({ space:20 }){

        //账号输入
        Stack(){
          TextInput({ placeholder: '请输入你的账号' ,text:this.account })
            .maxLength(11)
              //限定账号形式
            .type(InputType.Number)
            .inputStyle()
            .onChange((value: string) => {
              //获取输入的内容
              this.account = value;
            })

          //下划线
          Line()
            .lineStyle()

          if(this.account){
            Image($r('app.media.cancel'))
              .width(15)
              .height(20)
              .border({width: 1, radius: 15, color: '#fffffff'})
              .position({x: '68%', y: '50%'})
              .translate({y: '-50%'})
              .onClick(()=>{
                this.account = ''
              })
          }

        }.width('100%').height(45)

        //密码输入框
        Stack(){
          TextInput({ placeholder: '密码' ,text:this.password })
            .maxLength(15)
            .type(InputType.Password)
            .inputStyle()
            .onChange((value: string) => {
              //获取输入的内容
              this.password = value;
            })
          //下划线
          Line().lineStyle()

          if(this.password){
            Image($r('app.media.cancel'))
              .width(15)
              .height(20)
              .border({width: 1, radius: 15, color: '#fffffff'})
              .position({x: '68%', y: '50%'})
              .translate({y: '-50%'})
              .onClick(()=>{
                this.password = ''
              })
          }

        }.width('100%').height(45)

      }

      Row() {
        Text('短信验证码登录').blueTextStyle()
        Text('忘记密码').blueTextStyle()
          .onClick(()=>{
            router.pushUrl({
              url:'pages/ForgetAccount'
            })
          })
      }
      .justifyContent(FlexAlign.SpaceBetween)
      .width('50%')
      .margin({ top: 8 })

      //是否显示等待进度条
      if (this.isShowProgress) {
        //加载显示函数
        LoadingProgress()
          .color($r('app.color.loading_color'))
          .width(30)
          .height(30)
          .margin({ top:-30 })
      }

      //登录按钮
      Button('登录', { type: ButtonType.Capsule })
        .width('20%')
        .height(40)
        .fontSize(16)
        .fontWeight(FontWeight.Medium)
        .backgroundColor($r('app.color.login_button_color'))
        .margin({ top: 10, bottom: 20 })
        .onClick(() => {
          //登录
          this.login();
        })

      Row( {space: 80 } ){
        Text('注册账号')
          .fontSize(20)
          .fontWeight(FontWeight.Medium)
          .onClick(()=>{
            router.pushUrl({
              url:'pages/GetAccount'
            })
          })
          .fontColor($r('app.color.font_color'))
          .fontSize(20)
          .fontWeight(FontWeight.Medium)

        Text('其他登录方式')
          .fontColor(Color.Black)
          .fontSize(20)
          .fontWeight(FontWeight.Medium)
        Row({ space: 20 }) {
          this.imageButton($r('app.media.phone1'),null)
          this.imageButton($r('app.media.wechat'),null)
          this.imageButton($r('app.media.ixmn'),null)
        }
      }

    }
    .backgroundImage($r('app.media.backg'))
    .height('100%')
    .width('100%')
    .padding({
      left: 12,
      right: 12,
      bottom: 24
    })
  }

  /**
   * 其他登录方式按钮组件设计
   * @param src
   */
  @Builder
  imageButton(src: Resource , onclick: Function) {
    Button({ type: ButtonType.Circle, stateEffect: true }) {
      Image(src)
    }
    .onClick(()=>{

      onclick //等待事件设计

    })
    .height(35)
    .width(30)
    .backgroundColor($r('app.color.background'))
  }
}