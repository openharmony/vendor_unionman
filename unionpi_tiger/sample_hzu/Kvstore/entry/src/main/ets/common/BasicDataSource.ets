import { NoteModel } from '../model/NoteDataModel'
import Logger from '../util/Logger'
const TAG = 'BasicDataSource'
export default class NoteDataSource {
  public dataArray: NoteModel[] = []
  constructor(data: NoteModel[]) {
    this.dataArray = data
  }
  public totalCount(): number {
    return this.dataArray.length
  }
  public getData(index: number): NoteModel {
    return this.dataArray[index]
  }
  private listeners: DataChangeListener[] = []
  registerDataChangeListener(listener: DataChangeListener): void {
    if (this.listeners.indexOf(listener) < 0) {
      Logger.info(TAG, 'add listener')
      this.listeners.push(listener)
    }
  }
  unregisterDataChangeListener(listener: DataChangeListener): void {
    const pos = this.listeners.indexOf(listener);
    if (pos >= 0) {
      Logger.info(TAG, 'remove listener')
      this.listeners.splice(pos, 1)
    }
  }
  notifyDataReload(): void {
    this.listeners.forEach(listener => {
      listener.onDataReloaded()
    })
  }
  notifyDataAdd(index: number): void {
    this.listeners.forEach(listener => {
      listener.onDataAdd(index)
    })
  }
  notifyDataChange(index: number): void {
    this.listeners.forEach(listener => {
      listener.onDataChange(index)
    })
  }
  notifyDataDelete(index: number): void {
    this.listeners.forEach(listener => {
      listener.onDataDelete(index)
    })
  }
  notifyDataMove(from: number, to: number): void {
    this.listeners.forEach(listener => {
      listener.onDataMove(from, to)
    })
  }
  public addData(index: number, data: NoteModel): void {
    this.dataArray.splice(index, 0, data)
    this.notifyDataAdd(index)
  }
  public pushData(data: NoteModel): void {
    this.dataArray.push(data)
    this.notifyDataAdd(this.dataArray.length - 1)
  }
}