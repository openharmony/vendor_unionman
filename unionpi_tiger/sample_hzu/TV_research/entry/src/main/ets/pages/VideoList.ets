import router from '@ohos.router'

@Entry
@Component
export struct VideoList {
  build() {
    Column() {
      Image($r("app.media.fanhui"))
        .size({ width: 50, height: 50 })
        .onClick(() => {
          router.back()
        })


      Text('播放列表').fontSize(25).fontColor(Color.White).margin({ right: '80%' })
      Text("video001")
        .fontColor(Color.White)
        .padding(10)
        .border({width:1})
        .onClick(()=>{
          router.pushUrl({url:"pages/VideoPage",params:$r("app.media.video001")})
        }).margin(3)
      Text("video002")
        .fontColor(Color.White)
        .padding(10)
        .border({width:1})
        .onClick(()=>{
          router.pushUrl({url:"pages/VideoPage",params:$r("app.media.video002")})
        }).margin(3)
      Text("video003")
        .fontColor(Color.White)
        .padding(10)
        .border({width:1})
        .onClick(()=>{
          router.pushUrl({url:"pages/VideoPage",params:$r("app.media.video003")})
        }).margin(3)
      Text("video004")
        .fontColor(Color.White)
        .padding(10)
        .border({width:1})
        .onClick(()=>{
          router.pushUrl({url:"pages/VideoPage",params:$r("app.media.video004")})
        }).margin(3)
      Text("video005")
        .fontColor(Color.White)
        .padding(10)
        .border({width:1})
        .onClick(()=>{
          router.pushUrl({url:"pages/VideoPage",params:$r("app.media.video005")})
        }).margin(3)
      Text("video006")
        .fontColor(Color.White)
        .padding(10)
        .border({width:1})
        .onClick(()=>{
          router.pushUrl({url:"pages/VideoPage",params:$r("app.media.video006")})
        }).margin(3)
      Text("video007")
        .fontColor(Color.White)
        .padding(10)
        .border({width:1})
        .onClick(()=>{
          router.pushUrl({url:"pages/VideoPage",params:$r("app.media.video007")})
        }).margin(3)
      Text("video008")
        .fontColor(Color.White)
        .padding(10)
        .border({width:1})
        .onClick(()=>{
          router.pushUrl({url:"pages/VideoPage",params:$r("app.media.video008")})
        }).margin(3)

    }

      .backgroundImage($r('app.media.beijingtu'))
      .backgroundImageSize(ImageSize.Cover)


    .alignItems(HorizontalAlign.Start)
    .size({ width: '100%', height: '100%' })

  }
}