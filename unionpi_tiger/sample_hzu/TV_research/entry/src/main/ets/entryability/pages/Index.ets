@Entry
@Component
struct Index {

  build() {
    Row() {
      Column() {
        Row(){
          Text('2024年4月26日').fontSize(25).fontColor(Color.White).margin({left:'50%'})
          Text('星期五').fontSize(25).fontColor(Color.White).margin({right:'50%'})
          Text('17：35').fontSize(25).fontColor(Color.White)
        }
        Flex() {
          Text('首页').fontSize(30).padding({right:'4%'}).fontColor(Color.White)
          Text('电影').fontSize(30).padding({right:'4%'}).fontColor(Color.White)
          Text('电视剧').fontSize(30).padding({right:'4%'}).fontColor(Color.White)
          Text('动漫').fontSize(30).padding({right:'4%'}).fontColor(Color.White)
          Text('综艺').fontSize(30).padding({right:'4%'}).fontColor(Color.White)
          Text('少儿').fontSize(30).padding({right:'4%'}).fontColor(Color.White)
          Text('教育').fontSize(30).padding({right:'4%'}).fontColor(Color.White)
          Text('游戏').fontSize(30).padding({right:'8%'}).fontColor(Color.White)
          Text('更多').fontSize(30).fontColor(Color.White)
        }
        .flexGrow(1)
        .padding({ right:'6%',left:'6%',top:'2%' })
        Flex(){
          Image('./assets/1.png').height('60%').borderRadius(15).margin(2)
          Column() { // 上半部分布局
            Image('./assets/2.png').height('30%').borderRadius(15).margin(2)
            Image('./assets/3.png').height('30%').borderRadius(15).margin(2)
          }
          Column() { // 下半部分布局
            Image('./assets/5.png').height('30%').borderRadius(15).margin(2)
            Image('./assets/6.png').height('30%').borderRadius(15).margin(2)
          }
          Image('./assets/4.png').height('60%').borderRadius(15).margin(2)
        }
        .padding({ right:'8%',left:'8%',top:'4%' })
        Flex({ justifyContent: FlexAlign.SpaceAround }){
          Image('./assets/7.png').height('13%')
          Image('./assets/8.png').height('13%')
          Image('./assets/9.png').height('13%')
          Image('./assets/10.png').height('13%')
          Image('./assets/11.png').height('13%')
        }
        .padding({ right:'10%',left:'10%',top:'2%',bottom:'3%'})
      }
    }
    .padding({top:'2%'})
    .height('100%')
    .backgroundColor('#043ba0')
  }
}

// async avPlayerFdSrcDemo() {
//   // 创建avPlayer实例对象
//   let avPlayer: media.AVPlayer = await media.createAVPlayer();
//   // 创建状态机变化回调函数
//   this.setAVPlayerCallback(avPlayer);
//   // 通过UIAbilityContext的resourceManager成员的getRawFd接口获取媒体资源播放地址
//   // 返回类型为{fd,offset,length},fd为HAP包fd地址，offset为媒体资源偏移量，length为播放长度
//   let context = getContext(this) as common.UIAbilityContext;
//   let fileDescriptor = await context.resourceManager.getRawFd('H264_AAC.mp4');
//   let avFileDescriptor: media.AVFileDescriptor =
//     { fd: fileDescriptor.fd, offset: fileDescriptor.offset, length: fileDescriptor.length };
//   this.isSeek = true; // 支持seek操作
//   // 为fdSrc赋值触发initialized状态机上报
//   avPlayer.fdSrc = avFileDescriptor;
// }