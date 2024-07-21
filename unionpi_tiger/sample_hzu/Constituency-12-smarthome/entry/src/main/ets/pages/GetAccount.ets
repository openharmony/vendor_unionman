import { Header } from '../componets/headDia'
import  axios  from '@ohos/axios'
import router from '@ohos.router'
import promptAction from '@ohos.promptAction';

@Extend(Line) function lineStyle() {
  .width('92%')
  .height(3)
  .margin({ top : 30 })
  // .position({x : '0' , y : '0' })
  .backgroundColor(Color.Black)
}

@Entry
@Component
struct Index {
  // 上传数据
  @State account: string = ''
  @State pwd: string = ''
  @State account_find:string =''
  @State pwd_find:string =''

  build() {
    Column() {
      Header()
        .margin(20)

      Stack(){

        TextInput({placeholder:'注册账号' , text: this.account })
          .margin(20)
          .fontColor($r('app.color.font_color'))
          .fontWeight(800)
          .height(50)
          .onChange(value =>{
            console.log(value)
            this.account = value
          })
          .backgroundColor('#36D2')

        //下划线
        Line()
          .lineStyle()

        if(this.account){
          Image($r("app.media.cancel"))
            .width(20)
            .height(20)
            .border({width: 2, radius: 20, color: '#fffffff'})
            .position({x: '90%', y: '13%'})
            .translate({y: '-50%'})
            .onClick(()=>{
              this.account = ''
            })
        }

      }

      Stack(){

        TextInput({placeholder:'注册密码' , text: this.pwd })
          .fontColor($r('app.color.font_color'))
          .fontWeight(800)
          .type(InputType.Password)
          .margin({ left:20,right:20,bottom:20 })
          .height(50)
          .onChange(value =>{
            console.log(value)
            this.pwd = value
          })
          .backgroundColor('#36D2')

        //下划线
        Line()
          .lineStyle()

        if(this.pwd){
          Image($r('app.media.cancel'))
            .width(20)
            .height(20)
            .border({width: 2, radius: 20, color: '#fffffff'})
            .position({x: '90%', y: '7%'})
            .translate({y: '-50%'})
            .onClick(()=>{
              this.pwd = ''
            })
        }

      }

      Button('注册并登录')
        .width(200)
        .onClick(()=>{

          if (this.account === '' || this.pwd === '') {
            //显示函数promptAction
            promptAction.showToast({ message: '账号或密码不能为空' })
            return
          }

          axios({
            method: "post",
            url: 'http://ip:端口/publish',
            data:{
              account:this.account,
              pwd:this.pwd
            },
          }).then(res => {

            promptAction.showToast({ message: '注册成功，请重新登录' })

            console.info('result:' + JSON.stringify(res.data));
            router.pushUrl({
              url: 'pages/Login',
            })
          }).catch(error => {
            console.error(error);
          })
        })
    }
    .backgroundImage($r('app.media.backg'))
    .width('100%')
    .height('100%')
  }
}