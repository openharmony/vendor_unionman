import { DEFAULT } from '@ohos/hypium'

@CustomDialog
export default  struct DateDialog{
  controller: CustomDialogController
  date:Date= new Date()
  build(){
    Column(){
      DatePicker({
        start: new Date('2024-11-11'),
        end: new Date(),
        selected: this.date
      })
        .onChange((value: DatePickerResult) => {
          if (value.year !== undefined && value.month !== undefined && value.day !== undefined) { // 确保值不为 undefined
            this.date.setFullYear(value.year, value.month, value.day); // 添加 day 参数
          }
        })
      Row({space : 20}){
        Button('取消')
          .width(120)
          .backgroundColor($r('app.color.app_color_bar_normal'))
          .onClick(()=>{
            this.controller.close()
          })
        Button('确定')
          .width(120)
          .backgroundColor($r('app.color.app_color_bar_normal'))
          .onClick(()=>{
          AppStorage.SetOrCreate('date',this.date.getTime())
this.controller.close()
          })
      }

    }
.padding(12)
  }

}