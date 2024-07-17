import { NoteModel } from '../model/NoteDataModel'
@Component
export struct NoteListItem {
  private note?: NoteModel = undefined;
  private noteID?: number = undefined;
  private deleteNoteCallBack: (note: NoteModel) => void = (note: NoteModel) => {};
  build() {
    Column() {
      if (this.note!.title === '' && this.note!.content === '') {
        Image($r('app.media.add'))
          .id('add')
          .objectFit(ImageFit.Contain)
          .width(50).height('100%')
      } else {
        Stack({ alignContent: Alignment.TopEnd }) {
          Column() {
            Text(this.note!.title)
              .fontColor(Color.Black)
              .fontSize(20)
              .height('50%')
              .width('100%')
              .textAlign(TextAlign.Center)
              .margin({ top: 20 })
            Text(this.note!.content)
              .fontColor(Color.Gray)
              .fontSize(18)
              .width('100%')
              .textAlign(TextAlign.Center)
          }
          .width('100%').height('100%')
          Image($r('app.media.delete'))
            .id('del' + this.noteID)
            .width('30%')
            .height('30%')
            .objectFit(ImageFit.Contain)
            .onClick(() => {
              this.deleteNoteCallBack(this.note)
            })
        }
      }
    }
    .width('100%')
    .height(120)
    .backgroundColor('#DFDFF5')
    .border({ width: 5, color: '#F5F5F5', radius: 10 })
  }
}