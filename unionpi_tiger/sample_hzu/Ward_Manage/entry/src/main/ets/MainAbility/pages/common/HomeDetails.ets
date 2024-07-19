

@Entry
@Component
export struct HomeDetails{

  @State  private image: string= "common/images/home_out.png"

  @State  private selectColor: Color = Color.Red
 @State private isSelected: boolean = true;


  build(){


    Column(){
      Image(this.image)
        .objectFit(ImageFit.Contain)
        .width(80)
        .height(80)

      Text( 'ENTRY',)
        .fontSize(40).fontWeight(FontWeight.Bold)
      Text( 'OPEN',)
        .fontSize(40).fontWeight(FontWeight.Bold).fontColor(Color.Pink)

    }.width(300).height(260).border({
      width:8,
      radius:20,
      color: this. isSelected?Color.Red: Color.White
    }).onClick(()=>{
      this.isSelected=!this.isSelected;
    }).margin(10)
  }
}

