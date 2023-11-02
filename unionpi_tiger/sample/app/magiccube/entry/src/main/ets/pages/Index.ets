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

import magiccube from 'libmagiccube.so'

@Entry
@Component
struct Index {
  private xcomponentId = 'magiccube';
  private panOption: PanGestureOptions = new PanGestureOptions({ direction: PanDirection.All });
  build() {
    Row() {
      Column() {
        XComponent({ id: this.xcomponentId, type: 'surface', libraryname: 'magiccube' })
          .gesture(PanGesture(this.panOption)
            .onActionUpdate((event)=>{
              magiccube.updateAngle(event.offsetX,event.offsetY);
            })
          )
      }
      .width('50%')
      .height('100%')
      Text("Opengl魔方sample")
        .fontSize("20vp")
        .fontWeight(FontWeight.Bold)
        .textAlign(TextAlign.Center)
        .width('50%')
        .height('100%')
    }
    .width('100%')
    .height('100%')
  }
}
