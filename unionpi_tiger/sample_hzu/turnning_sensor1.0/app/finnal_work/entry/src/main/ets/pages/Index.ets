@Entry
@Component
struct Index {
  @State message: string = '当前转动小格数：';
  @State turn_click: number = 0;
  build() {
    RelativeContainer() {
      Text(this.message + this.turn_click)
        .fontSize(38)
        .fontWeight(FontWeight.Bold)
        .alignRules({
          center: { anchor: '__container__', align: VerticalAlign.Center },
          middle: { anchor: '__container__', align: HorizontalAlign.Center }
        })
    }
    .height('100%')
    .width('100%')
  }
}