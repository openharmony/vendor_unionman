import image from '@ohos.multimedia.image';
import router from '@ohos.router';
// @ts-ignore
import { BusinessError } from '@ohos.base';


import { Phbzi, setquan, sps1,  } from '../compon/hea';






@Component
struct shipin{
  private sp: Phbzi = new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'q6', '______')
  @State bw: number = 0
  @State bcolor: string = '#00e4eeea'
  build(){
    Row(){
      Column(){
        Image(this.sp.tsrc)
          .width(140)
          .height(100)
          .borderRadius(10)
          .focusable(true)
          .border({width:this.bw})
          .borderColor(this.bcolor)
          .onFocus(
            ()=>{
              this.bw = 3
              this.bcolor = '#ff0c27d4'
            }
          )
          .onBlur(
            ()=>{
              this.bw = 0
              this.bcolor = '#00e4eeea'
            }

          )
          .onKeyEvent(
            (event:KeyEvent)=>{
              if(event){
                if(event.keyCode === 2016){
                  console.info(`Succeeded in clicking the 'Next' button.`)
                  // 跳转到第二页
                  setquan(this.sp.srcc)
                  router.pushUrl({ url: 'pages/videoview' }).then(() => {
                    console.info('Succeeded in jumping to the second page.')
                  }).catch((err: BusinessError) => {
                    console.error(`Failed to jump to the second page.Code is ${err.code}, message is ${err.message}`)
                  })
                }

              }
            }
          )
      }

      Column(){
        Text(this.sp.sname)
          .fontSize(25)
          .margin({left:3})
        Text(this.sp.js)
          .fontSize(20)
          .fontColor('#ff7c7878')
          .margin({left:3})
      }
      .width(140)
      .height(100)
      .justifyContent(FlexAlign.SpaceBetween)
      .alignItems(HorizontalAlign.Start)
    }
  }
}

@Entry
@Component
struct shoucang{
  @State bws: number = 0

  build(){
    Column(){
      Row(){
        Text('我的收藏')
          .margin({left:50})
          .fontSize(30)

        Text('返回')
          .fontSize(30)
          .margin({left:440})
          .focusable(true)
          .border({width:this.bws})
          .borderColor(Color.Blue)
          .onFocus(
            ()=>{
              this.bws = 3
            }
          )
          .onBlur(
            ()=>{
              this.bws = 0
            }
          )

          .onKeyEvent(
            (event:KeyEvent)=>{
              if(event){
                if(event.keyCode === 2016){
                  router.pushUrl({ url: 'pages/EntryView' }).then(() => {
                    console.info('Succeeded in jumping to the second page.')
                  }).catch((err: BusinessError) => {
                    console.error(`Failed to jump to the second page.Code is ${err.code}, message is ${err.message}`)
                  })
                }

              }
            }
          )


      }
      .height('20%')
      Row(){
        List({space:10}){
          ForEach(
            sps1,
            (item,index)=>{
              if(index % 2 === 0){
                ListItem(){
                  Column(){
                    Row(){
                      shipin({sp:sps1[index]})
                        .margin({bottom:20})
                    }
                    if(index + 1 <= sps1.length - 1){
                      Row(){
                        shipin({sp:sps1[index + 1]})
                      }
                    }

                  }
                }
              }
            }
          )
        }
        .listDirection(Axis.Horizontal)

      }
      .height('70%')
      .margin({top:10, left:50})


    }
    .height('100%')
    .width('100%')
    .alignItems(HorizontalAlign.Start)
    .backgroundImage($r('app.media.bj1'))


  }
}