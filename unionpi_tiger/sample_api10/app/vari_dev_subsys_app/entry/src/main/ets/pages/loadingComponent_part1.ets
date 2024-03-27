//loadingComponent_part1.ets
@Component

export default struct loadingComponent_part1 {
  private imageWidth: number | Resource = 0
  private imageHeight: number | Resource = 0
  private images :  ImageFrameInfo[]=[]
  private imagePath : string='/images_black//4337e81bd5784c21984de88f01c5e30e.gif_p'
  @State textContent: string="LOADING"
  private text_index: number =0
  aboutToAppear(){
    for (let i = 0; i <=44; i++) {
      this.images.push(
        {src:this.imagePath+i+".png", duration:100}
      )
    }

    setInterval(() => {
      this.text_index++
      if (this.text_index>6) {
        this.textContent='LOADING'
        this.text_index=0
      }else{
        this.textContent += "~"
      }
    }, 200)
  }

  build() {
    Column() {
      ImageAnimator()
        .images(this.images)
        .size({width: "50%", height: '80%'})

        .iterations(-1)
          //	设置播放顺序。false表示从第1张图片播放到最后1张图片； true表示从最后1张图片播放到第1张图片。

        .fixedSize(true)
          //设置图片大小是否固定为组件大小。 true表示图片大小与组件大小一致，此时设置图片的width 、height 、top 和left属性是无效的。false表示每一张图片的width 、height 、top和left属性都要单独设置。

        .reverse(true)
          //设置播放顺序。false表示从第1张图片播放到最后1张图片； true表示从最后1张图片播放到第1张图片。

        .fillMode(FillMode.None)
          //设置动画开始前和结束后的状态，可选值参见FillMode说明

        .state(AnimationStatus.Running)
      //Running表示动画处于播放状态

      Text(this.textContent).fontSize(30)
    }
  }
}