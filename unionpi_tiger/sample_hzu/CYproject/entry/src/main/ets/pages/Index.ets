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
import hilog from '@ohos.hilog';
import gpio from '@ohos.gpioled'
import pwm from '@ohos.pwmtest'
import i2c from '@ohos.i2cnapidemo'
import abilityAccessCtrl from '@ohos.abilityAccessCtrl'
import common from '@ohos.app.ability.common'
import { NoteModel } from '../model/NoteDataModel'
import { NoteListItem } from '../common/NoteItem'
import NoteDataSource from '../common/BasicDataSource'
import { TitleBar } from '../common/TitleBar'
import { KvStoreModel } from '../model/KvStoreModel'
import { RemoteDeviceModel } from '../model/RemoteDeviceModel'
import { transStrToNoteModel } from '../model/NoteDataModel'
import Logger from '../util/Logger'
import Want from '@ohos.app.ability.Want';
const STATE:string = "STATE"
const NOTES_CHANGE = 'notesChange'
const EXIT = 'exit'
let kvStoreModel: KvStoreModel = new KvStoreModel()
let notesNum: number = 0
const TAG = 'KvstoreIndexPage'
@Extend(Button) function buttonStyle () {.margin(5).fontSize(20).fontWeight(FontWeight.Bold).fontStyle(FontStyle.Italic).type(ButtonType.Capsule).backgroundColor('#0D9FFB')}
@Entry
@Component
struct Index {
  @State message: number = 0
  @State pwm1Value: number = 0
  @State pwm2: string = '关'
  @State ReadTemperatureC: number = 0
  @State ReadHumidity: number = 0
  @State ReadTemperatureF: number = 0
  @State@Watch('now_state_change') STATE: Number = 0
  now_state_change(){
    Logger.info(TAG, `now_state_change:${this.STATE}`);
    switch(this.STATE){
      case 1: pwm.setPwmStatus(1,0); break;
      case 2: pwm.setPwmStatus(1,90); break;
      case 3: pwm.setPwmStatus(1,179); break;
      case 12: gpio.setLedStatusWithPromise(381, 0);gpio.setLedStatusWithPromise(382, 0);pwm.setPwmStatus(2,-44);break;
      case 13: gpio.setLedStatusWithPromise(381, 1);gpio.setLedStatusWithPromise(382, 0);pwm.setPwmStatus(2,0);break;
      case 14: gpio.setLedStatusWithPromise(381, 1);gpio.setLedStatusWithPromise(382, 0);pwm.setPwmStatus(2,90);break;
      case 15: gpio.setLedStatusWithPromise(381, 1);gpio.setLedStatusWithPromise(382, 0);pwm.setPwmStatus(2,179);break;
      case 16: i2c.SoftReset(); i2c.ModeSet(4, 2);
          this.message = setInterval(() => {
          i2c.ReadData();
          this.ReadTemperatureC = i2c.ReadTemperatureC();this.ReadHumidity = i2c.ReadHumidity();
          this.ReadTemperatureF = i2c.ReadTemperatureF();
      }, 2000);
        break;
    }
  }
  private noteDataSource: NoteDataSource = new NoteDataSource([new NoteModel('', '')]);
  @State isDistributed: boolean = false
  private remoteDeviceModel: RemoteDeviceModel = new RemoteDeviceModel()
  aboutToAppear() {
    let context = getContext(this) as common.UIAbilityContext
    let atManager = abilityAccessCtrl.createAtManager()
    try {
      atManager.requestPermissionsFromUser(context,['ohos.permission.DISTRIBUTED_DATASYNC']).then((data) => {
        Logger.info(TAG, `data: ${JSON.stringify(data)}`)
      }).catch((err: object) => {
        Logger.info(TAG, `err: ${JSON.stringify(err)}`)
      })
    } catch (err) {
      Logger.info(TAG, `catch err->${JSON.stringify(err)}`);
    }
    let want = JSON.parse(AppStorage.Get('wantMsg')) as Want;
    Logger.info(TAG,`getWant =${JSON.stringify(want)}`);
    if(want.parameters != undefined) {
      if (want.parameters.isStage === 'Stage') {
        this.isDistributed = true
      }
    }
    kvStoreModel.setOnMessageReceivedListener(STATE, (value) => {
      Logger.info(TAG,`NOTES_CHANGE${value}`)
      if (this.isDistributed) {
        if (value.search(EXIT) !== -1) {
          Logger.info(TAG,`[json]EXIT${EXIT}`)
          context.terminateSelf((error) => {
            Logger.info(TAG,`terminateSelf finished, error=${error}`)
          })
        } else {
          this.STATE=JSON.parse(value);
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
        break;
      }
    }
    this.noteDataSource.notifyDataReload()
    kvStoreModel.put(NOTES_CHANGE, JSON.stringify(this.noteDataSource.dataArray) + 'numBegin' + notesNum + 'numEnd');
  }
  startAbilityCallBack = (key: string) => {
    Logger.info(TAG,`startAbilityCallBack${key}`);
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
        startAbilityCallBack: this.startAbilityCallBack, remoteDeviceModel: this.remoteDeviceModel,
        isDistributed: $isDistributed
      })
      Column() {
        Row() {
          Text('分布式控制终端')
            .width('100%').textAlign(TextAlign.Center).fontSize(30).fontStyle(FontStyle.Italic)
            .fontWeight(FontWeight.Bold).fontFamily('Arial')
        }
        .width('100%')
        Row() {
          Column() {
            Row() {
              Column() {
                Button("打开数据刷新")
                  .buttonStyle()
                  .onClick(()=>{
                    kvStoreModel.put(STATE,"16")
                    i2c.SoftReset();
                    i2c.ModeSet(4, 2);
                    this.message = setInterval(() => {
                      i2c.ReadData();
                      this.ReadTemperatureC = i2c.ReadTemperatureC();
                      this.ReadHumidity = i2c.ReadHumidity();
                      this.ReadTemperatureF = i2c.ReadTemperatureF();
                    }, 2000);
                  })
              }
              Text("温度：" + this.ReadTemperatureC.toFixed(4) + "°C\n" + "湿度：" + this.ReadHumidity.toFixed(4) + "%RH\n" + "温度：" + this.ReadTemperatureF.toFixed(4) + "°F")
                .fontSize(20).fontWeight(FontWeight.Bold).textAlign(TextAlign.Start)
            }
          }
          .width('50%').height('30%')
          Column() {
            Row() {
              Column() {
                Text('风扇转速：'+ this.pwm2)
                  .fontSize(16)
              }
            }
            Row() {
              Button('关')
                .buttonStyle()
                .onClick(()=>{
                  kvStoreModel.put(STATE,"12")
                  gpio.setLedStatusWithPromise(381, 0)
                  gpio.setLedStatusWithPromise(382, 0)
                  pwm.setPwmStatus(2,-44)
                  this.pwm2='关'
                })
              Button('一档')
                .buttonStyle()
                .onClick(()=>{
                  kvStoreModel.put(STATE,"13")
                  gpio.setLedStatusWithPromise(381, 1)
                  gpio.setLedStatusWithPromise(382, 0)
                  pwm.setPwmStatus(2,0)
                  this.pwm2='一档'
                })
              Button('二档')
                .buttonStyle()
                .onClick(()=>{
                  kvStoreModel.put(STATE,"14")
                  gpio.setLedStatusWithPromise(381, 1)
                  gpio.setLedStatusWithPromise(382, 0)
                  pwm.setPwmStatus(2,90)
                  this.pwm2='二档'
                })
              Button('三档')
                .buttonStyle()
                .onClick(()=>{
                  kvStoreModel.put(STATE,"15")
                  gpio.setLedStatusWithPromise(381, 1)
                  gpio.setLedStatusWithPromise(382, 0)
                  pwm.setPwmStatus(2,179)
                  this.pwm2='三档'
                })
            }
          }
          .width('40%')
          .height('30%')
        }
        .width('100%')
        Row() {
          Column() {
            Row() {
              Text('客厅灯光')
                .fontSize(16)
              Toggle({ type: ToggleType.Switch, isOn: false })
                .selectedColor('#007DFF')
                .switchPointColor('#FFFFFF')
                .onChange((isOn: boolean) => {
                  console.info('Component status:' + isOn)
                  let led = isOn?gpio.LED_ON:gpio.LED_OFF
                  gpio.setLedStatusWithPromise(385, led).then((result:number) => {
                    if (result === 0) {
                      kvStoreModel.put(STATE,"4")
                      console.info("Turned on LED." + result)
                    } else {
                      kvStoreModel.put(STATE,"5")
                      console.info("Turned on LED failed." + result)
                    }
                  })
                })
              Text('厨房灯光')
                .fontSize(16)
              Toggle({ type: ToggleType.Switch, isOn: false })
                .selectedColor('#007DFF')
                .switchPointColor('#FFFFFF')
                .onChange((isOn: boolean) => {
                  console.info('Component status:' + isOn)
                  let led = isOn?gpio.LED_ON:gpio.LED_OFF
                  gpio.setLedStatusWithPromise(386, led).then((result:number) => {
                    if (result === 0) {
                      kvStoreModel.put(STATE,"6")
                      console.info("Turned on LED." + result)
                    } else {
                      kvStoreModel.put(STATE,"7")
                      console.info("Turned on LED failed." + result)
                    }
                  })
                })
            }
            Row() {
              Text('浴室灯光')
                .fontSize(16)
              Toggle({ type: ToggleType.Switch, isOn: false })
                .selectedColor('#007DFF')
                .switchPointColor('#FFFFFF')
                .onChange((isOn: boolean) => {
                  console.info('Component status:' + isOn)
                  let led = isOn?gpio.LED_ON:gpio.LED_OFF
                  gpio.setLedStatusWithPromise(387, led).then((result:number) => {
                    if (result === 0) {
                      kvStoreModel.put(STATE,"8")
                      console.info("Turned on LED." + result)
                    } else {
                      kvStoreModel.put(STATE,"9")
                      console.info("Turned on LED failed." + result)
                    }
                  })
                })
              Text('卧室灯光')
                .fontSize(16)
              Toggle({ type: ToggleType.Switch, isOn: false })
                .selectedColor('#007DFF')
                .switchPointColor('#FFFFFF')
                .onChange((isOn: boolean) => {
                  console.info('Component status:' + isOn)
                  let led = isOn?gpio.LED_ON:gpio.LED_OFF
                  gpio.setLedStatusWithPromise(388, led).then((result:number) => {
                    if (result === 0) {
                      kvStoreModel.put(STATE,"10")
                      console.info("Turned on LED." + result)
                    } else {
                      kvStoreModel.put(STATE,"11")
                      console.info("Turned on LED failed." + result)
                    }
                  })
                })
            }
          }
          .width('50%')
          .height('30%')
          Column() {
            Row() {
              Column() {
                Text('窗户开度：'+this.pwm1Value.toFixed(0)+'°')
                  .fontSize(16)
                Slider({
                  value: this.pwm1Value,
                  min: 0,
                  max: 179,
                  step: 1,
                  style: SliderStyle.OutSet
                })
                  .blockColor('#0D9FFB')
                  .selectedColor('#0D9FFB')
                  .trackColor(Color.Grey)
                  .showSteps(true)
                  .showTips(false)
                  .trackThickness(4)
                  .onChange((value: number, mode: SliderChangeMode) => {
                    this.pwm1Value = value
                    pwm.setPwmStatus(1,value)
                  })
              }
            }
            Row() {
              Button('全关')
                .buttonStyle()
                .onClick(()=>{
                  pwm.setPwmStatus(1,0)
                  this.pwm1Value=0
                  kvStoreModel.put(STATE,"1")
                })
              Button('半开')
                .buttonStyle()
                .onClick(()=>{
                  pwm.setPwmStatus(1,90)
                  this.pwm1Value=90
                  kvStoreModel.put(STATE,"2")
                })
              Button('全开')
                .buttonStyle()
                .onClick(()=>{
                  pwm.setPwmStatus(1,179)
                  this.pwm1Value=179
                  kvStoreModel.put(STATE,"3")
                })
            }
          }
          .width('40%')
          .height('30%')
        }
        .width('100%')
      }
      .justifyContent(FlexAlign.SpaceAround)
      .height('100%')
    }
  }
}