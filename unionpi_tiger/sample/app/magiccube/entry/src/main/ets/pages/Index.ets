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
  @State axisSelected: number = 0;
  private axisSelects: Array<SelectOption> = [{ value: 'X轴' }, { value: 'Y轴' }, { value: 'Z轴' }];
  private axisValue: Array<number> = [magiccube.Axis.X, magiccube.Axis.Y, magiccube.Axis.Z];
  @State dirSelected: number = 0;
  private dirSelects: Array<SelectOption> = [{ value: '左面' }, { value: '中间面' }, { value: '右面' }];
  private dirValue: Array<number> = [magiccube.Direction.Left, magiccube.Direction.Middle, magiccube.Direction.Right];

  build() {
    Row() {
      XComponent({ id: this.xcomponentId, type: 'surface', libraryname: 'magiccube' })
        .gesture(PanGesture(this.panOption)
          .onActionUpdate((event) => {
            magiccube.updateAngle(event.offsetX, event.offsetY);
          })
        )
        .width('50%')
        .height('100%')
      Column() {
        Text("Opengl魔方sample")
          .fontSize("20vp")
          .fontWeight(FontWeight.Bold)
          .textAlign(TextAlign.Center)
        Divider()
          .strokeWidth(8)
          .color('#F1F3F5')
        Text("扭动魔方")
        Row() {
          Select(this.dirSelects)
            .selected(this.dirSelected)
            .value(this.dirSelects[this.dirSelected].value.toString())
            .onSelect((index) => {
              this.dirSelected = index;
            })
          Text("绕")
          Select(this.axisSelects)
            .selected(this.axisSelected)
            .value(this.axisSelects[this.axisSelected].value.toString())
            .onSelect((index) => {
              this.axisSelected = index;
            })
          Button("旋转")
            .onClick((event) => {
              magiccube.twist(this.axisValue[this.axisSelected], this.dirValue[this.dirSelected]);
            })
        }
      }
      .width('50%')
      .height('100%')
    }
    .width('100%')
    .height('100%')
  }
}
