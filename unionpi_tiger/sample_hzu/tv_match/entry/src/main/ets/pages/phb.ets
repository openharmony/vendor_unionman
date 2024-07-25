import router from '@ohos.router';
// @ts-ignore
import { BusinessError } from '@ohos.base';
import { setquan } from '../compon/hea';
import { KeyCode } from '@ohos.multimodalInput.keyCode';


class Phbzi{
  public  tsrc: Resource = $r('app.media.icon');
  public  srcc: Resource = $rawfile('videotest.mp4')
  public  sname : string = '1'
  public  js : string = '2'

  constructor(tsrc: Resource, srcc: Resource, sname: string, js:string) {
    this.tsrc = tsrc
    this.srcc = srcc
    this.sname = sname
    this.js = js
  }
}

let sps1:Phbzi[] = [
  new Phbzi($r('app.media.dm1'), $rawfile('videotest.mp4'), '  咒术回战', '   排名1'),
  new Phbzi($r('app.media.dm2'), $rawfile('videotest.mp4'), '  视频名字', '   排名2'),
  new Phbzi($r('app.media.dm3'), $rawfile('videotest.mp4'), '  视频名字', '   排名3'),
  new Phbzi($r('app.media.lunbo1'), $rawfile('videotest.mp4'), '  视频名字', '   排名4'),
  new Phbzi($r('app.media.lunbo2'), $rawfile('videotest.mp4'), '  视频名字', '   排名5'),
  new Phbzi($r('app.media.lunbo3'), $rawfile('videotest.mp4'), '  视频名字', '   排名6'),
]

let sps2:Phbzi[] = [
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'd1', '______'),
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'd2', '______'),
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'd3', '______'),
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'd4', '______'),
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'd5', '______'),
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'd6', '______'),
]

let sps3:Phbzi[] = [
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'dy1', '______'),
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'dy2', '______'),
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'dy3', '______'),
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'dy4', '______'),
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'dy5', '______'),
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'dy6', '______'),
]

let sps4:Phbzi[] = [
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'ds1', '______'),
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'ds2', '______'),
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'ds3', '______'),
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'ds4', '______'),
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'ds5', '______'),
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'ds6', '______'),
]



@Component
struct phb_shipin{
  @State src: Resource = $r('app.media.icon');
  private h: Length = 100
  private w: Length = 140
  private s: string = 'pages/videoview'
  private sp:Phbzi =  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'dy6', '______')
  @State bw: number = 0
  @State bcolor: string = '#00e4eeea'
  build(){
    Row(){
      Column(){
        Image(this.sp.tsrc)
          .width(this.w)
          .height(this.h)
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
                  setquan(this.sp.srcc)
                  // 跳转到第二页
                  router.pushUrl({ url: this.s }).then(() => {
                    console.info('Succeeded in jumping to the second page.')
                  }).catch((err: BusinessError) => {
                    console.error(`Failed to jump to the second page.Code is ${err.code}, message is ${err.message}`)
                  })
                }

              }
            }
          )
      }
      .width('50%')
      Column(){
        Text(this.sp.sname)
          .fontSize(25)
        Text(this.sp.js)
          .fontSize(20)
          .fontColor('#ff7c7878')

      }
      .width('50%')
      .height(100)
      .justifyContent(FlexAlign.SpaceBetween)
      .alignItems(HorizontalAlign.Start)
    }
  }
}





@Component
struct phb_zhuti{
  private sps: Phbzi[] = sps1
  build(){
    Column(){
      Row(){
        Column(){
          ForEach(
            [1,2,3],
            (item,index)=>{
              Row(){
                phb_shipin({sp:this.sps[index]})
              }.height('33%')
              .width('100%')
              .margin({top:8})
            }
          )

        }
        .width('50%')

        Column(){
          ForEach(
            [1,2,3],
            (item, index)=>{
              Row(){
                phb_shipin({sp:this.sps[index + 3]})

              }.height('33%')
              .width('100%')
              .margin({top:8})

            }
          )
        }
        .width('50%')
      }
      .width('94%')
      .height('88%')

    }
    .alignItems(HorizontalAlign.Center)
    .justifyContent(FlexAlign.Center)
  }
}

let num = 1


@Component
struct pname{
  private name: string = '全部'
  private ssize: number = 30

  build(){
    Text(this.name)
      .width('100%')
      .fontSize(this.ssize)
      .fontColor(Color.White)
      .margin({top:15,left:20})


  }
}





@Entry
@Component
struct phb{
  @State xz:number = 1
  @State bws1: number = 0
  @State bws2: number = 0
  @State bws3: number = 0
  @State bws4: number = 0
  @State bws5: number = 0
  private bw1: number = 0
  private bw2: number = 3
  @State colo:string = '#ff0c27d4'


  build(){
    Row(){
      Column(){

        Text('排行榜')
          .width('100%')
          .fontSize(35)
          .fontColor(Color.White)
          .margin({top:15,left:20})


        Text('全部')
          .width('100%')
          .fontSize(30)
          .fontColor(Color.White)
          .margin({top:15,left:20})
          .focusable(true)
          .border({width:this.bws1})
          .borderColor(this.colo)
          .onFocus(
            ()=>{
              this.bws1 = 3
            }
          )
          .onBlur(
            ()=>{
              this.bws1 = 0
            }
          )
          .onKeyEvent(
            (event:KeyEvent)=>{
              if(event){
                if(event.keyCode === 2016){
                  this.xz = 1
                }
              }
            }
          )


        Text('动漫')
          .width('100%')
          .fontSize(30)
          .fontColor(Color.White)
          .margin({top:15,left:20})
          .focusable(true)
          .border({width:this.bws2})
          .borderColor(this.colo)
          .onFocus(
            ()=>{
              this.bws2 = 3
            }
          )
          .onBlur(
            ()=>{
              this.bws2 = 0
            }
          )

          .onKeyEvent(
            (event:KeyEvent)=>{
              if(event){
                if(event.keyCode === 2016){
                  this.xz = 2
                }
              }
            }
          )


        Text('电影')
          .width('100%')
          .fontSize(30)
          .fontColor(Color.White)
          .margin({top:15,left:20})
          .focusable(true)
          .border({width:this.bws3})
          .borderColor(this.colo)
          .onFocus(
            ()=>{
              this.bws3 = 3
            }
          )
          .onBlur(
            ()=>{
              this.bws3 = 0
            }
          )
          .onKeyEvent(
            (event:KeyEvent)=>{
              if(event){
                if(event.keyCode === 2016){
                  this.xz = 3
                }
              }
            }
          )


        Text('电视剧')
          .width('100%')
          .fontSize(30)
          .fontColor(Color.White)
          .margin({top:15,left:20})
          .focusable(true)
          .border({width:this.bws4})
          .borderColor(this.colo)
          .onFocus(
            ()=>{
              this.bws4 = 3
            }
          )
          .onBlur(
            ()=>{
              this.bws4 = 0
            }
          )
          .onKeyEvent(
            (event:KeyEvent)=>{
              if(event){
                if(event.keyCode === 2016){
                  this.xz = 4
                }
              }
            }
          )

        Text('返回')
          .width('100%')
          .fontSize(30)
          .fontColor(Color.White)
          .margin({top:15,left:40})
          .focusable(true)
          .border({width:this.bws5})
          .borderColor(this.colo)
          .onFocus(
            ()=>{
              this.bws5 = 3
            }
          )
          .onBlur(
            ()=>{
              this.bws5 = 0
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
      .height('100%')
      .width('20%')
      .backgroundColor('#4f695c69')
      Column(){

        if(this.xz === 1){
          phb_zhuti({sps:sps1})
        }

        if(this.xz === 2){
          phb_zhuti({sps:sps2})
        }

        if(this.xz === 3){
          phb_zhuti({sps:sps3})
        }

        if(this.xz === 4){
          phb_zhuti({sps:sps4})
        }

      }
      .width('80%')
      .height('100%')
      .margin({top:30})
    }
    .width('100%')
    .height('100%')
    .backgroundImage($r('app.media.bj1'))
  }
}