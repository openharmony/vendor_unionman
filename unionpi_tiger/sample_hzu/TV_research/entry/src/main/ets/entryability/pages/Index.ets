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
