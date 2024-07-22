/*
 * Copyright 2023 Unionman Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import router from '@ohos.router';

import { Header } from '../components/common/Header';
import { Home } from '../components/home/Home';


@Entry
@Component
struct Index {
  @State username: string = ''

  aboutToAppear() {
    const username: string = AppStorage.Get('user') as string
    this.username = username
    if (this.username == null || this.username == undefined) {
      router.pushUrl({
        url: 'pages/Login'
      })
    }
  }

  build() {
    Column() {
      Header({ username: $username })
        .width('100%')
        .height('10%')

      Row() {
        Home()
      }.width('100%')
      .height('90%')

    }
    .width('100%')
    .height('100%')
    .backgroundImage($r('app.media.index_bg'))
    .backgroundImageSize({
      width: '100%',
      height: '100%'
    })
  }
}