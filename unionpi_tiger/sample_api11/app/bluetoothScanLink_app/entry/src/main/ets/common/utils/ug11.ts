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

import ble from '@ohos.bluetooth.ble';

function ab2hex(buffer: ArrayBuffer): string {
  return Array.from(new Uint8Array(buffer), x => ('00' + x.toString(16)).slice(-2)).join('');
}

function formatNumber(n: number) {
  const num = n.toString()
  return num[1] ? num : '0' + num
}

export default function Ug11ParseData(res: ble.BLECharacteristic) {
  let resValue = ab2hex(res.characteristicValue);
  let startStr = (resValue.substring(0, 4) == "534e" || resValue.substring(0, 4) == "534E");
  let array = Array.from(new Uint8Array(res.characteristicValue));
  let resObject: ResObject = {}
  if (array[5] === 4 || array[5] === 14) {
    // 测试检测项目  0是血糖 1是尿酸
    if (array[5] === 4) {
      resObject.code = '04'
    }
    if (array[5] === 14) {
      resObject.code = '0e'
    }
    let year = '20' + array[6]
    let month = formatNumber(array[7])
    let day = formatNumber(array[8])
    let h = formatNumber(array[9])
    let m = formatNumber(array[10])
    let seconds = formatNumber(array[11]);
    let testType = array[14]
    let hight = array[15] & 0xff
    let low = array[16] & 0xff

    if (testType == 0) {
      let result: string | number = (Math.round((hight << 8) + low)) / 10;
      if (result < 1.1) {
        result = "L";
      } else if (result > 33.3) {
        result = "H";
      }
      // object.GLU = result;
      resObject.data = {
        data: array,
        str: resValue,
        result: result,
        unit: 'mmol/L',
        formatTime: year + '-' + month + '-' + day + ' ' + h + ':' + m + ':' + seconds,
        time: new Date(year + '/' + month + '/' + day + ' ' + h + ':' + m + ':' + seconds).getTime(),
        type: 'GLU'
      }
    } else {
      let result: string | number = (Math.round((hight << 8) + low));
      if (result < 181) {
        result = "L";
      } else if (result > 1188) {
        result = "H";
      }
      resObject.data = {
        result: result,
        data: array,
        str: resValue,
        unit: 'μmol/L',
        formatTime: year + '-' + month + '-' + day + ' ' + h + ':' + m + ':' + seconds,
        time: new Date(year + '/' + month + '/' + day + ' ' + h + ':' + m + ':' + seconds).getTime(),
        type: 'UA'
      }

    }
    return resObject
  }
}

// 定义 GLU 数据的类型
interface GluData {
  result: Record<string, string | number> // 结果可以是字符串或数字
  unit: Record<string, string>; // 单位为字符串类型
}

// 定义 sendData 数据结构的类型
interface SendData {
  data: Record<string, GluData>
}


interface ResObjectData {
  data: number[];
  str: string;
  result: string | number;
  unit: string;
  formatTime: string;
  time: number;
  type: string;
}

// 解析数据
interface ResObject {
  code?: string;
  data?: ResObjectData
}