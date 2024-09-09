
/**
 * 视频滑块组件
 */
@Component
export struct VideoSlider{
  @Consume isOpacity: boolean;
  private controller: VideoController = new VideoController();
  @Consume currentStringTime: string;
  @Consume currentTime: number;
  @Consume durationTime: number;
  @Consume durationStringTime: string;
  @Consume isPlay: boolean;
  @Consume flag: boolean;
  @Consume isLoading: boolean;
  @Consume progressVal: number;

  build() {
    Row({ space: 12}) {
      Image(this.isPlay ? $r('app.media.ic_pause') : $r('app.media.ic_play'))
        .width(24)
        .height(24)
        .margin({ left: 12})
        .onClick(() => {
          this.iconOnClick();
        })
      Text(this.currentStringTime)
        .fontSize(16)
        .fontColor(Color.White)
        .margin({ left: 12})
      Slider({
        value: this.currentTime,
        min: 0,
        max: this.durationTime,
        step: 1,
        style: SliderStyle.OutSet
      })
        .blockColor('#FFFFFF')
        .width('46.7%')
        .trackColor(Color.Gray)
        .selectedColor('#FFFFFF')
        .showSteps(true)
        .showTips(true)
        .trackThickness(this.isOpacity ? 2 : 4)
        .onChange((value: number, mode: SliderChangeMode) => {
          this.sliderOnchange(value, mode);
        })
      Text(this.durationStringTime)
        .fontSize(16)
        .margin({ right: 12})
        .fontColor(Color.White)
    }
    .opacity(this.isOpacity ? 0.2 : 1)
    .width('100%')
    .alignItems(VerticalAlign.Center)
    .justifyContent(FlexAlign.Center)
  }

  /**
   * 图标点击回调
   */
  iconOnClick() {
    if(this.isPlay === true) {
      this.controller.pause()
      this.isPlay = false;
      this.isOpacity = false;
      return;
    }
    if(this.flag === true) {
      this.controller.start();
      this.isPlay = true;
      this.isOpacity = true;
    } else {
      this.isLoading = true;
      //  视频加载未完成，加载事件显示
      let intervalLoading = setInterval(() => {
        if(this.progressVal >= 100) {
          this.progressVal = 0
        } else {
          this.progressVal += 10
        }
      }, 100)
      //计划任务确定视频加载是否完成。
      let intervalFlag = setInterval(() => {
        if(this.flag === true) {
          this.controller.start();
          this.isPlay = true;
          this.isOpacity = true;
          this.isLoading = false;
          clearInterval(intervalFlag);
          clearInterval(intervalLoading);
        }
      }, 100)
    }
  }

  /**
   * 视频滑块组件
   */
  sliderOnchange(value: number, mode: SliderChangeMode) {
    this.currentTime = Number.parseInt(value.toString());
    this.controller.setCurrentTime(Number.parseInt(value.toString()), SeekMode.Accurate);
    if(mode === SliderChangeMode.Begin || mode === SliderChangeMode.Moving) {
      this.isOpacity = false;
    }
    if(mode === SliderChangeMode.End) {
      this.isOpacity = true;
    }
  }
}