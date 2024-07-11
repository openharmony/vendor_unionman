
import router from '@ohos.router';

@Entry
@Component
struct PageVideo {
  @State videoSrc: Resource = $r('app.media.c1')
  @State previewUri: Resource =$r('app.media.h1')
  @State curRate: PlaybackSpeed = PlaybackSpeed.Speed_Forward_1_00_X
  @State isAutoPlay: boolean = false
  @State showControls: boolean = true
  controller: VideoController = new VideoController()


  build() {
    Column() {
      Video({
        src: this.videoSrc,
        previewUri: this.previewUri,
        currentProgressRate: this.curRate,
        controller: this.controller
      }).width('100%').height('80%')
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

      Row() {
        Button('切换来源').onClick(() => {

          if(this.videoSrc != $r('app.media.c1')) {
            this.previewUri = $r('app.media.h1')
            this.videoSrc = $r('app.media.c1')
          } else{
            this.previewUri = $r('app.media.h1')
            this.videoSrc = $r('app.media.c1')
          }// 切换视频源

        }).margin(20)

        Button('显示导航').onClick(() => {
          this.showControls = !this.showControls // 切换是否显示视频控制栏
        })
          .margin(20)
          .onClick(() => {
            console.info(`Succeeded in clicking the 'Next' button.`)
            try {
              router.pushUrl({ url: 'pages/EntryView' })
              console.info('Succeeded in jumping to the second page.')
            } catch (err) {
              console.error(`Failed to jump to the second page.Code is ${err.code}, message is ${err.message}`)
            }
          })
      }
      .height('10%')
      .width('100%')

      Row() {
        Button('开始').onClick(() => {
          this.controller.start() // 开始播放
        }).margin(20)
        Button('暂停').onClick(() => {
          this.controller.pause() // 暂停播放
        }).margin(20)
        Button('停止').onClick(() => {
          this.controller.stop() // 结束播放
        }).margin(20)
        Button('回到10秒播放').onClick(() => {
          this.controller.setCurrentTime(10, SeekMode.Accurate) // 精准跳转到视频的10s位置
        }).margin(20)
      }
      .height('10%')
      .width('100%')
    }
    .width('100%')
    .backgroundImage($r('app.media.h1'))
    .backgroundImageSize(1)
  }
}
