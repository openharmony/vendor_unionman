/*
 * Copyright 2023 Unionman Technology Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

import photoAccessHelper from '@ohos.file.photoAccessHelper';

class BasicDataSource implements IDataSource {
  private listeners: DataChangeListener[] = []
  private originDataArray: photoAccessHelper.PhotoAsset[] = [];

  public totalCount(): number {
    return 0
  }

  public getData(index: number): photoAccessHelper.PhotoAsset {
    return this.originDataArray[index];
  }

  registerDataChangeListener(listener: DataChangeListener): void {
    if (this.listeners.indexOf(listener) < 0) {
      this.listeners.push(listener)
    }
  }

  unregisterDataChangeListener(listener: DataChangeListener): void {
    const pos = this.listeners.indexOf(listener);
    if (pos >= 0) {
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
}

export default class MediaDataSource extends BasicDataSource {
  private dataArray: photoAccessHelper.PhotoAsset[] = []

  constructor(data: photoAccessHelper.PhotoAsset[]) {
    super()
    this.dataArray = data
  }

  public totalCount(): number {
    return this.dataArray.length
  }

  public getData(index: number): photoAccessHelper.PhotoAsset {
    return this.dataArray[index]
  }

  public addData(index: number, data: photoAccessHelper.PhotoAsset): void {
    this.dataArray.splice(index, 0, data)
    this.notifyDataAdd(index)
  }

  public pushData(data: photoAccessHelper.PhotoAsset): void {
    this.dataArray.push(data)
    this.notifyDataAdd(this.dataArray.length - 1)
  }
}