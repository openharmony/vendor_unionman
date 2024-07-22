import { Header } from '../componets/headDia'
import  axios  from '@ohos/axios'
import router from '@ohos.router'
import promptAction from '@ohos.promptAction';

@Extend(Line) function lineStyle()
{.width('92%').margin({ top : 30 }).backgroundColor(Color.Black)}

@Entry
@Component
struct Index {
  // 上传数据
  @State account: string = ''
  @State setpwd: string = ''
  @State pwd: string = ''

  //是否显示加载条
  @State isShowProgress: boolean = false;

  //超时标识
  private timeOutId: number = -1;

  build() {
    Column() {
      Header()
        .margin(20)

      Stack(){

        TextInput({ placeholder:'原账号' , text:this.account })
          .type(InputType.Number)
          .margin(20)
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
          Image($r('app.media.cancel'))
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

        TextInput({ placeholder:'原密码' , text:this.pwd })
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

      Stack(){

        TextInput({ placeholder:'新密码' , text:this.setpwd })
          .type(InputType.Password)
          .margin({ left:20,right:20,bottom:20 })
          .height(50)
          .onChange(value =>{
            console.log(value)
            this.setpwd = value
          })
          .backgroundColor('#36D2')

        //下划线
        Line()
          .lineStyle()

        if(this.setpwd){
          Image($r('app.media.cancel'))
            .width(20)
            .height(20)
            .border({width: 2, radius: 20, color: '#fffffff'})
            .position({x: '90%', y: '7%'})
            .translate({y: '-50%'})
            .onClick(()=>{
              this.setpwd = ''
            })
        }
      }

      //是否显示等待进度条
      if (this.isShowProgress) {
        //加载显示函数
        LoadingProgress()
          .color($r('app.color.loading_color'))
          .width(30)
          .height(30)
          .margin({ top:-30 })
      }

      Button('修改密码')
        .width(200)
        .onClick(()=>{

          if (this.account === '' || this.pwd === '' ) {
            //显示函数promptAction
            promptAction.showToast({ message: '账号或密码不能为空' })
            return
          }

          axios({
            method: 'post',
            url: 'http://ip:端口/upd',
            data:{
              account:this.account,
              pwd:this.pwd,
              newpwd:this.setpwd
            },
          }).then(res => {
            this.isShowProgress = true
            console.info('result:' + JSON.stringify(res.data));
            {
              promptAction.showToast({ message: '密码修改成功，请重新登录' })
              if (this.timeOutId === -1) {
                this.timeOutId = setTimeout(() => {
                  this.isShowProgress = false;
                  this.timeOutId = -1;
                  router.pushUrl({
                    url: 'pages/Login',
                  })
                }, 2000);
              }
            }
          }).catch(error => {
            promptAction.showToast({ message: '账号或密码错误' })
            console.error(error);
          })
        })
    }
    .backgroundImage($r('app.media.backg'))
    .width('100%')
    .height('100%')
  }
}