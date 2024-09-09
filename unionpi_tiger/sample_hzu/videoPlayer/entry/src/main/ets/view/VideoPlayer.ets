
import prompt from '@ohos.promptAction';
import { changeSliderTime } from '../model/VideoController';
import { VideoSlider } from './VideoPlaySlider';
/**
 *  视频控制组件
 */
@Component
export struct VidePlayer{
  private source: string | Resource = '';
  private controller: VideoController = new VideoController();
  private previewUris: Resource = $r('app.media.preview');
  @Provide currentTime: number = 0;
  @Provide durationTime: number = 0;
  @Provide durationStringTime: string = '00:00';
  @Provide currentStringTime: string = '00:00';
  @Consume isPlay: boolean;
  @Consume isOpacity: boolean;
  @Consume flag: boolean;
  @Consume isLoading: boolean;
  @Consume progressVal: boolean;

  build() {
    Column() {
      Video({
        src: this.source,
        previewUri: this.previewUris,
        controller: this.controller
      })
        .width('100%')
        .height('83%')
        .controls(false)
        .autoPlay(false)
        .objectFit(ImageFit.Contain)
        .loop(false)
        .onUpdate((event) => {
          if(event) {
            this.currentTime = event.time
            this.currentStringTime = changeSliderTime(this.currentTime)
          }
        })
        .onPrepared((event) => {
          this.prepared(event?.duration)
        })
        .onFinish(() => {
          this.finish()
        })
        .onError(() => {
          prompt.showToast({
            duration: 5000,
            message: '请检查网络'
          });
        })
      VideoSlider({ controller: this.controller})
    }
  }

  /**
   *视频组件准备回调
   */
  prepared(duration: number) {
    this.durationTime = duration;
    let second: number = duration % 60;
    let min: number = Number.parseInt((duration / 60).toString());
    let head = min < 10 ? `${0}${min}`: min;
    let end = second < 10 ? `${0}${second}`: second;
    this.durationStringTime = `${head}:${end}`;
    this.flag = true
  }

  /**
   * 视频组件完成回调
   */
  finish() {
    this.isPlay = false;
    this.isOpacity = false;
  }
}