/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
 */

import { timeConvert } from '../utils/TimeUtils';
import avPlayManage from '../videomanager/AvPlayManager';

@Component
export struct VideoOperate {
  @Link currentTime: number;
  @Link durationTime: number;
  // @Link isSwiping: boolean;
  @Link avPlayManage: avPlayManage;
  @Link flag: boolean;
  // @Link XComponentFlag: boolean;
  @StorageLink('sliderWidth') sliderWidth: string = '';

  build() {
    Row() {
      Row() {
        Image(this.flag ? $r("app.media.ic_video_play") : $r("app.media.ic_video_pause")) // 暂停/播放
          .id('play')
          .width($r('app.float.size_40'))
          .height($r('app.float.size_40'))
          .onClick(() => {
            if (this.flag) {
              this.avPlayManage.videoPause();
              this.flag = false;
            } else {
              this.avPlayManage.videoPlay();
              this.flag = true;
            }
          })

        // 左侧时间
        Text(timeConvert(this.currentTime))
          .fontColor(Color.White)
          .textAlign(TextAlign.End)
          .fontWeight(FontWeight.Regular)
          .margin({ left: $r('app.float.size_10') })
      }

      Row() {
        Slider({
          value: this.currentTime,
          min: 0,
          max: this.durationTime,
          style: SliderStyle.OutSet
        })
          .id('Slider')
          .blockColor(Color.White)
          .trackColor(Color.Gray)
          .selectedColor($r("app.color.slider_selected"))
          .showTips(false)
          .onChange((value: number, mode: SliderChangeMode) => {
            if (mode == SliderChangeMode.Begin) {
              this.avPlayManage.videoPause();
            }
            this.avPlayManage.videoSeek(value);
            this.currentTime = value;
            if (mode == SliderChangeMode.End) {
              this.flag = true;
              this.avPlayManage.videoPlay();
            }
          })
      }
      .layoutWeight(1)

      Row() {
        // 右侧时间
        Text(timeConvert(this.durationTime))
          .fontColor(Color.White)
          .fontWeight(FontWeight.Regular)
      }
    }
    .justifyContent(FlexAlign.Center)
    .padding({ left: $r('app.float.size_25'), right: $r('app.float.size_30') })
    .width('100%')
  }
}