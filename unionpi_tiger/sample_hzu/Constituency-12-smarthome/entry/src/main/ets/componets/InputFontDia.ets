@Component
struct InputFont{
  @State isFloatHint: boolean = false
  @State textValue: string = ''
  isEditing = false
  placeholder: string
  typeInput:InputType = InputType.Normal

  build(){
    Stack(){
      TextInput({text:this.textValue})
        .width('100%')
        .height('100%')
        .borderRadius(0)
        .borderColor('#86C7CC')
        .borderWidth({bottom: 1})
        .backgroundColor(Color.Transparent)
        .type(this.typeInput)
        .showPasswordIcon(false)
        .onEditChange((isEditing)=>{
          this.isEditing = isEditing
          this.isFloatHint = this.isEditing || this.textValue !==''
        })
        .onChange((value)=>{
          this.textValue = value
          this.isFloatHint = this.isEditing || this.textValue !== ''
        })
      Text(this.placeholder)
        .enabled(false)
        .fontColor(this.isFloatHint ?  '#5D7DB3' : '#E1E4EA')
        .position({x: 0, y: this.isFloatHint ? 0 : '50%'})
        .translate({x: 15, y: '-50%'})
        .animation({duration:100, curve: Curve.Linear})

      Line()
        .width('100%')
        .height(1)
        .backgroundColor('#5D7DB3')
        .position({x: 0, y: 44})
        .scale({x: this.isFloatHint ? 1 : 0, centerX: 0})
        .animation({duration: 300, curve: Curve.Linear})

      if (this.textValue){
        Image($r('app.media.cancel'))
          .width(15)
          .height(15)
          .border({width: 1, radius: 15, color: '#fffffff'})
          .position({x: '90%', y: '50%'})
          .translate({y: '-50%'})
          .onClick(()=>{
            this.textValue = ''
          })
      }
    }.width('100%').height(45)
  }
}