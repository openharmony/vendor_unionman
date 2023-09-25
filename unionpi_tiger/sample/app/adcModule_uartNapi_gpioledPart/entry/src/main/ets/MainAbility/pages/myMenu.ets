/*
 * Copyright 2023 Unionman Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { TempInfo, TempInfoTable } from '../../common/database/TempInfoTable';
import { show_switch,value_text, get_value_button,GridExample, time_text, getdata,showtemp,onlywirte, led_module } from "./widgets"
import prompt from '@system.prompt';
import router from '@ohos.router';
import Queue from '@ohos.util.Queue';
// @ts-ignore
import uart_napi from '@ohos.uart_napi';

@Entry
@Component
struct MyMenu {
  @State adc_value: number = 0 ;
  @State temperature: number = 0 ;
  @State datainfolist:Array<TempInfo>=[];//存放查询结果的数组
  @State tempmode:boolean=true;
  @State mode:boolean=true;
  @State lowthreshold:number=20;
  @State highthreshold:number=40;
  @State lowthreshold_led:number=1;
  @State highthreshold_led:number=16;
  private settings: RenderingContextSettings = new RenderingContextSettings(true)
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings)
  @State gesture_switch:number = 1;
  //switch开关，用来控制是否打开手势控制功能
  @State sensor_respond: string = "尚未启动"
  store_gesture:string="";
  private timeID: number = -1
  ison: Boolean = false;
  Nextread:Boolean =true;//控制异步的锁，只有上一个read读出数据才会新开一个异步工作项
  targetSequence1:Array<string> = ["手势识别为向右","手势识别为下压"];
  targetSequence2:Array<string> = ["手势识别为向左","手势识别为下压"];
  gestureQueue1=new Queue();
  gestureQueue2=new Queue();
  @State setbutton_color:string="#007bff"
  @State historybutton_color:string="#007bff"

  private uart_get() {
    this.Nextread = false;
    uart_napi.uartreadCallback((result) => {
      this.sensor_respond = result;
      this.Nextread = true;
      console.log("result的值为：" + result);
    });
    this.processGesture(this.sensor_respond);
    if(!this.gesture_switch)//如果手势识别已开启
    {
      if(this.sensor_respond==="手势识别为向前")
      {
        this.historybutton_color="#007bff"
        this.setbutton_color="#FFFF00"
        prompt.showToast({
          message: "设置，确认请下压", // 显示文本
          duration: 1000, // 显示时长
          bottom: 100 // 距离底部的距离
        })
        this.store_gesture=this.sensor_respond;
      }
      if(this.sensor_respond==="手势识别为向后")
      {
        this.setbutton_color="#007bff"
        this.historybutton_color="#FFFF00"
        prompt.showToast({
          message: "历史温度趋势，确认请下压", // 显示文本
          duration: 1000, // 显示时长
          bottom: 100 // 距离底部的距离
        })
        this.store_gesture=this.sensor_respond;
      }
      if(this.sensor_respond==="手势识别为下压")
      {
        if(this.store_gesture==="手势识别为向前")
        {
          router.replace({
            url: 'pages/myset',
            params: {
              mode: this.mode,
              lowthreshold: this.lowthreshold,
              highthreshold: this.highthreshold,
              lowthreshold_led: this.lowthreshold_led,
              highthreshold_led: this.highthreshold_led,
            },
          })
        }
        if(this.store_gesture==="手势识别为向后")
        {
          router.push({
            url: 'pages/index',
          })
        }
      }
    }
  }

  // 检查两个数组是否相等
  private arraysAreEqual(arr1, arr2) {
    if (arr1.length !== arr2.length) {
      return false;
    }
    for (let i = 0; i < arr1.length; i++) {
      if (arr1[i] !== arr2[i]) {
        return false;
      }
    }
    return true;
  }

  private processGesture(gesture: string) {
    this.gestureQueue1.add(gesture); // 将手势添加到队列末尾
    // 如果队列长度超过目标手势序列的长度，移除队首元素
    if (this.gestureQueue1.length > this.targetSequence1.length) {
      this.gestureQueue1.pop();
    }
    // 检查队列是否与目标手势序列相同
    if (this.arraysAreEqual(this.gestureQueue1, this.targetSequence1)) {
      prompt.showToast({
        message: "Gesture recognition mode on", // 显示文本
        duration: 1000, // 显示时长
        bottom: 100 // 距离底部的距离
      })
      this.gesture_switch = 0;
    }
    this.gestureQueue2.add(gesture);
    if (this.gestureQueue2.length > this.targetSequence2.length) {
      this.gestureQueue2.pop();
    }
    // 检查队列是否与目标手势序列相同
    if (this.arraysAreEqual(this.gestureQueue2, this.targetSequence2)) {
      this.historybutton_color="#007bff"
      this.setbutton_color="#007bff"
      prompt.showToast({
        message: "Gesture recognition mode off", // 显示文本
        duration: 1000, // 显示时长
        bottom: 100 // 距离底部的距离
      })
      this.gesture_switch = 1;
    }
  }
  onPageShow()
  {
    try{
      // console.log("myMenu中接收到了table对象吗："+this.TempInfoobj)
      this.mode=router.getParams()['mode'];
      this.lowthreshold=router.getParams()['lowthreshold'];
      this.highthreshold=router.getParams()['highthreshold'];
      this.lowthreshold_led=router.getParams()['low_led'];
      this.highthreshold_led=router.getParams()['high_led'];
      uart_napi.uart_init_napi();
      console.log('init succeed');
      this.timeID = setInterval(() => {
        if(this.Nextread)
        {
          this.uart_get();
        }
      },0.2)
    }catch (err){
      console.log("nomode,its first time");
    }
  }
  onPageHide()
  {
    if (this.timeID > 0) {
      clearTimeout(this.timeID);
      //uart_napi.uart_close_napi();
      this.timeID = -1;
      console.log('close succeed');
    }
  }
  build() {
    Column() {
      Row() {
        Column() {
          showtemp({ adc_value: $adc_value, temperature: $temperature, mode: $mode })
          value_text({ adc_value: $adc_value, temperature: $temperature, mode: $mode });
          led_module({
            lowthreshold: $lowthreshold,
            highthreshold: $highthreshold,
            lowthreshold_led: $lowthreshold_led,
            highthreshold_led: $highthreshold_led,
            temperature: $temperature
          })
        }
        .margin({ top: '15%', left: '10%' })

        Column() {
          Button('设置', { type: ButtonType.Capsule }).borderRadius(20).fontSize(30)
            .margin({ bottom: 12 })
            .backgroundColor(this.setbutton_color)
            .onClick(() => {
              router.replace({
                url: 'pages/myset',
                params: {
                  mode: this.mode,
                  lowthreshold: this.lowthreshold,
                  highthreshold: this.highthreshold,
                  lowthreshold_led: this.lowthreshold_led,
                  highthreshold_led: this.highthreshold_led,
                },
              })
            })
          Button('历史温度趋势', { type: ButtonType.Capsule }).borderRadius(20).fontSize(30)
            .margin({ bottom: 12 })
            .backgroundColor(this.historybutton_color)
            .onClick(() => {
              router.push({
                url: 'pages/index',
              })
            })
        }
        .margin({ top: '15%', left: '15%' })
      }
      .height('85%')
      .width('100%')
      show_switch({visual:$gesture_switch,setbutton_color:$setbutton_color,historybutton_color:$historybutton_color});
    }
    .height('100%')
    .width('100%')
    .backgroundImage($r('app.media.bg2')).backgroundImageSize(ImageSize.Cover)
  }
}