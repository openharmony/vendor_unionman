import { DeviceDataModel } from '../model/DeviceDataModel'
@CustomDialog
export struct DeviceDialog {
  controller: CustomDialogController
  private deviceList: Array<DeviceDataModel> = []
  private selectedIndex: number = 0
  private handleSelectDevice: (selectedIndex: number) => void = (selectedIndex: number)=>{};
  build() {
    Column() {
      Text($r('app.string.check_device'))
        .fontSize(20)
        .width('100%')
        .textAlign(TextAlign.Center)
        .fontColor(Color.Black)
        .fontWeight(FontWeight.Bold)
      List() {
        ForEach(this.deviceList, (item: DeviceDataModel, index: number) => {
          ListItem() {
            Row() {
              Text(item.deviceName)
                .fontSize(20)
                .width('90%')
                .fontColor(Color.Black)
              if (this.deviceList.indexOf(item) === this.selectedIndex) {
                Image($r('app.media.checked'))
                  .width('8%')
                  .objectFit(ImageFit.Contain)
              } else {
                Image($r('app.media.uncheck'))
                  .width('8%')
                  .objectFit(ImageFit.Contain)
              }
            }
            .height(55)
            .onClick(() => {
              this.handleSelectDevice(index)
            })
          }
        }, (item: DeviceDataModel) => item.deviceName)
      }
      .layoutWeight(1)
      Button() {
        Text($r('app.string.cancel'))
          .fontColor('#0D9FFB')
          .width('90%')
          .textAlign(TextAlign.Center)
          .fontSize(20)
      }
      .id('cancel')
      .type(ButtonType.Capsule)
      .backgroundColor(Color.White)
      .onClick(() => {
        this.controller.close()
      })
    }
    .backgroundColor(Color.White)
    .border({ color: Color.White, radius: 20 })
    .padding(10)
  }
}