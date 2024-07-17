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
export default class ItemCategory{
  public id:number//视频编号
  public name:String//视频名字
  public video:Resource//视频
  public image:Resource
  constructor(id:number,name:String,video:Resource,image:Resource){
    this.id=id
    this.name=name
    this.video=video
    this.image=image
  }
}