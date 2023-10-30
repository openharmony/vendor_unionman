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
import hilog from '@ohos.hilog';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from '@ohos/hypium'
import i2cnapidemo from '@ohos.i2cnapidemo';

const BUNDLE = 'I2C_APP';
const TAG = 'i2cAppTest';
const DOMAIN = 0xF811;

export default function abilityTest() {
  describe('ActsAbilityTest', function () {
    // Defines a test suite. Two parameters are supported: test suite name and test suite function.
    beforeAll(function () {
      // Presets an action, which is performed only once before all test cases of the test suite start.
      // This API supports only one parameter: preset action function.
    })
    beforeEach(function () {
      // Presets an action, which is performed before each unit test case starts.
      // The number of execution times is the same as the number of test cases defined by **it**.
      // This API supports only one parameter: preset action function.
    })
    afterEach(function () {
      // Presets a clear action, which is performed after each unit test case ends.
      // The number of execution times is the same as the number of test cases defined by **it**.
      // This API supports only one parameter: clear action function.
    })
    afterAll(function () {
      // Presets a clear action, which is performed after all test cases of the test suite end.
      // This API supports only one parameter: clear action function.
    })
    it(BUNDLE + 'SoftResetAbility_001', 0, function () {
      // Defines a test case. This API supports three parameters: test case name, filter parameter, and test case function.
      hilog.info(DOMAIN, TAG, '%{public}s', 'SoftReset test begin');
      let a = i2cnapidemo.SoftReset();
      // Defines a variety of assertion methods, which are used to declare expected boolean conditions.
      expect(0).assertEqual(a)
      hilog.info(DOMAIN, TAG, '%{public}s', 'SoftReset test end');
    })

    it(BUNDLE + 'ModeSetAbility_001', 1, function () {
      // Defines a test case. This API supports three parameters: test case name, filter parameter, and test case function.
      hilog.info(DOMAIN, TAG, '%{public}s', 'ModeSet test begin');
      let MAX_Mps_Num = 5;
      let MAX_Rate_Num = 3;
      for (let Mps = 0; Mps < MAX_Mps_Num; Mps++) {
        for (let rate = 0; rate < MAX_Rate_Num; rate++) {
          let a = i2cnapidemo.ModeSet(Mps, rate);
          // Defines a variety of assertion methods, which are used to declare expected boolean conditions.
          expect(0).assertEqual(a)
        }
      }
      hilog.info(DOMAIN, TAG, '%{public}s', 'ModeSet test end');
    })

    it(BUNDLE + 'ReadDataAbility_001', 2, function () {
      // Defines a test case. This API supports three parameters: test case name, filter parameter, and test case function.
      hilog.info(DOMAIN, TAG, '%{public}s', 'ReadData test begin');

      let a = i2cnapidemo.ReadData();
      // Defines a variety of assertion methods, which are used to declare expected boolean conditions.
      expect(0).assertEqual(a)
      hilog.info(DOMAIN, TAG, '%{public}s', 'ReadData test end');
    })

    it(BUNDLE + 'ReadTemperatureCAbility_001', 3, function () {
      // Defines a test case. This API supports three parameters: test case name, filter parameter, and test case function.
      hilog.info(DOMAIN, TAG, '%{public}s', 'ReadTemperatureC test begin');

      let a = i2cnapidemo.ReadTemperatureC();
      // Defines a variety of assertion methods, which are used to declare expected boolean conditions.
      expect(a).assertLarger(0)
      hilog.info(DOMAIN, TAG, '%{public}s', 'ReadTemperatureC test end');
    })

    it(BUNDLE + 'ReadTemperatureFAbility_001', 4, function () {
      // Defines a test case. This API supports three parameters: test case name, filter parameter, and test case function.
      hilog.info(DOMAIN, TAG, '%{public}s', 'ReadTemperatureF test begin');

      let a = i2cnapidemo.ReadTemperatureF();
      // Defines a variety of assertion methods, which are used to declare expected boolean conditions.
      expect(a).assertLarger(0)
      hilog.info(DOMAIN, TAG, '%{public}s', 'ReadTemperatureF test end');
    })

    it(BUNDLE + 'ReadHumidityAbility_001', 5, function () {
      // Defines a test case. This API supports three parameters: test case name, filter parameter, and test case function.
      hilog.info(DOMAIN, TAG, '%{public}s', 'ReadHumidity test begin');

      let a = i2cnapidemo.ReadHumidity();
      // Defines a variety of assertion methods, which are used to declare expected boolean conditions.
      expect(a).assertLarger(0)
      hilog.info(DOMAIN, TAG, '%{public}s', 'ReadHumidity test end');
    })

  })
}