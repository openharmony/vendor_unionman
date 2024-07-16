import router from '@ohos.router'

@Entry
@Component
struct VideoPage {
  @State videoSrc: Resource = $r('app.media.video003')
  @State previewUri: Resource = $r('app.media.video001')
  @State curRate: PlaybackSpeed = PlaybackSpeed.Speed_Forward_1_00_X
  @State isAutoPlay: boolean = true
  @State showControls: boolean = true
  controller: VideoController = new VideoController()

  aboutToAppear() {
    this.videoSrc = router.getParams() as Resource
  }

  build() {
    Column() {
      Stack({ alignContent: Alignment.TopStart }) {
        Video({
          src: this.videoSrc,
          previewUri: this.previewUri,
          currentProgressRate: this.curRate,
          controller: this.controller
        })
          .width('100%')
          .height(450)
          .autoPlay(this.isAutoPlay)
          .controls(this.showControls)
          .onStart(() => {
            console.info('onStart')
          })
          .onPause(() => {
            console.info('onPause')
          })
          .onFinish(() => {
            console.info('onFinish')
          })
          .onError(() => {
            console.info('onError')
          })
          .onPrepared((e) => {
            console.info('onPrepared is ' + e.duration)
          })
          .onSeeking((e) => {
            console.info('onSeeking is ' + e.time)
          })
          .onSeeked((e) => {
            console.info('onSeeked is ' + e.time)
          })
          .onUpdate((e) => {
            console.info('onUpdate is ' + e.time)
          })
        Image($r("app.media.fanhui"))
          .onClick(() => {
            router.back()
          })
          .position({ x: 10, y: 10 })
          .size({ width: 32, height: 36 })
      }

      Row() {
        Button('src').onClick(() => {
          this.videoSrc = $r('app.media.video005')

        }).margin(0.2)
        Button('controls').onClick(() => {
          this.showControls = !this.showControls // 切换是否显示视频控制栏
        }).margin(0.2)



        Button('start').onClick(() => {
          this.controller.start() // 开始播放
        }).margin(0.2)
        Button('pause').onClick(() => {
          this.controller.pause() // 暂停播放
        }).margin(0.2)
        Button('stop').onClick(() => {
          this.controller.stop() // 结束播放
        }).margin(0.2)


        Button('rate 0.75').onClick(() => {
          this.curRate = PlaybackSpeed.Speed_Forward_0_75_X // 0.75倍速播放
        }).margin(5)
        Button('rate 1').onClick(() => {
          this.curRate = PlaybackSpeed.Speed_Forward_1_00_X // 原倍速播放
        }).margin(5)
        Button('rate 2').onClick(() => {
          this.curRate = PlaybackSpeed.Speed_Forward_2_00_X // 2倍速播放
        }).margin(5)
      }
    }
  }
}