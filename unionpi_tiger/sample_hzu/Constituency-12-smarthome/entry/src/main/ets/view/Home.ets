/* Copyright 2023 Unionman Technology Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
import image from '@ohos.multimedia.image'
import router from '@ohos.router'
import i2cnapidemo from '@ohos.i2cnapidemo';
import pwmtest from '@ohos.pwmtest';
import gpioled from '@ohos.gpioled';
import prompt from '@ohos.prompt';
import promptAction from '@ohos.promptAction'
import OkCancelInputDialog from '../pages/OkCancelInputDialog'
import LedControl from '../pages/LedControl'
import CurtainControl from '../pages/CurtainControl'
import  DoorControl  from  '../pages/DoorControl'
import  { Header } from '../componets/headDia'

@Component
export default struct Home {

  //声明自定义弹窗
  dialogController: CustomDialogController = new CustomDialogController({
    builder: OkCancelInputDialog({
      confirm: (): void => this.dialogController.close(),
      cancel: (): void => this.dialogController.close(),
    })
  })

  ledcontroller: CustomDialogController = new CustomDialogController({
    builder: LedControl({
      confirm: (): void => this.ledcontroller.close(),
      cancel: (): void => this.ledcontroller.close(),
    })
  })

  curtaincontroller: CustomDialogController = new CustomDialogController({
    builder: CurtainControl({
      confirm: (): void => this.curtaincontroller.close(),
      cancel: (): void => this.curtaincontroller.close(),
    })
  })

  doorcontroller: CustomDialogController = new CustomDialogController({
    builder: DoorControl({
      confirm: (): void => this.doorcontroller.close(),
      cancel: (): void => this.doorcontroller.close(),
    })
  })

  //温湿度初始化
  @State temp: number = 25
  @State hum: number = 0
  @State OutWeather: string = ' 晴 '
  @State OutTemp: number = 0
  @State OutHum: number = 0
  //默认温度限制28°C
  @State SetTemp: number = 28

  intervalID1: number
  intervalID2: number

  //弹框初始选择从10开始
  @State select: number = 10
  //温度索引范围
  private selectTemp: string [] = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12', '13', '14', '15', '16', '17', '18',
    '19', '20', '21', '22', '23', '24', '25', '26', '27', '28', '29', '30', '31', '32', '33', '34', '35', '36', '37', '38', '39', '40', '41']

  getValue() {
    i2cnapidemo.ReadData();
    this.temp = i2cnapidemo.ReadTemperatureC();
    this.OutTemp = i2cnapidemo.ReadTemperatureC();
    this.hum = i2cnapidemo.ReadHumidity();
    this.OutHum = i2cnapidemo.ReadHumidity();
  }

  getWeather() {
    if (this.OutHum >= 70) {
      this.OutWeather = '雨';
    }  else this.OutWeather = '晴';
  }

  startTempAndHum() {
    this.intervalID1 = setInterval(() => {
      this.getValue();
      this.getWeather();
    }, 1000)
  }

  startTempEW() {
    this.intervalID2 = setInterval(() => {
      if (this.temp >= this.SetTemp) {
        this.Open()
      }else this.Close()
    }, 1000)
  }

  //设置低电平
  Open() {
    gpioled.setLedStatus(this.pin, gpioled.LED_OFF)
  }

  //设置高电平
  Close() {
    gpioled.setLedStatus(this.pin, gpioled.LED_ON)
  }

  build() {
    Column(){
      Row( { space: 28 } ){
        Column( { space :30 } ){
          Column( { space : 15 } ){
            Header().width('70%')
            Text( '设备监控台' )
              .fontStyle(FontStyle.Italic)
              .fontWeight('800')
              .fontSize( ' 40 ' )
            Text( '室内温度：'+ this.temp.toFixed(1) + '°C' )
              .fontWeight('800')
              .fontStyle(FontStyle.Italic)
              .fontSize( ' 30 ' )
            Text( '室内湿度：' + this.hum.toFixed(1) + '%' )
              .fontWeight('800')
              .fontStyle(FontStyle.Italic)
              .fontSize( ' 30 ' )
          }
          Row({ space : 35 }){
            Button('开始监测',{type:ButtonType.Normal,stateEffect: true})
              .fontSize(20)
              .fontColor(Color.Black)
              .fontStyle(FontStyle.Italic)
              .backgroundColor($r('app.color.mainPage_normal'))
              .onClick(()=>{
                this.startTempAndHum()
                this.startTempEW()
              })
            Button('停止监测',{type:ButtonType.Normal,stateEffect: true})
              .fontSize(20)
              .fontColor(Color.Black)
              .fontStyle(FontStyle.Italic)
              .backgroundColor($r('app.color.mainPage_normal'))
              .onClick(()=>{
                clearInterval(this.intervalID1)
                clearInterval(this.intervalID2)
                this.Close()
              })

          }
        }
        .width('40%')
        Column( { space : 40 }){
          Button( {type:ButtonType.Normal,stateEffect:true} ){
            Stack(){
              Image($r('app.media.led'))
              Text('灯光开关')
                .fontSize(25)
                .fontStyle(FontStyle.Italic)
            }
          }
            .onClick(()=>{
              this.ledcontroller.open()
            })
            .height(130)
            .width(150)
            .backgroundColor('#00000000')


          Button( {type:ButtonType.Normal,stateEffect:true} ){
            Stack(){
              Image($r('app.media.curtain'))
                .width(120)
              Text('窗帘开关')
                .fontSize(25)
                .fontStyle(FontStyle.Italic)
            }
          }
            .onClick(()=>{
              this.curtaincontroller.open()
            })
          .height(130)
          .width(150)
          .backgroundColor('#00000000')

        }
        .width('25%')

        Column( { space : 28 }){

          Button( {type:ButtonType.Normal,stateEffect:true} ){
            Stack(){
              Image($r('app.media.door'))
              Text('房门开关')
                .fontSize(25)
                .fontStyle(FontStyle.Italic)
            }
          }
          .onClick(()=>{
            this.doorcontroller.open()
          })
          .height(130)
          .width(150)
          .backgroundColor('#00000000')

          Text('预警温度：' + this.SetTemp.toFixed(1) + '°C' ).fontSize(27)
            .fontWeight('800')
            .fontStyle(FontStyle.Italic)
          Button('设定温度', { type : ButtonType.Capsule , stateEffect : true })
            .fontSize(20)
            .fontStyle(FontStyle.Italic)
            .fontColor(Color.Black)
            .backgroundColor(Color.Yellow)
              // .margin(15)
            .onClick(() => {
              TextPickerDialog.show({
                // 设置文本选择器的选择范围
                range: this.selectTemp,
                // 设置初始选中项的索引值
                selected: this.select,
                // 点击弹窗中的“确定”按钮时触发该回调。
                onAccept: (value: TextPickerResult) => {
                  // 设置select为按下确定按钮时候的选中项index，这样当弹窗再次弹出时显示选中的是上一次确定的选项
                  this.SetTemp = value.index
                  this.select = value.index
                  console.info('TextPickerDialog:onAccept()' + JSON.stringify(value))
                },
                // 点击弹窗中的“取消”按钮时触发该回调
                onCancel: () => {
                  console.info('TextPickerDialog:onCancel()')
                },
                // 滑动弹窗中的选择器使当前选中项改变时触发该回调
                onChange: (value: TextPickerResult) => {
                  console.info('TextPickerDialog:onChange()' + JSON.stringify(value))
                }
              })
            })
            .width('80%')
            .height('15%')
        }
        .width('25%')
      }
      // .backgroundColor($r('app.color.mainPage_selected'))
      .backgroundImage($r('app.media.backg'))
      .height( '100%' )
      .width( '100%' )
      .padding({
        left: 12,
        right: 12,
        bottom: 12
      })
    }
  }

  @Builder
  imageButton(src: Resource , onclick: Function) {
    Button({ type: ButtonType.Normal, stateEffect: true }) {
      Image(src)
    }
    .onClick(()=>{

       onclick//等待事件设计

    })
    .height(100)
    .width(100)
    .backgroundColor($r('app.color.background'))
  }

  onAccept() {
    this.dialogController.close();//关闭弹窗
  }

  onCancel() {
    this.dialogController.close();//关闭弹窗
  }

}
