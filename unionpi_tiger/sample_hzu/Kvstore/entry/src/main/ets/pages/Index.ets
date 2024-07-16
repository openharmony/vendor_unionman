/*
 * Copyright (c) 2020-2023 Huawei Device Co., Ltd.
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
 */
import gpioled from '@ohos.gpioled';
import pwmtest from '@ohos.pwmtest';
import i2cnapidemo from '@ohos.i2cnapidemo';
import abilityAccessCtrl from '@ohos.abilityAccessCtrl'
import common from '@ohos.app.ability.common'
import { NoteModel } from '../model/NoteDataModel'
import NoteDataSource from '../common/BasicDataSource'
import { TitleBar } from '../common/TitleBar'
import { KvStoreModel } from '../model/KvStoreModel'
import { RemoteDeviceModel } from '../model/RemoteDeviceModel'
import Logger from '../util/Logger'
import Want from '@ohos.app.ability.Want';
const STATE = "STATE"
const NOTES_CHANGE = 'notesChange'
const EXIT = 'exit'
let kvStoreModel: KvStoreModel = new KvStoreModel()
let notesNum: number = 0
const TAG = 'KvstoreIndexPage'
@Entry
@Component
struct Index {
  @State ReadTemperatureC: number = 0;
  @State ReadHumidity: number = 0;
  @State ReadTemperatureF: number = 0;
  private noteDataSource: NoteDataSource = new NoteDataSource([new NoteModel('', '')]);
  @State isDistributed: boolean = false
  @State gpio_value: Number = 0
  @State i2c_value: Number = 0
  @State pwm_value1: Number = 0
  @State pwm_value2: Number = 0
  @State pwm_value3: Number = 0
  @State pwm_value4: Number = 0
  @State@Watch('now_state_change') now_state: Number = 0
  now_state_change(){
    Logger.info(TAG, `now_state_change:${this.now_state}`);
    switch(this.now_state){
      case 1:   /* 注 ：1, 2, 3, 4 为风扇档位为蜂鸣器 9 为温湿度传感器 */
        gpioled.setLedStatusWithPromise(388, gpioled.LED_OFF)
        gpioled.setLedStatusWithPromise(389, gpioled.LED_ON)
        pwmtest.setPwmStatus(2, 0)
        break;
      case 2:
        gpioled.setLedStatusWithPromise(388, gpioled.LED_OFF)
        gpioled.setLedStatusWithPromise(389, gpioled.LED_ON)
        pwmtest.setPwmStatus(2, 90)
        break;
      case 3:
        gpioled.setLedStatusWithPromise(388, gpioled.LED_OFF)
        gpioled.setLedStatusWithPromise(389, gpioled.LED_ON)
        pwmtest.setPwmStatus(2, 179)
        break;
      case 4:
        gpioled.setLedStatusWithPromise(388, gpioled.LED_ON)
        gpioled.setLedStatusWithPromise(389, gpioled.LED_ON)
        pwmtest.setPwmStatus(2, -44)
        break;
      case 9:
        i2cnapidemo.SoftReset();
        i2cnapidemo.ModeSet(3, 1);
        const time = setInterval(() => {
          i2cnapidemo.ReadData();
          if (i2cnapidemo.ReadTemperatureC() <= 30) {
            this.ReadTemperatureC = i2cnapidemo.ReadTemperatureC();
            this.ReadHumidity = i2cnapidemo.ReadHumidity();
            this.ReadTemperatureF = i2cnapidemo.ReadTemperatureF();
          } else {
            gpioled.setLedStatusWithPromise(380, 0);
            clearInterval(time);
          }
        }, 1000);
        break;
      case 8:
        gpioled.setLedStatusWithPromise(380, 1);
    }
  }
  private remoteDeviceModel: RemoteDeviceModel = new RemoteDeviceModel()
  aboutToAppear() {
    let context = getContext(this) as common.UIAbilityContext
    let atManager = abilityAccessCtrl.createAtManager()
    try {
      atManager.requestPermissionsFromUser(context, ['ohos.permission.DISTRIBUTED_DATASYNC']).then((data) => {
        Logger.info(TAG, `data: ${JSON.stringify(data)}`)
      }).catch((err: object) => {
        Logger.info(TAG, `err: ${JSON.stringify(err)}`)
      })
    } catch (err) {
      Logger.info(TAG, `catch err->${JSON.stringify(err)}`);
    }
    let want = JSON.parse(AppStorage.Get('wantMsg')) as Want;
    Logger.info(TAG, `getWant =${JSON.stringify(want)}`);
    if (want.parameters != undefined) {
      if (want.parameters.isStage === 'Stage') {
        this.isDistributed = true
      }
    }
    kvStoreModel.setOnMessageReceivedListener(STATE, (value) => {
      Logger.info(TAG, `NOTES_CHANGE${value}`)
      if (this.isDistributed) {
        if (value.search(EXIT) !== -1) {
          Logger.info(TAG, `[json]EXIT${EXIT}`)
          context.terminateSelf((error) => {
            Logger.info(TAG, `terminateSelf finished, error=${error}`)
          })
        } else {
          this.now_state = JSON.parse(value);
          Logger.info(TAG, `now_state:${this.now_state}`)
        }
      }
    })
  }
  deleteNoteCallBack = (item: NoteModel) => {
    Logger.info(TAG, `deleteNote${JSON.stringify(item)}`);
    let dataArray: NoteModel[] = this.noteDataSource.dataArray;
    for (let i = 0; i < dataArray.length; i++) {
      Logger.info(TAG, `i = ${i} and dataArray = ${JSON.stringify(dataArray[i])}`)
      if (dataArray[i].title === item.title) {
        Logger.info(TAG, `deleteNote index` + i);
        this.noteDataSource.dataArray.splice(i, 1);
        break
      }
    }
    this.noteDataSource.notifyDataReload()
    kvStoreModel.put(NOTES_CHANGE, JSON.stringify(this.noteDataSource.dataArray) + 'numBegin' + notesNum + 'numEnd');
  }
  startAbilityCallBack = (key: string) => {
    Logger.info(TAG, `startAbilityCallBack${key}`);
    if (NOTES_CHANGE === key) {
      kvStoreModel.put(NOTES_CHANGE, `${JSON.stringify(this.noteDataSource.dataArray)}numBegin${notesNum}numEnd`);
    }
    if (EXIT === key) {
      kvStoreModel.put(NOTES_CHANGE, EXIT)
    }
  }
  build() {
    Column() {
      TitleBar({
        startAbilityCallBack: this.startAbilityCallBack,
        remoteDeviceModel: this.remoteDeviceModel,
        isDistributed: $isDistributed
      })
      Column() {
        Text("state:"+this.now_state)
        Row() {
          Column() {
            Button("开启系统")
              .margin({ top: 20, bottom: 20, left: 100 })
              .backgroundColor(Color.Blue)
              .width(150)
              .onClick(() => {
                this.now_state = 9;
                kvStoreModel.put(STATE,this.now_state.toString());
                i2cnapidemo.SoftReset();
                i2cnapidemo.ModeSet(3, 1);
                const time = setInterval(() => {
                  i2cnapidemo.ReadData();
                  if (i2cnapidemo.ReadTemperatureC() <= 30) {
                    this.ReadTemperatureC = i2cnapidemo.ReadTemperatureC();
                    this.ReadHumidity = i2cnapidemo.ReadHumidity();
                    this.ReadTemperatureF = i2cnapidemo.ReadTemperatureF();
                  }
                  else {
                    gpioled.setLedStatusWithPromise(380, 0);
                    clearInterval(time);
                  }
                }, 1000);
              })
          }
          Column() {
            Text('关闭警报:')
              .width('30%')
              .textAlign(TextAlign.Center)
              .fontSize(25)
              .fontStyle(FontStyle.Normal)
              .fontWeight(FontWeight.Bolder)
              .fontFamily('Arial')
              .margin({ left: 50 })
          }
          Toggle({ type: ToggleType.Switch, isOn: true })
            .selectedColor('#007DFF')
            .switchPointColor('#FFFFFF')
            .margin({ top: 30, left: -20 })
            .onChange((isOn: boolean) => {
              this.now_state = 8;
              kvStoreModel.put(STATE,this.now_state.toString())
              let led = 0;
              if (isOn) {
                led = gpioled.LED_OFF;
              } else {
                led = gpioled.LED_ON;
              }
              gpioled.setLedStatusWithPromise(380, led)
            })
            .margin({ left: -10 })
        }
        .margin({ top: 30 })
        Row() {
          Column() {
            Text('选择风扇档位:')
              .width('15%')
              .textAlign(TextAlign.Center)
              .fontSize(20)
              .fontStyle(FontStyle.Normal)
              .fontWeight(FontWeight.Bolder)
              .fontFamily('Arial')
          }
          .margin({ left: 30, top: -10 })
          Button("一档")
            .margin({ top: 20, bottom: 20, left: 100 })
            .backgroundColor(Color.Blue)
            .width(80)
            .onClick(() => {
              this.now_state = 1;
              kvStoreModel.put(STATE,this.now_state.toString())
              gpioled.setLedStatusWithPromise(388, gpioled.LED_OFF)
              gpioled.setLedStatusWithPromise(389, gpioled.LED_ON)
              pwmtest.setPwmStatus(2, 0)
            })
            .margin({ left: 30 })
          Button("二档")
            .margin({ top: 20, bottom: 20, left: 100 })
            .backgroundColor(Color.Blue)
            .width(80)
            .onClick(() => {
              this.now_state = 2;
              kvStoreModel.put(STATE,this.now_state.toString())
              gpioled.setLedStatusWithPromise(388, gpioled.LED_OFF)
              gpioled.setLedStatusWithPromise(389, gpioled.LED_ON)
              pwmtest.setPwmStatus(2, 90)
            })
            .margin({ left: 30 })
          Button("三档")
            .margin({ top: 20, bottom: 20, left: -300 })
            .backgroundColor(Color.Blue)
            .width(80)
            .onClick(() => {
              this.now_state = 3;
              kvStoreModel.put(STATE,this.now_state.toString())
              gpioled.setLedStatusWithPromise(388, gpioled.LED_OFF)
              gpioled.setLedStatusWithPromise(389, gpioled.LED_ON)
              pwmtest.setPwmStatus(2, 179)
            })
            .margin({ left: 30 })
          Button("关风扇")
            .margin({ top: 20, bottom: 20, left: 100 })
            .backgroundColor(Color.Blue)
            .width(100)
            .onClick(() => {
              this.now_state = 4;
              kvStoreModel.put(STATE,this.now_state.toString())
              gpioled.setLedStatusWithPromise(388, gpioled.LED_ON)
              gpioled.setLedStatusWithPromise(389, gpioled.LED_ON)
              pwmtest.setPwmStatus(2, -44)
            })
            .margin({ left: 30 })
        }
        .margin({ top: 40, left: 30 })
        Row() {
          Column() {
            Text('调节窗帘')
              .width('25%')
              .textAlign(TextAlign.Center)
              .fontSize(25)
              .fontStyle(FontStyle.Normal)
              .fontWeight(FontWeight.Bolder)
              .fontFamily('Arial')
              .margin({ left: 200 })
            Slider({
              value: 0,
              min: 0,
              max: 179,
              step: 1,
              style: SliderStyle.OutSet
            })
              .blockColor(Color.Red)
              .width(300)
              .selectedColor(Color.Red)
              .trackColor(Color.Grey)
              .showSteps(true)
              .showTips(false)
              .trackThickness(4)
              .margin({ left: 200, top: 20 })
              .onChange((value: number, mode: SliderChangeMode) => {
                pwmtest.setPwmStatus(1, value)
              })
          }
          .margin({ left: 50 })
          Column() {
            Text("温度：" + this.ReadTemperatureC.toFixed(4) + "°C\n" +
            "湿度：" + this.ReadHumidity.toFixed(4) + "%RH\n" +
            "温度：" + this.ReadTemperatureF.toFixed(4) + "°F")
              .fontSize(25)
              .fontWeight(FontWeight.Bold)
              .textAlign(TextAlign.Start)
              .margin({ left: -150 })
          }
          .margin({ top: 50, bottom: 20, left: 175 })
          .backgroundColor(Color.White)
          .height(90)
          .width(150)
        }
        .margin({ top: 20, left: -150 })
      }
      .layoutWeight(1)
    }
  }
}