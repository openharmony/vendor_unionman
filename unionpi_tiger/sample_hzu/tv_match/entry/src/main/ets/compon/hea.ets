export  let quanvideoSrc: Resource = $rawfile('videotest.mp4')

export function  setquan(quanvideoSrc1: Resource): void{
  quanvideoSrc = quanvideoSrc1
}

export class Phbzi{
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

export let sps1:Phbzi[] = [
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'q1', '______'),
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'q2', '______'),
  new Phbzi($r('app.media.icon'), $rawfile('videotest.mp4'), 'q3', '______')
]