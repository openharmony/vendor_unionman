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
import uart_napi from '@ohos.uart_napi';
import AbilityDelegatorRegistry from '@ohos.app.ability.abilityDelegatorRegistry';
import { Driver, ON } from '@ohos.UiTest';
import Want from '@ohos.app.ability.Want';

const BUNDLE = 'com.um.uart_napi_app';
const ABILITY_NAME = 'EntryAbility';
const TAG = 'uart_napi';
const DOMAIN = 0xF811;

export default function abilityTest() {
  describe('ActsAbilityTest', () => {
    let driver = Driver.create();
    let delegator = AbilityDelegatorRegistry.getAbilityDelegator();
    // Defines a test suite. Two parameters are supported: test suite name and test suite function.
    beforeAll(() => {
      // Presets an action, which is performed only once before all test cases of the test suite start.
      // This API supports only one parameter: preset action function.
    })
    beforeEach(() => {
      // Presets an action, which is performed before each unit test case starts.
      // The number of execution times is the same as the number of test cases defined by **it**.
      // This API supports only one parameter: preset action function.
    })
    afterEach(() => {
      // Presets a clear action, which is performed after each unit test case ends.
      // The number of execution times is the same as the number of test cases defined by **it**.
      // This API supports only one parameter: clear action function.
    })
    afterAll(() => {
      // Presets a clear action, which is performed after all test cases of the test suite end.
      // This API supports only one parameter: clear action function.
    })

    /**
     * @tc.number    : StartAbility_001
     * @tc.name      : StartAbility
     * @tc.desc      : start main Ability
     */
    it(BUNDLE + ' StartAbility_001', 0, async (done : Function) => {
      hilog.info(DOMAIN, TAG, `${BUNDLE} StartAbility_001 begin`);
      let want : Want= {
        bundleName: BUNDLE,
        abilityName: ABILITY_NAME
      };
      delegator.startAbility(want, (err) => {
        hilog.info(DOMAIN, TAG, `${BUNDLE} startAbility end err ${JSON.stringify(err)}`);
      })
      await driver.delayMs(1000);
      hilog.info(DOMAIN, TAG, `${BUNDLE} StartAbility_001 end`);
      done();
    })

    /**
     * @tc.number    : uart_initAbility_001
     * @tc.name      : uart_initAbility
     * @tc.desc      : Test uart_init_napi() interface
     */
    it(BUNDLE + 'uart_initAbility_001', 1, () => {
      // Defines a test case. This API supports three parameters: test case name, filter parameter, and test case function.
      hilog.info(DOMAIN, TAG, '%{public}s', 'uart_init test begin');
      let a = uart_napi.uart_init_napi()
      // Defines a variety of assertion methods, which are used to declare expected boolean conditions.
      expect(0).assertEqual(a)
      hilog.info(DOMAIN, TAG, '%{public}s', 'uart_init test end');
    })

    /**
     * @tc.number    : uart_closeAbility_001
     * @tc.name      : uart_closeAbility
     * @tc.desc      : Test uart_close_napi() interface
     */
    it(BUNDLE + 'uart_closeAbility_001', 3, () => {
      // Defines a test case. This API supports three parameters: test case name, filter parameter, and test case function.
      hilog.info(DOMAIN, TAG, '%{public}s', 'uart_close test begin');
      let a = uart_napi.uart_close_napi()
      // Defines a variety of assertion methods, which are used to declare expected boolean conditions.
      expect(0).assertEqual(a)
      hilog.info(DOMAIN, TAG, '%{public}s', 'uart_close test end');
    })

    /**
     * @tc.number    : Click_001
     * @tc.name      : Click
     * @tc.desc      : Check Text value
     */
    it(BUNDLE + ' Click_001', 4, async (done : Function) => {
      hilog.info(DOMAIN, TAG, '%{public}s', BUNDLE + ' Click_001 begin');

      try {
        await driver.delayMs(500);
        let button = await driver.findComponent(ON.id('button'));
        await button.click()
        await driver.delayMs(500);
        let num = 0;
        let text = await driver.findComponent(ON.id('text'))
        let getText = await text.getText();
        let TextStr = getText;
        hilog.info(DOMAIN, TAG, '%{public}s', TextStr);
        while(TextStr == "感应值: 尚未启动")
        {
          await driver.delayMs(500);
          hilog.info(DOMAIN, TAG, '%{public}s %{public}d', TextStr,num++);
          getText = await text.getText();
          TextStr = getText;
        }
        hilog.info(DOMAIN, TAG, '%{public}s', TextStr);
        expect("感应值: 尚未启动").not().assertEqual(TextStr)
        done();
      } catch (err) {
        hilog.error(DOMAIN, TAG, '%{public}s', `Click_001 err ${err.code}  msg ${err.message}`);
        expect().assertFail();
      }
      hilog.info(DOMAIN, TAG, '%{public}s', BUNDLE + ' Click_001 end');
    });

    /**
     * @tc.number    : Click_002
     * @tc.name      : Click
     * @tc.desc      : Check Text value and close button
     */
    it(BUNDLE + ' Click_002', 5, async (done : Function) => {
      hilog.info(DOMAIN, TAG, '%{public}s', BUNDLE + ' Click_002 begin');

      try {
        await driver.delayMs(1000);
        let button = await driver.findComponent(ON.id('button'));
        await button.click()
        await driver.delayMs(1000);
        let text = await driver.findComponent(ON.id('text'))
        let getText = await text.getText();
        let TextStr = getText
        hilog.info(DOMAIN, TAG, '%{public}s', TextStr);
        expect("感应值: 尚未启动").assertEqual(TextStr)
        done();
      } catch (err) {
        hilog.error(DOMAIN, TAG, '%{public}s', `Click_002 err ${err.code}  msg ${err.message}`);
        expect().assertFail();
      }
      hilog.info(DOMAIN, TAG, '%{public}s', BUNDLE + ' Click_002 end');
    });

    /**
     * @tc.number    : Click_003
     * @tc.name      : Click
     * @tc.desc      : Check Text value and restart button
     */
    it(BUNDLE + ' Click_003', 6, async (done : Function) => {
      hilog.info(DOMAIN, TAG, '%{public}s', BUNDLE + ' Click_003 begin');

      try {
        await driver.delayMs(1000);
        let button = await driver.findComponent(ON.id('button'));
        await button.click()
        await driver.delayMs(1000);
        let text = await driver.findComponent(ON.id('text'))
        let getTextMessage = await text.getText();
        let TextStr = getTextMessage
        hilog.info(DOMAIN, TAG, '%{public}s', TextStr);
        while(TextStr == "感应值: 尚未启动") {
          getTextMessage = await text.getText();
          TextStr = getTextMessage
        }
        expect("感应值: 尚未启动").not().assertEqual(TextStr)
        done();
      } catch (err) {
        hilog.error(DOMAIN, TAG, '%{public}s', `Click_003 err ${err.code}  msg ${err.message}`);
        expect().assertFail();
      }
      hilog.info(DOMAIN, TAG, '%{public}s', BUNDLE + ' Click_003 end');
    });

  })
}