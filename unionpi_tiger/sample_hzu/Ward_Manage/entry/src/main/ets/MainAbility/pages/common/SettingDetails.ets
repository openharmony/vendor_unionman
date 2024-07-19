@Entry
@Component
export struct SettingDetails {
  @State  private image: string = 'common/images/setting.png'
  @State  private title: string = 'Maintenance\nRequests'
  @State private isSelected: boolean = true;

  build() {


  Column() {
    Image(this.image)
      .objectFit(ImageFit.Contain)
      .width(140)
      .height(120)
      .margin(20)
      .border({
        width: 12, color: this.isSelected ? Color.White : Color.Red,
        radius: 20
      })
      .onClick(() => {
        this.isSelected = !this.isSelected;
      })
    Text(this.title).fontSize(32).width(200).textAlign(TextAlign.Center)
  }
}}