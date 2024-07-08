/* Copyright 2022 Unionman Technology Co., Ltd.
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

declare namespace TempAndHum
{
  function Soft_Reset():number;//软重起
  function SetMode(mps:number, repeatability:number):number;//设置模式
  function ReadTemp_Hum():number;//读取数据
  function GetTemperatureC():number;//读取温度摄氏度
  function GetTemperatureF():number;//华氏度
  function GetHumidity():number;//湿度
}
export default TempAndHum;