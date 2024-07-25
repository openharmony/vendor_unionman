import router from '@ohos.router'
import '../compon/hea'
import { Phbzi, quanvideoSrc, sps1 } from '../compon/hea'

@Entry
@Component
struct VideoGuide1 {
  @State videoSrc: Resource = quanvideoSrc
  @State previewUri: string = 'common/videoIcon.png'
  @State curRate: PlaybackSpeed = PlaybackSpeed.Speed_Forward_1_00_X
  @State isAutoPlay: boolean = false
  @State showControls: boolean = true
  @State sliderStartTime: string = '';
  @State currentTime: number = 0;
  @State durationTime: number = 0;
  @State durationStringTime: string = '';
  @State sss: number = 0
  @State bf: string = ' 播放 '
  controller: VideoController = new VideoController()

  build() {
    Stack() {
      Video({
        src: this.videoSrc,
        previewUri: this.previewUri,
        currentProgressRate: this.curRate,
        controller: this.controller
      })
        .controls(false)
        .autoPlay(true)
        .onPrepared((event) => {
          if (event) {
            this.durationTime = event.duration
          }
        })
        .height('100%')
        .width('100%')
        .onUpdate((event) => {
          if (event) {
            this.currentTime = event.time
          }
        })
      Row() {
        Text(this.bf)
        Text(JSON.stringify(this.currentTime) + 's')


        Slider({
          value: this.currentTime,
          min: 0,
          max: this.durationTime
        })
          .onChange((value: number, mode: SliderChangeMode) => {
            this.controller.setCurrentTime(value);
          }).width('80%')
          .focusable(true)
          .onKeyEvent(
            (event?: KeyEvent) => {


              if (event) {
                if (event.keyCode === 2016) {
                  if (this.bf === ' 播放 ') {
                    this.controller.pause()
                    this.bf = ' 暂停 '
                  } else {
                    this.controller.start()
                    this.bf = ' 播放 '
                  }
                }


                if (event.keyCode === 2014) {
                  if (this.currentTime <= 15) {
                    this.currentTime = 0
                  } else {
                    this.currentTime -= 15
                  }
                  this.controller.setCurrentTime(this.currentTime)
                }

                if (event.keyCode === 2015) {
                  if ((this.durationTime - this.currentTime) <= 15) {
                    this.currentTime = this.durationTime
                  } else {
                    this.currentTime += 15
                  }
                  this.controller.setCurrentTime(this.currentTime)
                }

                if (event.keyCode === 2012) {
                  router.pushUrl({ url: 'pages/phb' })
                }

                if (event.keyCode === 2001) {
                  let ss: Phbzi = new Phbzi($r('app.media.icon'), this.videoSrc, '   视频', '   ？？？')
                  sps1.push(ss)
                }

              }
            }

          )
        Text(JSON.stringify(this.durationTime) + 's')
      }
      .opacity(0.8)
      .width('100%')
      .margin({ top: 330 })

    }
    .width('100%')
    .height('100%')
  }
}
