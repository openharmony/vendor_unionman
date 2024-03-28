/*
 * Copyright (c) 2024 Unionman Technology Co., Ltd.
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
//loadingComponent_part1.ets
@Component

export default struct loadingComponent_part1 {
  private imageWidth: number | Resource = 0
  private imageHeight: number | Resource = 0
  private images :  ImageFrameInfo[]=[]
  private imagePath : string='/images_black//4337e81bd5784c21984de88f01c5e30e.gif_p'
  @State textContent: string="LOADING"
  private text_index: number =0
  aboutToAppear(){
    for (let i = 0; i <=44; i++) {
      this.images.push(
        {src:this.imagePath+i+".png", duration:100}
      )
    }

    setInterval(() => {
      this.text_index++
      if (this.text_index>6) {
        this.textContent='LOADING'
        this.text_index=0
      }else{
        this.textContent += "~"
      }
    }, 200)
  }

  build() {
    Column() {
      ImageAnimator()
        .images(this.images)
        .size({width: "50%", height: '80%'})
        .iterations(-1)
        .fixedSize(true)
        .reverse(true)
        .fillMode(FillMode.None)
        .state(AnimationStatus.Running)
      Text(this.textContent).fontSize(30)
    }
  }
}