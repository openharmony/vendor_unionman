export class NoteModel {
  public title: string
  public content: string
  constructor(title: string, content: string) {
    this.title = title
    this.content = content
  }
}
export function transStrToNoteModel(str: string): Array<NoteModel> {
  let noteList: NoteModel[] = [];
  let notes: NoteModel[] = [];
  notes = JSON.parse(str);
  for (let note = 0; note < notes.length; ++note ) {
    noteList.push(new NoteModel(notes[note].title, notes[note].content));
  }
  return noteList
}