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
import { onlywirte } from "./widgets"
import { TempInfo, TempInfoTable } from '../common/database/TempInfoTable';
import { GlobalContext } from './GlobalContext';
import { tested } from '../pages/myMenu'

@Entry
@Component
struct Welcome {
  @State message: string = '欢迎使用app'
  @State mode:boolean=true;//初始为摄氏度模式
  @State TempInfoobj:TempInfoTable=new TempInfoTable()//新建一个tempinfotable对象
  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .onClick(()=>{
            GlobalContext.getContext().setObject('tempInfo', this.TempInfoobj);
            router.push({ url: 'pages/myMenu',
              params: {
                mode: this.mode,
              }as tested,
            })
          })
        //onlywirte({TempInfoobj:$TempInfoobj});
      }
      .width('100%')
    }
    .height('100%')
    .width('100%')
    .backgroundImage($r('app.media.bg3')).backgroundImageSize(ImageSize.Cover)
  }
}