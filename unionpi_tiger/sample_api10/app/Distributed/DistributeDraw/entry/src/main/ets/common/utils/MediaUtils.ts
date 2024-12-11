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
import fs from '@ohos.file.fs';
import image from '@ohos.multimedia.image';
import common from '@ohos.app.ability.common';
import promptAction from '@ohos.promptAction';
import photoAccessHelper from '@ohos.file.photoAccessHelper';
import DateTimeUtil from './DateTimeUtil';
import Logger from './Logger';

const TAG: string = 'MediaUtils';

export default class MediaUtils {
  private phAccessHelper: photoAccessHelper.PhotoAccessHelper;

  constructor(context: common.UIAbilityContext) {
    this.phAccessHelper = photoAccessHelper.getPhotoAccessHelper(context);
  }

  async createAndGetFile() {
    let asset_uri: string
    try {
      let photoType: photoAccessHelper.PhotoType = photoAccessHelper.PhotoType.IMAGE;
      let extension: string = 'jpg';
      let dateTimeUtil = new DateTimeUtil()
      let name = `${dateTimeUtil.getDate()}_${dateTimeUtil.getTime()}`
      let displayName = `DistributeDraw_${name}`
      let options: photoAccessHelper.CreateOptions = {
        title: displayName
      }
      asset_uri = await this.phAccessHelper.createAsset(photoType, extension, options);
      console.info('createAsset uri' + asset_uri);
      console.info('createAsset successfully');
    } catch (err) {
      console.error(`createAsset failed, error: ${err.code}, ${err.message}`);
    }
    return asset_uri
  }

  async savePicture(data) {
    Logger.info(TAG, `savePicture`)
    let packOpts: image.PackingOption = {
      format: "image/jpeg", quality: 100
    }
    let imagePackerApi = image.createImagePacker()
    let arrayBuffer = await imagePackerApi.packing(data, packOpts)

    let uri = await this.createAndGetFile()
    // 使用uri打开文件，可以持续写入内容，写入过程不受时间限制
    let file = await fs.open(uri, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE);
    try {
      await fs.write(file.fd, arrayBuffer);
    } catch (err) {
      Logger.error(TAG, `write failed, code is ${err.code}, message is ${err.message}`)
    }
    await fs.close(file.fd);
    Logger.info(TAG, `write done`)
    promptAction.showToast({
      message: '图片保存成功', duration: 1000
    });
  }
}