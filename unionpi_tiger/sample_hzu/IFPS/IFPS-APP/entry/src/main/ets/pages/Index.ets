import {createTable, insertData, queryData} from './DBManager'
import systemDateTime from '@ohos.systemDateTime';
import display from '@ohos.display'

// @ts-ignore
import ifpsnapidemo from '@ohos.ifpsnapidemo';

import prompt from '@ohos.prompt';

type device = {
  deviceName: string;
  value: string;
  status: boolean;
};

type logItem = {
  time: string;
  level: string;
  info :string;
}

@Entry
@Component
struct Home {

  @State systemStatus: number = 0
  @State pwm1_start :boolean = false
  @State pwm2_start :boolean = false
  @State flameSensorThreshold: number = 23
  private flameSensorStatus: Boolean = false
  @State smokeSensorThreshold: number = 16
  private smokeSensorStatus: Boolean = false
  @State temperatureThreshold: number = 50
  private temperatureStatus: Boolean = false
  @State humidityThreshold:    number = 30
  private humidityStatus:    Boolean = false

  private controller_tabs : TabsController = new TabsController()
  private controller_dialog1: CustomDialogController = new CustomDialogController({
    builder: CustomDialogValueInput({
      textValue: this.flameSensorThreshold,
      inputValue: $flameSensorThreshold,
    })
  })
  private controller_dialog2: CustomDialogController = new CustomDialogController({
    builder: CustomDialogValueInput({
      textValue: this.smokeSensorThreshold,
      inputValue: $smokeSensorThreshold,
    })
  })
  private controller_dialog3: CustomDialogController = new CustomDialogController({
    builder: CustomDialogValueInput({
      textValue: this.temperatureThreshold,
      inputValue: $temperatureThreshold,
    })
  })
  private controller_dialog4: CustomDialogController = new CustomDialogController({
    builder: CustomDialogValueInput({
      textValue: this.humidityThreshold,
      inputValue: $humidityThreshold,
    })
  })
  private displayClass: display.Display | null = null
  private dpi: number = 160

  @State logListData: Array<logItem> = []
  private devicesData: Array<device> = []
  @State gridData: Array<device> = []
  @State page: number = 1
  private pageSize: number = 20
  private indexOffset: number = 0

  aboutToAppear() {
    this.init()
    console.log(this.page.toString())
  }

  init() {
    // 获得屏幕信息
    try {
      this.displayClass = display.getDefaultDisplaySync();
      this.dpi = this.displayClass.densityDPI
      console.log('Test densityDPI:' + JSON.stringify(this.displayClass.densityDPI));
    } catch (exception) {
      console.error('Failed to obtain the default display object. Code: ' + JSON.stringify(exception));
    }
    // 数据初始化
    createTable(getContext(this))
    this.query()
    this.update()
    // 启动定时器
    setInterval(this.update.bind(this), 3000)
  }

  update() {
    while(this.devicesData.length > 0) {
      this.devicesData.pop()
    }

    // 获取ppm(烟雾浓度)
    const ppmValue_f = getPPMValue()
    if(ppmValue_f){
      const ppmValue = ppmValue_f
      const ppmValueString = ppmValue_f.toFixed(3)
      const deviceStatus = ppmValue > this.smokeSensorThreshold
      if(deviceStatus && !this.smokeSensorStatus) {
        this.smokeSensorStatus = true
        this.addData('2', '检测到可燃气体，二级警报!')
      }
      else if(!deviceStatus && this.smokeSensorStatus) {
        this.smokeSensorStatus = false
      }
      this.devicesData.push({"deviceName": "烟雾传感器", "value": ppmValueString, "status": deviceStatus})
    }

    // 获取火焰传感器 adc值
    const fireValue_f = getFIREValue();
    if(fireValue_f){
      const fireValue = fireValue_f
      const fireValueString = fireValue_f.toFixed(3)
      const deviceStatus = fireValue < this.flameSensorThreshold
      if(deviceStatus && !this.flameSensorStatus) {
        this.flameSensorStatus = true
        this.addData('1', '检测到火焰，一级警报!')
        // 自动开启舵机
        this.pwm1_start = true

      }
      else if(!deviceStatus && this.flameSensorStatus) {
        this.flameSensorStatus = false
        this.pwm1_start = false
      }
      this.devicesData.push({"deviceName": "火焰传感器", "value": fireValueString, "status": deviceStatus})
    }

    // 温湿度
    const sensorData_f = readTemperatureAndHumidity()
    if(sensorData_f) {
      const temperatureValue = sensorData_f.temperatureC
      const temperatureValueString = temperatureValue.toFixed(3)
      const deviceStatus1 = temperatureValue > this.temperatureThreshold
      if(deviceStatus1 && !this.temperatureStatus) {
        this.temperatureStatus = true
        this.addData('3', `温度高于${this.temperatureThreshold}，三级警报!`)
      }
      else if(!deviceStatus1 && this.temperatureStatus) {
        this.temperatureStatus = false
      }
      this.devicesData.push({"deviceName": "温度传感器", "value": temperatureValueString, "status": deviceStatus1})

      const humidityValue = sensorData_f.humidity
      const humidityValueString = sensorData_f.humidity.toFixed(3)
      const deviceStatus2 = humidityValue < this.humidityThreshold
      if(deviceStatus2 && !this.humidityStatus) {
        this.humidityStatus = true
        this.addData('3', `湿度低于${this.humidityThreshold}，三级警报!`)
      }
      else if(!deviceStatus2 && this.humidityStatus) {
        this.humidityStatus = false
      }
      this.devicesData.push({"deviceName": "湿度传感器", "value": humidityValueString, "status": deviceStatus2})
    }

    this.devicesData.push({"deviceName": "舵机", "value": 'none', "status": false})
    this.devicesData.push({"deviceName": "蜂鸣器", "value": 'none', "status": false})

    // console.log(JSON.stringify(this.devicesData))

    // 更新主页警告状态
    if(this.flameSensorStatus) this.systemStatus = 1
    else if(this.smokeSensorStatus) this.systemStatus = 2
    else if(this.temperatureStatus || this.humidityStatus) this.systemStatus = 3
    else this.systemStatus = 0
    // 更新数据
    this.gridData = this.devicesData
  }

  addData(level_s: string, info_s: string) {
    getTimeString().then(time_s => {
      const item: logItem = {time: time_s, level: level_s, info: info_s}
      insertData(getContext(this), item).then(() => {
        // 更新列表
        console.log(`update: ${JSON.stringify(item)}`);
        this.indexOffset++
        this.logListData.unshift(item)
        // 弹窗警告
        AlertDialog.show(
          {
            title: '警告!',
            message: info_s,
            autoCancel: true,
            alignment: DialogAlignment.Bottom,
            offset: { dx: 0, dy: -20 },
            gridCount: 3
          }
        )
      })
    })
  }

  query() {
    queryData(getContext(this), this.page, this.indexOffset, this.pageSize)
      .then(data => {
        if(data){
          if(this.page === 1)
            this.logListData = data
          else
            this.logListData = this.logListData.concat(data)
          this.page++
        }
        else {
          console.log('now last page')
        }
      })
  }

  @Builder buildLogList() {
    Row() {
      this.buildLogo()
      if(this.logListData.length != 0) {
        List({space: 20, initialIndex: 0}) {
          ForEach(this.logListData, (item) => {
            ListItem() {
              Row(){
                // 警告等级数字
                Text(item.level)
                  .width('30%')
                  .fontSize(this.dpi / 2.5)
                  .fontColor(getTextColor(Number(item.level)))
                  .textAlign(TextAlign.Center)
                // 警告内容
                Column(){
                  Text(item.date)
                    .fontSize(this.dpi / 5)
                    .fontColor(Color.Black)
                    .textAlign(TextAlign.Center)
                    .padding({bottom: 5})
                  Text(item.info)
                    .fontSize(this.dpi / 4)
                    .maxLines(2)
                    .textOverflow({overflow:TextOverflow.Ellipsis})
                }
                .width('70%')
                .padding({right:'5%'})
                .alignItems(HorizontalAlign.Start)
                .justifyContent(FlexAlign.Start)
              }
              .height('20%')
              .alignItems(VerticalAlign.Center)
              .justifyContent(FlexAlign.Center)
              .backgroundColor('#f5f5f5')
              .borderRadius(10)
            }.editable(true)
          })
        }
        .width('60%')
        .height('80%')
        //.divider({strokeWidth:1, color:Color.Grey, startMargin:20, endMargin:20})
        .onReachEnd( () => {
          console.log(`now page ${this.page}`)
          this.query()
        })
      }
      else {
        Text('empty')
          .fontColor('#708090')
          .fontSize(this.dpi / 4)
          .textAlign(TextAlign.Center)
          .height('80%')
          .width('60%')
      }
    }
  }
  @Builder buildGrid() {
    Row() {
      this.buildLogo()
      Grid(){
        ForEach(this.gridData, (item, index) => {
          GridItem(){
            Column(){
              Text(`${item.deviceName}`)
                .fontSize(this.dpi / 5)
                .fontColor(Color.Black)
                .textAlign(TextAlign.Center)
                .padding({top: '3%'})
                .height('30%')

              Column(){
                // 火焰传感器 & 烟雾传感器
                if(index === 0 || index === 1) {
                  Stack() {
                    Column() {
                      // 状态显示
                      Text(`${getStatusText(item.status)}`)
                        .fontColor(getStatusTextColor(item.status))
                        .fontSize(this.dpi / 4)
                        .fontWeight(FontWeight.Bold)
                      // 数据显示
                      Text(String(item.value))
                        .fontSize(this.dpi / 4)
                        .fontColor(getStatusTextColor(item.status))
                        .padding({top: '2%'})
                    }
                    .width('100%')

                    Column() {
                      // 阈值设置
                      Button({ type: ButtonType.Circle, stateEffect: false })
                        .height(this.dpi / 4).width(this.dpi / 4)
                        .backgroundColor('#00000000')
                        .backgroundImage($r('app.media.ic_public_settings'))
                        .backgroundImageSize({ width: '100%', height: '100%' })
                        .onClick(() => {
                          // 弹窗
                          if(index === 1) this.controller_dialog1.open()
                          else this.controller_dialog2.open()
                        })
                    }
                    .width('90%').height('90%')
                    .justifyContent(FlexAlign.End)
                    .alignItems(HorizontalAlign.End)
                  }
                }
                // 温湿度传感器
                else if(index === 2 || index === 3) {
                  Stack() {
                    if(index === 2) {
                      Text(`${item.value}°C`)
                        .fontSize(this.dpi / 3)
                        .fontColor(getNumberTextColor(item.value, this.temperatureThreshold))
                        .padding('5%')
                    }
                    else {
                      Text(`${item.value}%`)
                        .fontSize(this.dpi / 3)
                        .fontColor(getNumberTextColor(item.value, this.humidityThreshold))
                        .padding('5%')
                    }
                    Column() {
                      // 阈值设置
                      Button({ type: ButtonType.Circle, stateEffect: false })
                        .height(this.dpi / 4).width(this.dpi / 4)
                        .backgroundColor('#00000000')
                        .backgroundImage($r('app.media.ic_public_settings'))
                        .backgroundImageSize({ width: '100%', height: '100%' })
                        .onClick(() => {
                          // 弹窗
                          if(index === 2) this.controller_dialog3.open()
                          else this.controller_dialog4.open()
                        })
                    }
                    .width('90%').height('90%')
                    .justifyContent(FlexAlign.End)
                    .alignItems(HorizontalAlign.End)
                  }
                }
                // 舵机
                else if(index === 4){
                  // 舵机开关
                  Row() {
                    Toggle({ type: ToggleType.Switch, isOn: this.pwm1_start })
                      .size({ width: '35%', height: '35%' })
                      .onChange((isChecked) => {
                        this.pwm1_start = isChecked
                        if(isChecked) {
                          prompt.showToast({
                            message: "打开舵机", // 显示文本
                          })
                          try {
                            ifpsnapidemo.SetPwmStatus(1, 90)
                            console.log(`SetPwmStatus ${isChecked}}`)
                          }catch(err) {
                            console.log('ERROR: SetPwmStatus failed' + err)
                          }
                        }
                        else {
                          prompt.showToast({
                            message: "关闭舵机", // 显示文本
                          })
                          try {
                            ifpsnapidemo.SetPwmStatus(1, 0)
                            console.log(`SetPwmStatus ${isChecked}}`)
                          }catch(err) {
                            console.log('ERROR: SetPwmStatus failed' + err)
                          }
                        }
                      })
                  }
                  .width('100%')
                  .padding({ top: '3%' })
                  .justifyContent(FlexAlign.Center)
                }
                else if(index === 5){
                  // 蜂鸣器开关
                  Row() {
                    Toggle({ type: ToggleType.Switch, isOn: this.pwm2_start })
                      .size({ width: '35%', height: '35%' })
                      .onChange((isChecked) => {
                        this.pwm2_start = !this.pwm2_start
                        if(isChecked) {
                          prompt.showToast({
                            message: "打开蜂鸣器",
                          })
                          try {
                            ifpsnapidemo.SetPwmStatus(2, 90)
                            console.log(`setPwmStatus ${isChecked}}`)
                          }catch(err) {
                            console.log('ERROR: setPwmStatus failed' + err)
                          }
                        }
                        else {
                          prompt.showToast({
                            message: "关闭蜂鸣器",
                          })
                          try {
                            ifpsnapidemo.SetPwmStatus(2, -90)
                            console.log(`SetPwmStatus ${isChecked}}`)
                          }catch(err) {
                            console.log('ERROR: SetPwmStatus failed' + err)
                          }
                        }
                      })
                  }
                  .width('100%')
                  .padding({ top: '3%' })
                  .justifyContent(FlexAlign.Center)
                }
              }
              .height('65%').width('100%')
            }
            .height('100%')
            .width('100%')
            .backgroundColor('#f7f7f7')
            .borderRadius(10)
          }
        })
      }
      .columnsTemplate('1fr 1fr')
      .rowsTemplate('1fr 1fr 1fr')
      .columnsGap(10)
      .rowsGap(10)
      .height('80%')
      .width('60%')
    }
    .backgroundColor(Color.White)
  }
  @Builder buildLogo() {
    Column(){
      Column(){
        // 标题
        Text('OpenHarmony')
          .fontColor(Color.Blue)
          .fontSize(this.dpi / 2)
          .fontStyle(FontStyle.Italic)
          .fontWeight(FontWeight.Bold)
          .fontFamily('Arial')
          .width('100%')
          .textAlign(TextAlign.Center)
          .padding({top:'30%'})
          .margin({bottom:10})

        Text('智能消防系统')
          .fontColor(Color.Black)
          .fontSize(this.dpi / 2)
          .fontFamily('SimHei')
          .width('100%')
          .textAlign(TextAlign.Center)
          .padding( {bottom: '20%'} )
      }
      .width('90%')
      .height('80%')
      .backgroundColor('#f5f5f5')
      .borderRadius(10)
    }
    .width('40%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
    .alignItems(HorizontalAlign.Center)
    .backgroundColor(Color.White)
  }
  @Builder buildHomePage() {
    Row(){
      this.buildLogo()
      Column(){
        Column(){
          Text('消防监控系统目前')
            .fontColor(Color.Black)
            .fontSize(this.dpi / 4)
            .textAlign(TextAlign.Center)
          Text(getStatusText(this.systemStatus))
            .fontColor(getTextColor(this.systemStatus))
            .fontSize(this.dpi / 2)
            .fontWeight(FontWeight.Bold)
            .padding({top: '5%'})
        }
        .backgroundColor(getStatusBackgroundColor(this.systemStatus))
        .width('90%')
        .height('50%')
        .justifyContent(FlexAlign.Center)
        .alignItems(HorizontalAlign.Center)
        .borderRadius(10)
        .margin( {top: '2%'} )

        // 报警按钮
        Button('一键报警', { type: ButtonType.Normal, stateEffect: true })
          .onClick(() => {
            // 报警弹窗
            AlertDialog.show(
              {
                title: '警告',
                message: '即将报警，是否确认？',
                autoCancel: true,
                alignment: DialogAlignment.Bottom,
                gridCount: 4,
                offset: { dx: 0, dy: -20 },
                primaryButton: {
                  value: '取消',
                  action: () => {
                    console.info('取消报警')
                  }
                },
                secondaryButton: {
                  value: '确认',
                  action: () => {
                    console.info('已报警')
                  }
                },
                cancel: () => {
                  console.info('Closed callbacks')
                }
              }
            )
          })
          .width('90%')
          .height('35%')
          .fontSize(this.dpi / 3)
          .fontColor(Color.Red)
          .backgroundColor(Color.Yellow)
          .borderRadius(10)
          .margin( {top: '5%'} )
      }
      .width('60%')
      .height('80%')
      .alignItems(HorizontalAlign.Center)
    }
    .backgroundColor(Color.White)
  }

  build() {
    Column(){

      // 导航栏组件
      Tabs({ barPosition: BarPosition.End, controller: this.controller_tabs }){
        // 首页
        TabContent(){
          this.buildHomePage()
        }
        .tabBar('首页')

        // 消防页面
        TabContent(){
          Column(){
            this.buildGrid()
          }
        }
        .tabBar('消防')

        // 日志页面
        TabContent(){
          Column(){
            // 日志信息列表
            this.buildLogList()
          }
          .backgroundColor(Color.White)
          .width('100%')
          .height('100%')
        }
        .tabBar('日志')
      }
      .barWidth('100%')
      .barHeight('15%')
      .width('100%')
      .height('100%')
      .backgroundColor(0xF5F5F5)
      .vertical(false)
    }
  }
}

@CustomDialog
struct CustomDialogValueInput {
  @Link inputValue: number
  private textValue: number
  controller?: CustomDialogController

  build() {
    Column() {
      Text('更新警报阈值：').fontSize(20).margin({ top: 10, bottom: 10 })
      TextInput({ placeholder: '', text: this.inputValue.toString() }).height(60).width('90%')
        .type(InputType.Number)
        .onChange((value: string) => {
          this.textValue = Number(value)
        })
      Text('是否确认修改?').fontSize(16).margin({ bottom: 10 }).padding({top: 20})
      Flex({ justifyContent: FlexAlign.SpaceAround }) {
        Button('取消')
          .onClick(() => {
            this.controller.close()
          }).backgroundColor(0xffffff).fontColor(Color.Black)
        Button('确认')
          .onClick(() => {
            if (this.controller != undefined) {
              this.inputValue = this.textValue
              this.controller.close()
              prompt.showToast({
                message: `成功更新警报阈值为${this.inputValue}`,
              })
            }
          }).backgroundColor(0xffffff).fontColor(Color.Red)
      }.margin({ bottom: 10 })
    }.borderRadius(10)
  }
}

function getTextColor(x: number){
  if(x === 1)
    return Color.Red
  if(x === 2)
    return '#ffca3230'
  if(x === 3)
    return '#c55c2d'
  else
    return Color.Green
}
function getStatusBackgroundColor(x: number) {
  if(x > 0) return '#dfea50'
  else return '#98ff98'
}
function getStatusText(x: number){
  if(x === 0) return '正常'
  else return '异常'
}
function getStatusTextColor(x: Boolean){
  if(x) return Color.Red
  else return Color.Green
}
function getNumberTextColor(x: number, y: number){
  if(x > y) return Color.Red
  return '#708090'
}

function getTimeString(): any {
  const promise = systemDateTime.getCurrentTime(false)
  return promise.then(timestamp => {
    const currentTime: Date = new Date(timestamp);
    const year: number = currentTime.getFullYear();
    const month: number = currentTime.getMonth() + 1;
    const day: number = currentTime.getDate();
    const hours: number = currentTime.getHours();
    const minutes: number = currentTime.getMinutes();
    const seconds: number = currentTime.getSeconds();

    // 将数字转换为两位数的字符串形式（补0）
    const padZero = (num: number): string => {
      return num < 10 ? '0' + num : num.toString()
    }

    // 构建字符串
    const formattedTime: string = `${year}-${padZero(month)}-${padZero(day)} ${padZero(hours)}:${padZero(minutes)}:${padZero(seconds)}`;
    return formattedTime
  }).catch(err => {
    throw err
  })
}

// 函数：读取温湿度数据
function readTemperatureAndHumidity() {
  // 调用传感器软重启函数
  const softResetResult = ifpsnapidemo.Sht3xSoftReset();
  console.log('Sensor Sht3xSoft Reset Result:', softResetResult);

  // 设置传感器测量模式
  const mps = 2; // 2Hz
  const repeatability = 1; // 中刷新率
  const modeSetResult = ifpsnapidemo.Sht3xModeSet(mps, repeatability);
  console.log('Sensor Sht3xMode Set Result:', modeSetResult);

  // 读取温湿度数据
  const readDataResult = ifpsnapidemo.Sht3xReadData();
  console.log('Read Sht3xReadData Result:', readDataResult);

  // 使用下列三个接口时必须先调用ReadData
  if (readDataResult === 0) {
    // 读取摄氏温度
    const temperatureC = ifpsnapidemo.Sht3xReadTemperatureC();

    // 读取华氏温度
    const temperatureF = ifpsnapidemo.Sht3xReadTemperatureF();

    // 读取湿度
    const humidity = ifpsnapidemo.Sht3xReadHumidity();

    return {
      temperatureC,
      temperatureF,
      humidity
    };
  } else {
    console.log('Failed to read data. Please check the sensor connection and try again.');
    return false;
  }
}

function getPPMValue() {
  // 获取 ADC 值
  let adcValue = 20
  try {
    adcValue = ifpsnapidemo.Mq2GetAdcValue();
  }catch (err) {
    console.log('ERROR: Mq2GetAdcValue failed.');
    return false
  }

  // 检查是否需要权限
  if (adcValue < 0) {
    let permissionGranted = ifpsnapidemo.Mq2GetPermission();

    // 检查权限获取结果
    if (!permissionGranted) {
      console.error("无法获取 ADC 值并且权限获取失败");
      return null; // 返回 null 表示无法获取 ppm 值
    }

    // 再次尝试获取 ADC 值
    try {
      adcValue = ifpsnapidemo.Mq2GetAdcValue();
    }catch (err) {
      console.log('ERROR: Mq2GetAdcValue failed.');
      return false
    }

    // 检查是否成功获取 ADC 值
    if (adcValue < 0) {
      console.error("无法获取 ADC 值");
      return null; // 返回 null 表示无法获取 ppm 值
    }
  }

  // 计算电压值（伏特）
  let voltage = adcValue * (3.3 / 4096);

  // 使用特定的公式将电压转换为 ppm 值
  // 这里使用一个简单的示例，实际上应该根据传感器的特性来计算 ppm 值
  let ppmValue = voltage * 10; // 假设一个简单的线性关系

  return ppmValue;
}
function getFIREValue() {
  // 获取 ADC 值
  let adcValue = 20
  try {
    adcValue = ifpsnapidemo.FlameSensorGetAdcValue();
  }catch (err) {
    console.log('ERROR: FlameSensorGetAdcValue failed.');
    return false
  }

  // 检查是否需要权限
  if (adcValue < 0) {
    let permissionGranted = ifpsnapidemo.FlameSensorGetPermission();

    // 检查权限获取结果
    if (!permissionGranted) {
      console.error("无法获取 ADC 值并且权限获取失败");
      return null; // 返回 null 表示无法获取 ppm 值
    }

    // 再次尝试获取 ADC 值
    try {
      adcValue = ifpsnapidemo.FlameSensorGetAdcValue();
    }catch (err) {
      console.log('ERROR: FlameSensorGetAdcValue failed.');
      return false
    }

    // 检查是否成功获取 ADC 值
    if (adcValue < 0) {
      console.error("无法获取 ADC 值");
      return null; // 返回 null 表示无法获取 fire 值
    }
  }

  // 计算电压值（伏特）
  let voltage = adcValue * (3.3 / 4096);

  // 使用特定的公式将电压转换为 fire 值
  // 这里使用一个简单的示例，实际上应该根据传感器的特性来计算 fire 值
  let fireValue = voltage * 10.; // 假设一个简单的线性关系

  return fireValue;
}