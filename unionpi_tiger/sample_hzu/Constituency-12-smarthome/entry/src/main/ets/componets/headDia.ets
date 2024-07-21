import router from '@ohos.router'
@Component
export struct Header{
  build(){
    //   标题部分
    Row({space:5}){
      Image($r('app.media.back1'))
        .width(30)
        .height(30)
        .onClick(() =>{
          router.back()
        })
      Blank()
      Image($r('app.media.flash'))
        .width(30)
        .height(30)
    }
    .width('98%')
    .height(30)
  }
}