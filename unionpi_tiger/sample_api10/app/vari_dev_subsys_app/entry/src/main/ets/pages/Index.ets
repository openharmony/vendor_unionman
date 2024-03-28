/* Copyright 2024 Unionman Technology Co., Ltd.
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

import prompt from '@system.prompt'             //文本弹窗模块
import loading1 from './loadingComponent_part1';//自定义加载UI模块
import pwm_napi from '@ohos.mypwm'              //PWM_Napi模块
import adc_target from '@ohos.myadc'            //ADC_Napi模块
import uart_napi from '@ohos.myuart'            //Uart_Napi模块

@Component
struct ADC_Picker {
  @Link aaa:number
  private select: number | number[] =0 ;
  private fruits: string[] = ['ADC1', 'ADC2', 'ADC3', 'ADC4', 'ADC5']   //弹窗内容
  build() {
      Column() {
        Button("ADC_Channel_Choose")
          .margin(10)
          .fontSize(24)
          .backgroundColor(Color.Gray)
          .onClick(() => {
            TextPickerDialog.show({
              range: this.fruits,
              selected: this.select,
              disappearTextStyle: {color: Color.Red, font: {size: 15, weight: FontWeight.Lighter}},
              textStyle: {color: Color.Black, font: {size: 20, weight: FontWeight.Normal}},
              selectedTextStyle: {color: Color.Blue, font: {size: 30, weight: FontWeight.Bolder}},
              onAccept: (value: TextPickerResult) => {
                // 设置select为按下确定按钮时候的选中项index，这样当弹窗再次弹出时显示选中的是上一次确定的选项
                this.select = value.index
                if (typeof this.select === 'number') {
                  this.aaa=this.select
                }
              },
              onCancel: () => {
                console.info("TextPickerDialog:onCancel()")
              },
              onChange: (value: TextPickerResult) => {
                console.info("TextPickerDialog:onChange()" + JSON.stringify(value))
              }
            })
          })
    }.height('20%')
  }
}

//用来绘制loading动画的自定义弹窗
@CustomDialog
struct loadingProgress{
  controller: CustomDialogController;
  build() {
    Column() {
      loading1({ imageWidth: 160, imageHeight: 160 })
        .margin({top:150})
    }
    .width('100%')
    .height('100%')
    .alignItems(HorizontalAlign.Center)
    .backgroundColor(Color.White)
  }
}

@Entry
@Component
struct HelloNAPI {
  @State  Set_PWM_Value: number = 0
  @State  adc_value:number =0
  @State  adc_channel:number =0
  @State  temperature:number =0
  @State  uart_init_flag :number =0
  @State  uart_state:string ="开启串口"
  @State  uart_buffer:string ="没有数据"
  @State  timeID:number =-1
  Nextread:Boolean =true;// 控制异步的锁，只有上一个read读出数据才会新开一个异步工作项

  //用来绘制loading动画的自定义弹窗构造器
  private loading1: CustomDialogController = new CustomDialogController({
    builder: loadingProgress(),           //弹框构造器，指定哪个自定义窗口
    alignment: DialogAlignment.Center,         // 弹框在竖直方向上的对齐方式
    offset: ({ dx: 0, dy: 0 }),                 // 根据alignment的偏移
    autoCancel: false,
    customStyle: true
  })

  private get_Temperature(){
    this.adc_value = adc_target.GetAdcValue(this.adc_channel+1)
    this.temperature=this.adc_value  * 0.042
  }
  //温度刷新定时器
  aboutToAppear():void
  {
    setInterval(() => {
      this.get_Temperature()
    }, 1000)
  }

  build() {
    Row() {
      Column({space: 10}){
        Button("PWM Demo",{stateEffect:true})
          .fontSize(30)
          .backgroundColor(Color.Gray)
          .onClick(() => {
          let strFromNAPI:string = pwm_napi.get_Hello_Generator()
          prompt.showToast({ message: strFromNAPI })         //弹窗测试
          })
          .onAppear(()=>{
            this.loading1.open();
            setTimeout( () => {this.loading1.close();} , 5000);
           })
        //滑块组件
        Slider({
          value: this.Set_PWM_Value,
          min: 0,
          max: 180,
          style: SliderStyle.InSet
        })
          .width('60%')
          .blockColor('#191970')
          .trackColor('#ADD8E6')
          .selectedColor('#4169E1')
          .showTips(true)             //是否显示当前步长
          .onChange((value: number, mode: SliderChangeMode) => {
            this.Set_PWM_Value = value
            // 调用 NAPI 模块中的接口 updateAndGetSliderValue，并将滑块的值传递进去
            let updatedValue: number = pwm_napi.updateAndGetsliderValue(value)
            // 显示弹窗，提示更新后的值
            prompt.showToast({ message: "curent angle: " + updatedValue.toString() })
            console.info('value:' + value + 'mode:' + mode.toString())
          })

        Text(this.Set_PWM_Value.toFixed(0)).fontSize(30).fontColor(Color.White)
      }.size({width: "50%", height: 100}).justifyContent(FlexAlign.Start)

      Column({space: 5}){
        //文本选择组件
        ADC_Picker({aaa:this.adc_channel})

        Text("ADC_Channel: " + (this.adc_channel+1))
          .fontColor(Color.Black)
          .fontSize(30)
        Text('Temperature : '+this.temperature.toFixed(1)+'°')
          .fontSize(30)
          .backgroundColor(Color.Gray)
          .fontColor(Color.Black)
          .fontWeight(FontWeight.Bolder)
        //进度条组件
        Progress({
          value: this.temperature,           // 设置当前进度
          total: 100,                  // 设置进度总量
          type: ProgressType.Capsule
        })
          .size({width: '60%', height: 30})
          .color(Color.Pink)

        Button(this.uart_state)
          .backgroundColor(Color.Gray)
          .onClick(()=>{
            if(this.uart_init_flag){          //由开启状态转换成关闭状态
              console.log('串口关闭')
              this.uart_state="开启串口"
              this.uart_init_flag=0
              uart_napi.uart_close_napi()
              clearInterval(this.timeID)
              this.uart_buffer="串口没打开，没有数据"
            }else{                            //由关闭状态转变成启动状态
              this.uart_state="关闭串口"
              if (uart_napi.uart_init_napi() === 0) {
                this.uart_init_flag=1
              }
              this.timeID=setInterval(()=>{
                // console.log('Nextread:'+this.Nextread)
                console.log('uart buffer:'+this.uart_buffer)
                if(this.Nextread){
                  this.Nextread=false
                  // console.log('aaaaaaaaaaaa')
                  uart_napi.uartcallback((result:string)=>{   //等待回调结果
                    this.uart_buffer=result
                    this.Nextread=true
                    // console.log('cccc')
                    switch (this.uart_buffer){
                      case "触摸点1" :{
                        this.adc_channel=0
                        break;
                      }
                      case "触摸点2" :{
                        this.adc_channel=1
                        break;
                      }
                      case "触摸点5" :{
                        this.Set_PWM_Value = 100
                        pwm_napi.updateAndGetsliderValue(this.Set_PWM_Value)
                      break;
                    }
                      case "触摸点4" :{
                        this.Set_PWM_Value = 0
                        pwm_napi.updateAndGetsliderValue(this.Set_PWM_Value)
                        break;
                      }
                      default:{
                        break;
                      }
                    }
                  })
                  // console.log('bbbbbbbbb')
                }
              },0.3)
            }
        })
        Text('Nextread: '+ this.Nextread).fontColor(Color.White)
        Text('timeID:'+this.timeID.toFixed(0)).fontColor(Color.White)
        Text('uart_init_flag:'+this.uart_init_flag.toFixed(0)).fontColor(Color.White)
        Text('uart_buffer: '+this.uart_buffer).fontColor(Color.White)
      }
      .size({width: "50%", height: '75%'})
    }
    .size({width: "100%", height: '100%'})
    .height('100%')
    .width('100%')
    .backgroundImage($r('app.media.d'))
    .backgroundImageSize({width: "100%", height: '100%'})
  }
}

