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


import adc_napi from '@ohos.napi_adc';
import gpioled from '@ohos.gpioled';
import featureAbility from '@ohos.ability.featureAbility'
import prompt from '@system.prompt';
import { TempInfo, TempInfoTable } from "../common/database/TempInfoTable"
import systemTime from '@ohos.systemTime';
import hilog from '@ohos.hilog';
import radio from '@ohos.telephony.radio';


let context: featureAbility.Context = featureAbility.getContext();
context.getFilesDir((error, data) => {
  if (error && error.code !== 0) {
    console.error('getFilesDir fail, error:' + JSON.stringify(error));
  } else {
    console.log('getFilesDir success, data:' + JSON.stringify(data));
  }
});

@Component
export struct show_switch {
  @Link visual: number;
  @Link historybutton_color: string;
  @Link setbutton_color: string;

  build() {
    Toggle({ type: ToggleType.Switch, isOn: Boolean(1 - this.visual) })
      .onChange((isOn) => {
        if (this.visual) {
          prompt.showToast({
            message: "Gesture recognition mode on", // 显示文本
            duration: 1000, // 显示时长
            bottom: 100 // 距离底部的距离
          })
        }
        else {
          this.setbutton_color = "#007bff"
          this.historybutton_color = "#007bff"
          prompt.showToast({
            message: "Gesture recognition mode off", // 显示文本
            duration: 1000, // 显示时长
            bottom: 100 // 距离底部的距离
          })
        }
        this.visual = 1 - this.visual;
      })
  }
}


@Component
export struct showtemp {
  @Link adc_value: number;
  @Link temperature: number;
  @State CONST_TEMP: number = 0.042;
  @State timerId: number = -1;
  @Link mode: boolean;

  //销毁定时器
  aboutToDisappear() {
    if (this.timerId > 0) {
      clearTimeout(this.timerId)
      this.timerId = -1
    }
  }

  private get_adc_value() {
    let value: number = adc_napi.get_adc_value();
    this.adc_value = value;
  }

  private get_adc_tempature() {
    this.temperature = this.adc_value * this.CONST_TEMP;
  }

  private get_adc_tempatureF() {
    this.temperature = this.adc_value * this.CONST_TEMP * 9 / 5 + 32;
  }

  build() {
    Text()
      .height(1)
      .onAppear(() => {
        console.log("在showtemp组件中mode值为：" + this.mode)
        if (this.mode) {
          this.timerId = setInterval(() => {
            this.get_adc_value();
            this.get_adc_tempature();
          }, 1000)
        }
        else {
          console.log("华氏度模式")
          this.timerId = setInterval(() => {
            this.get_adc_value();
            this.get_adc_tempatureF();
          }, 1000)
        }
      })
  }
}

@Component
export struct onlywirte {
  @Link TempInfoobj: TempInfoTable //link index.ets中的tempinfotable对象
  @State timerId: number = -1;

  //销毁定时器
  aboutToDisappear() {
    if (this.timerId > 0) {
      clearTimeout(this.timerId)
      this.timerId = -1
    }
  }

  writetemptofile() {
    this.timerId = setInterval(() => {
      console.log("这里又如何");
      let adc_value1: number = adc_napi.get_adc_value();
      console.log("adc_value1" + adc_value1.toString());
      let temperature = adc_value1 * 0.042;
      if (adc_value1 <= 800 && adc_value1 >= 500) {
        systemTime.getDate((error, date) => {
          if (error) {
            console.error(`Failed to get date. Cause:` + JSON.stringify(error));
            return;
          }
          console.log(`Succeeded in getting Date. Date: ` + JSON.stringify(date));
          let year = date.getFullYear();
          let month = date.getMonth();
          let day = date.getDate();
          let hour = date.getHours();
          let min = date.getMinutes();
          let sec = date.getSeconds();
          console.log("year:" + year);
          console.log("month:" + month);
          console.log("day:" + day);
          console.log("hour:" + hour);
          console.log("min:" + min);
          console.log("sec:" + sec);
          this.TempInfoobj.insertdata(year, month, day, hour, min, sec, temperature, (isdone: boolean) => {
            if (isdone) {
              console.log("myTempInfotable success write!");
            }
            else {
              console.log('myTempInfotable Failed to write');
            }
          });
        });

      }
      else {
        prompt.showToast({
          message: "adc值异常，请检查连线", // 显示文本
          duration: 1000, // 显示时长
          bottom: 200 // 距离底部的距离
        })
      }
    }, 1000);
  }

  aboutToAppear() {
    this.writetemptofile();
  }

  build() {
  }
}

@Component
export struct time_text {
  gettime() {
    systemTime.getDate((error, date) => {
      if (error) {
        console.info(`Failed to get date. message: ${error.message}, code: ${error.code}`);
        return;
      }
      let year = date.getFullYear();
      let month = date.getMonth() + 1;
      let day = date.getDate();
      let hour = date.getHours();
      let min = date.getMinutes();
      let sec = date.getSeconds();
      console.log("year:" + year);
      console.log("month:" + month);
      console.log("day:" + day);
      console.log("hour:" + hour);
      console.log("min:" + min);
      console.log("sec:" + sec);
      console.info(`Succeeded in get date : ${date}`);
      ;
    });
  }

  build() {
    Button('获取时间', { type: ButtonType.Capsule }).borderRadius(20).fontSize(30)
      .onClick(() => {
        this.gettime();
      })
  }
}

@Component
export struct led_module {
  @Link lowthreshold: number;
  @Link highthreshold: number;
  @Link lowthreshold_led: number;
  @Link highthreshold_led: number;
  @Link temperature: number;
  @State timerId: number = -1; //每隔一秒判断温度阈值的定时器id

  //@State key:number[]=[380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,395]
  addLedEffect(num: number) {
    gpioled.setLedStatusWithPromise(num, gpioled.LED_ON).then((result: number) => {
    })
  }

  //关灯
  removeLedEffect(num: number) {
    gpioled.setLedStatusWithPromise(num, gpioled.LED_OFF).then((result: number) => {
    })
  }

  aboutToAppear() {
    //延迟0.2s等待读到temperature
    setTimeout(() => {
      this.timerId = setInterval(() => {
        if (this.temperature < this.lowthreshold) {
          //this.addLedEffect(this.key[this.lowthreshold_led-1]);
          prompt.showToast({
            message: "温度过低！！"
          })
        }
        else {
          //this.removeLedEffect(this.key[this.lowthreshold_led-1]);
        }
        if (this.temperature > this.highthreshold) {
          //this.addLedEffect(this.key[this.highthreshold_led-1]);
          console.log("温度高：" + this.key[this.highthreshold_led-1] + "高温led：" + this.highthreshold_led);
          prompt.showToast({
            message: "温度过高！！"
          })
        }
        else {
          //this.removeLedEffect(this.key[this.highthreshold_led-1]);
        }
      }, 1000)
    }, 200);

  }

  aboutToDisappear() {
    if (this.timerId > 0) {
      clearTimeout(this.timerId)
      this.timerId = -1
    }
  }

  build() {
  }
}

@Component
export struct value_text {
  @Link adc_value: number;
  @Link temperature: number;
  @Link mode: boolean;

  build() {
    Column() {
      Text("adc数值：" + this.adc_value)
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
      if (this.mode) {
        Text("温度：" + this.temperature.toFixed(1) + "°C")// toFixed(1) 浮点数保留一位小数
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
      }
      else {
        Text("温度：" + this.temperature.toFixed(1) + "°F")// toFixed(1) 浮点数保留一位小数
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
      }
    }
  }
}

@Component
export struct get_value_button {
  //@Link dataarray: Array<Number>;
  @Link adc_value: number;
  @Link temperature: number;
  @State CONST_TEMP: number = 0.042;
  //private id: number = 123456;
  private error: string = "";

  build() {
    Column() {
      Button() {
        Text('获取')
          .fontSize(25)
          .fontWeight(FontWeight.Bold)
          .fontStyle(FontStyle.Italic)
      }
      .type(ButtonType.Capsule)
      .margin({
        top: 20
      })
      .backgroundColor('#0D9FFB')
      .width('20%')
      .height('10%')
      .onClick(() => {
        this.get_adc_value();
        this.get_adc_tempature();
        this.writeTemperature();
      })
    }
  }

  private get_adc_value() {
    let value: number = adc_napi.get_adc_value();
    this.adc_value = value;
  }

  private get_adc_tempature() {
    this.temperature = this.adc_value * this.CONST_TEMP;
  }

  private async writeTemperature() {
    let ret: number = adc_napi.filewriteCallback();
  }
}


@Component
export struct startrecord_button {
  @Link TempInfoobj: TempInfoTable //link index.ets中的tempinfotable对象

  private async writetemptofile() {
    let intervalID = setInterval(() => {
      let adc_value: number = adc_napi.get_adc_value();
      console.log("adc_value" + adc_value.toString());
      let temperature = adc_value * 0.042;
      if (adc_value <= 800 && adc_value >= 500) {
        systemTime.getDate((error, date) => {
          if (error) {
            console.error(`Failed to get date. Cause:` + JSON.stringify(error));
            return;
          }
          console.log(`Succeeded in getting Date. Date: ` + JSON.stringify(date));
          let year = date.getFullYear();
          let month = date.getMonth();
          let day = date.getDate();
          let hour = date.getHours();
          let min = date.getMinutes();
          let sec = date.getSeconds();
          console.log("year:" + year);
          console.log("month:" + month);
          console.log("day:" + day);
          console.log("hour:" + hour);
          console.log("min:" + min);
          console.log("sec:" + sec);
          this.TempInfoobj.insertdata(year, month, day, hour, min, sec, temperature, (isdone: boolean) => {
            if (isdone) {
              console.log("myTempInfotable success write!");
            }
            else {
              console.log('myTempInfotable Failed to write');
            }
          });
        });

      }
      else {
        prompt.showToast({
          message: "adc值异常，请检查连线", // 显示文本
          duration: 1000, // 显示时长
          bottom: 200 // 距离底部的距离
        })
      }
    }, 1000);
  }

  build() {
    Column() {
      Button('开始记录', { type: ButtonType.Capsule }).borderRadius(20).fontSize(30)
        .onClick(() => {
          this.writetemptofile();
        })
      // Button('读数据',{type: ButtonType.Capsule}).borderRadius(20).fontSize(30)
      //   // .onAppear(()=>{
      //   //
      //   // })
      //   .onClick(()=>{
      //     console.log("开始读数据测试");
      //     this.TempInfoobj.queryAll((tempinfolist)=>{
      //       for(let i=0;i<tempinfolist.length;i++)
      //       {
      //         console.log(tempinfolist[i].temperature+"readtest");
      //       }
      //     })
      //   })
    }
  }
}

@Component
export struct getdata {
  @Link TempInfoobj: TempInfoTable //link index.ets中的tempinfotable对象
  @Link datainfolist: Array<TempInfo>;

  build() {
    Button('查询数据', { type: ButtonType.Capsule }).borderRadius(20).fontSize(30)
      .onClick(() => {
        console.log("开始读数据测试");
        this.TempInfoobj.queryAll((tempinfolist: TempInfo[]) => {
          this.datainfolist = tempinfolist;
          for (let i = 0; i < this.datainfolist.length; i++) {
            console.log("温度数据：" + this.datainfolist[i].temperature);
          }
        })
      })
  }
}

@Component
export struct GridExample {
  private settings: RenderingContextSettings = new RenderingContextSettings(true)
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings)
  @Link datainfolist: Array<TempInfo>;
  @Link TempInfoobj: TempInfoTable //link index.ets中的tempinfotable对象
  @State timerId: number = -1; //实时绘制的定时器id
  @State slidervaluefortext: number = 1;
  slidervalue: number = 1;
  xleft: number = 40
  axisratio: number = 1;
  intervalratio: number = 1;

  @State mockData: Array<TempInfo> = [
    new TempInfo(2024, 12, 12, 14, 30, 0, 32.5),
    new TempInfo(2024, 12, 12, 14, 45, 0, 33.1),
    new TempInfo(2024, 12, 12, 15, 0, 0, 32.8),
    new TempInfo(2024, 12, 12, 15, 15, 0, 33.3),
    new TempInfo(2024, 12, 12, 15, 30, 0, 28.9),
    new TempInfo(2024, 12, 12, 17, 0, 0, 33.2),
    new TempInfo(2024, 12, 12, 17, 45, 0, 32.6),
    new TempInfo(2024, 12, 12, 18, 0, 0, 30.2),
    new TempInfo(2024, 12, 12, 18, 15, 0, 31.5),
    new TempInfo(2024, 12, 12, 18, 30, 0, 29.9),
    new TempInfo(2024, 12, 12, 18, 45, 0, 28.4),
    new TempInfo(2024, 12, 12, 19, 0, 0, 27.7),
    new TempInfo(2024, 12, 12, 17, 15, 0, 34.1),
    new TempInfo(2024, 12, 12, 17, 30, 0, 33.8),
    new TempInfo(2024, 12, 12, 15, 45, 0, 31.0),
    new TempInfo(2024, 12, 12, 16, 0, 0, 30.6),
    new TempInfo(2024, 12, 12, 16, 15, 0, 29.8),
    new TempInfo(2024, 12, 12, 16, 30, 0, 32.0),
    new TempInfo(2024, 12, 12, 16, 45, 0, 30.3),
    new TempInfo(2024, 12, 12, 19, 15, 0, 26.8)
  ];

  build() {
    Column({ space: 5 }) {
      Scroll() {
        Column() {
          Grid() {
            GridItem() {
              Flex({
                direction: FlexDirection.Column,
                alignItems: ItemAlign.Center,
                justifyContent: FlexAlign.Center
              }) {
                Canvas(this.context)
                  .width('100%')
                  .height('100%')
                  .backgroundColor('#ffff00')
                  .onReady(() => {
                    this.drawAxis()
                    this.drawYLables()
                  })
              }
            }
          }
          .columnsTemplate('1fr')
          .rowsTemplate('1fr')
          .columnsGap(10)
          .rowsGap(10)
          .width('95%')
          .backgroundColor(0xFAEEE0)
          .height(300)
        }
        .width(2100).margin({ top: 5 })
      }
      .scrollable(ScrollDirection.Horizontal)
      .scrollBar(BarState.Auto)

      Row() {
        Button('绘制', { type: ButtonType.Capsule }).borderRadius(20).fontSize(30)
          .onClick(async () => {
            console.log("开始读数据测试");
            this.datainfolist = this.mockData;
            this.context.clearRect(this.xleft + 2, 0, 2100, 0.89 * this.context.height)
            this.drawhistoricalData();
          })
          .margin({ left: 340 })
        Slider({
          value: this.slidervalue,
          min: 1,
          step: 1,
          max: 8,
          style: SliderStyle.OutSet
        })
          .margin({ left: 100 })
          .width(200)
          .showSteps(true)
          .onChange((value: number, mode: SliderChangeMode) => {
            if (this.timerId > 0) {
              clearTimeout(this.timerId)
              this.timerId = -1
            }
            this.slidervaluefortext = value;
            this.slidervalue = value;
            console.info('value:' + value + 'mode:' + mode.toString());
          })
        Text(this.slidervaluefortext.toFixed(0)).fontSize(12)
      }
      .width('100%')
    }
  }

  //销毁定时器
  aboutToDisappear() {
    if (this.timerId > 0) {
      clearTimeout(this.timerId)
      this.timerId = -1
    }
  }

  //画坐标轴
  private drawAxis() {
    const context = this.context;
    context.save()
    context.lineWidth = 3
    context.strokeStyle = '#353535'
    this.context.lineCap = 'round'
    let h1 = this.context.height
    let w1 = this.context.width
    let xstart = this.xleft, ystart = 0.1 * h1, xend = 2000, yend = 0.9 * h1
    context.beginPath()
    this.context.beginPath()
    this.context.moveTo(xstart, 0)
    this.context.lineTo(xstart, yend)
    this.context.lineTo(xend, yend)
    this.context.stroke()
  }

  drawYLables() {
    let h1 = this.context.height
    let w1 = this.context.width
    let xstart = this.xleft, ystart = 0.1 * h1, xend = 0.9 * w1, yend = 0.9 * h1
    let coordinates = 25, value = 35
    while (coordinates <= yend) {
      // 画文本
      this.context.save()
      // 指定绘制的填充色
      this.context.fillStyle = '#0000ff'
      // 设置文本绘制中的字体样式
      this.context.font = '20px sans-serif'
      // 设置文本绘制中的文本对齐方式：文本右对齐
      this.context.textAlign = 'left'
      // 绘制填充类文本
      this.context.fillText(value.toString(), 0, coordinates + 10)
      //绘制y轴上的小线段。
      this.context.strokeStyle = '#353535';
      if (coordinates < yend) {
        this.context.beginPath();
        this.context.moveTo(xstart - 10, coordinates);
        this.context.lineTo(xstart, coordinates)
        this.context.stroke()
      }
      // 更新y轴坐标：每次加上y轴等分
      coordinates += (yend - ystart) / 2
      value -= 5
    }
    this.context.stroke()
  }

  private drawhistoricalData() {
    const context = this.context;
    let h1 = this.context.height
    let w1 = this.context.width
    let xstart = this.xleft, ystart = 0.1 * h1, xend = 0.9 * w1, yend = 0.9 * h1
    let data = this.datainfolist; //数据点坐标
    let gap = 20 * this.axisratio;
    //缓存从数据值到坐标距离的比例因子
    let yFactor = (yend - ystart) / 10;
    let activeX = 0; //记录绘制过程中当前点的坐标
    let activeY = 0; //记录绘制过程中当前点的y坐标
    context.strokeStyle = '#1abc9c';
    //绘制折线
    context.beginPath();
    context.moveTo(xstart, yend - (data[0].temperature - 25) * yFactor); //先将起点移动至第一个数据点
    for (let i = 0, j = 0; i < data.length; i += this.slidervalue, j++) {
      activeX = xstart + (j) * gap;
      activeY = yend - (data[i].temperature - 25) * yFactor;
      context.lineTo(activeX, activeY);
      context.stroke();
    }
    // context.restore()

    // //绘制数据点辅助虚线
    // context.strokeStyle = '#a29d9d'
    // context.setLineDash([10, 20])
    // context.beginPath()
    // for (let i = 0; i < data.length; i++) {
    //   context.moveTo(xstart, yend-(data[i].temperature-25) * yFactor)
    //   activeX = xstart + (i + 1) * gap;
    //   activeY = yend-(data[i].temperature-25) * yFactor;
    //   context.lineTo(activeX, activeY)
    //   context.lineTo(activeX, yend)
    //   context.lineTo(activeX, yend)
    //   context.stroke()
    // }
    //绘制数据点
    let j = 0 //历史数据横坐标画到了哪里
    for (let i = 0; i < data.length; i += this.slidervalue, j++) {
      activeX = xstart + j * gap;
      activeY = yend - (data[i].temperature - 25) * yFactor;
      context.beginPath()
      context.arc(activeX, activeY, 3, 0, 2 * Math.PI, false)
      context.fill()
      context.closePath()
    }
    context.restore()

    //以上为历史数据绘制
    //实时数据绘制
    if (this.timerId > 0) {
      clearTimeout(this.timerId)
      this.timerId = -1
    }
    let i = 0;
    context.beginPath();
    context.moveTo(activeX, activeY)
    this.timerId = setInterval(() => {
      let adc_value1: number = 600;
      let temperature = adc_value1 * 0.042;
      if (adc_value1 <= 800 && adc_value1 >= 500) {
        context.lineTo(activeX + (i + 1) * gap, yend - (temperature - 25) * yFactor)
        context.stroke();
        context.arc(activeX + (i + 1) * gap, yend - (temperature - 25) * yFactor, 3, 0, 2 * Math.PI, false)
        context.fill()
        context.closePath()
        context.moveTo(activeX + (i + 1) * gap, yend - (temperature - 25) * yFactor)
        i++;
      }
      else {
        prompt.showToast({
          message: "adc值异常，请检查连线", // 显示文本
          duration: 1000, // 显示时长
          bottom: 200 // 距离底部的距离
        })
      }
    }, 1000 * this.slidervalue);
  }
}


