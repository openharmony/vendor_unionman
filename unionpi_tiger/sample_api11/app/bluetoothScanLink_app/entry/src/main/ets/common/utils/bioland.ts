/* Copyright 2023 Unionman Technology Co., Ltd.
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

// 将一个 ArrayBuffer 转换为十六进制字符串
import ble from '@ohos.bluetooth.ble';

function buf2hex(buffer: ArrayBuffer): string {
  return Array.from(new Uint8Array(buffer), x => ('00' + x.toString(16)).slice(-2)).join('');
}

// 解析数据并返回特定数值
export default function BiolandParseData(res: ble.BLECharacteristic): number {
  // 将 ArrayBuffer 转换为十六进制字符串
  const array: string = buf2hex(res.characteristicValue);

  // 从十六进制字符串中提取指定部分
  const testValue1 = array.substring(18, 2);
  const testValue2 = array.substring(20, 2);

  // 拼接并转换为十六进制数值
  let testValue3: string = '0x' + testValue2 + testValue1;

  // 将十六进制值转换为整数，除以 10 后保留 1 位小数

  return (parseInt(testValue3, 16) / 10) / 10;
}

