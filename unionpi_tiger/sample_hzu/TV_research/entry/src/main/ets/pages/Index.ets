import router from '@ohos.router'

@Entry
@Component
struct Index {
  @State bgColor: string = '#043ba0'

  build() {
    Row() {
      Column() {
        Row() {
          Text('2024年6月27日').fontSize(25).fontColor(Color.White).margin({ left: '50%' })
          Text('星期四').fontSize(25).fontColor(Color.White).margin({ right: '50%' })
          Text('19：35').fontSize(25).fontColor(Color.White)
        }

        Flex() {
          Text('首页').fontSize(30).padding({ right: '4%' }).fontColor(Color.White)
          Text('电影').fontSize(30).padding({ right: '4%' }).fontColor(Color.White)
          Text('电视剧').fontSize(30).padding({ right: '4%' }).fontColor(Color.White)
          Text('动漫').fontSize(30).padding({ right: '4%' }).fontColor(Color.White)
          Text('综艺').fontSize(30).padding({ right: '4%' }).fontColor(Color.White)
          Text('少儿').fontSize(30).padding({ right: '4%' }).fontColor(Color.White)
          Text('教育').fontSize(30).padding({ right: '4%' }).fontColor(Color.White)
          Text('游戏').fontSize(30).padding({ right: '8%' }).fontColor(Color.White)
          Text('更多').fontSize(30).fontColor(Color.White)
        }
        .onClick(() => {
          this.bgColor = generateRandomColor()
        })
        .flexGrow(1)
        .padding({ right: '6%', left: '6%', top: '2%' })

        Flex() {
          Image('./assets/1.png').height('60%').borderRadius(15).margin(2)
            .onClick(() => {
              router.pushUrl({url:'pages/VideoPage',params:$r('app.media.video003')})
            })
          Column() { // 上半部分布局
            Image('./assets/2.png').height('30%').borderRadius(15).margin(2)
            Image('./assets/3.png').height('30%').borderRadius(15).margin(2)
          }



          Column() { // 下半部分布局
            Image('./assets/5.png').height('30%').borderRadius(15).margin(2)
              .onClick(()=>{
                router.pushUrl({url:'pages/VideoList'})
              })
            Image('./assets/6.png').height('30%').borderRadius(15).margin(2)
          }

          Image('./assets/4.png').height('60%').borderRadius(15).margin(2)
            .onClick(()=>{
              router.pushUrl({url:'pages/VideoPage',params:$r('app.media.video004')})
            })
        }
        .padding({ right: '8%', left: '8%', top: '4%' })

        Flex({ justifyContent: FlexAlign.SpaceAround }) {
          Image('./assets/7.png').height('13%')
          Image('./assets/8.png').height('13%')
          Image('./assets/9.png').height('13%')
          Image('./assets/10.png').height('13%')
          Image('./assets/11.png').height('13%')
        }
        .padding({ right: '10%', left: '10%', top: '2%', bottom: '3%' })
      }
    }
    .padding({ top: '2%' })
    .height('100%')
    .backgroundColor(this.bgColor)
  }
}

function generateRandomColor(): string {

  // 生成RGB颜色的三个组成部分
  let r = toHexString(getRandomInt(0, 255));
  let g = toHexString(getRandomInt(0, 255));
  let b = toHexString(getRandomInt(0, 255));

  // 组合成完整的十六进制颜色代码
  return '#' + r + g + b;
}

function getRandomInt(min: number, max: number): number {
  min = Math.ceil(min);
  max = Math.floor(max);
  return Math.floor(Math.random() * (max - min + 1)) + min;
}

// 生成0-255之间的随机整数，然后转换为两位的十六进制字符串
function toHexString(num: number): string {
  let hex = num.toString(16);
  if (hex.length === 1) {
    hex = '0' + hex;
  }
  return hex;
}

