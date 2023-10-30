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

import mediaLibrary from '@ohos.multimedia.mediaLibrary';
import Logger from '../model/Logger';

const TAG: string = 'MediaUtils';

export default class MediaUtils {
  private mediaList: Array<mediaLibrary.FileAsset> = []
  private mediaLib: mediaLibrary.MediaLibrary = undefined

  constructor() {
    this.mediaLib = mediaLibrary.getMediaLibrary(globalThis.context)
  }

  async getFileAssetsFromType(mediaType: number) {
    Logger.info(TAG, `getFileAssetsFromType,mediaType = ${mediaType}`)
    let fileKeyObj = mediaLibrary.FileKey
    let fetchOp = {
      selections: `${fileKeyObj.MEDIA_TYPE}=?`,
      selectionArgs: [`${mediaType}`],
    }
    let fetchFileResult = await this.mediaLib.getFileAssets(fetchOp)
    Logger.info(TAG, `getFileAssetsFromType,fetchFileResult.count = ${fetchFileResult.getCount()}`)
    if (fetchFileResult.getCount() > 0) {
      this.mediaList = await fetchFileResult.getAllObject()
      Logger.info(TAG, `getFileAssetsFromType,fetchFileResult.count = ${JSON.stringify(this.mediaList)}`)
    }
    return this.mediaList
  }
}