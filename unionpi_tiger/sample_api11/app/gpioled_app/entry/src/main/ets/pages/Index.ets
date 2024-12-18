import gpioLed from '@ohos.gpioLed';

@Entry
@Component
struct Index {
  @State pin: number = 384
  @State isOn: boolean = false

  onPageShow() {
    this.syncButtonStatus()
  }

  build() {
    Flex({ justifyContent: FlexAlign.Center, alignItems: ItemAlign.Center, direction: FlexDirection.Column }) {
      Select([{ value: 'UM_GPIO_01' },
        { value: 'UM_GPIO_02' },
        { value: 'UM_GPIO_03' },
        { value: 'UM_GPIO_04' },
        { value: 'UM_GPIO_05' },
        { value: 'UM_GPIO_06' },
        { value: 'UM_GPIO_07' },
        { value: 'UM_GPIO_08' },
        { value: 'UM_GPIO_09' },
        { value: 'UM_GPIO_10' }])
        .selected(4) //默认选择索引为4的值（起始为0）
        .value('UM_GPIO_05')
        .font({ size: 30, weight: 400, family: 'serif', style: FontStyle.Normal })
        .selectedOptionFont({ size: 40, weight: 500, family: 'serif', style: FontStyle.Normal })
        .optionFont({ size: 30, weight: 400, family: 'serif', style: FontStyle.Normal })
        .onSelect((index: number) => {
          this.pin = 380 + index
          this.syncButtonStatus()
        })
      Toggle({ type: ToggleType.Switch, isOn: this.isOn })
        .selectedColor(0x21a3fd)
        .switchPointColor(0xe5ffffff)
        .onChange((isOn: boolean) => {
          this.isOn = isOn
          this.setLedStatus()
        })
    }
    .height('100%')
    .width('100%')
  }

  setLedStatus() {
    if (this.isOn) {
      this.addLedEffect()
    } else {
      this.removeLedEffect()
    }
  }

  //开灯
  addLedEffect() {
    gpioLed.setLedStatus(this.pin, gpioLed.LED_ON)
  }

  //关灯
  removeLedEffect() {
    gpioLed.setLedStatus(this.pin, gpioLed.LED_OFF)
  }

  //同步按钮开关状态
  syncButtonStatus() {
    gpioLed.getLedStatus(this.pin).then((result) => {
      this.isOn = Boolean(result)
    })
  }
}