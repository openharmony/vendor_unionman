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
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from '@ohos/hypium'

import hilog from '@ohos.hilog';

import { Driver, ON, MatchPattern, Component } from '@ohos.UiTest'
import AbilityDelegatorRegistry from '@ohos.app.ability.abilityDelegatorRegistry'
import mediaLibrary from '@ohos.multimedia.mediaLibrary'


const BUNDLE = 'ComponentCollection_';

const DELAY_TIME = 1000;
let driver: Driver = Driver.create();

// 获取AbilityDelegator
let abilityDelegatorRegistry = AbilityDelegatorRegistry.getAbilityDelegator();
// 启动后获取app上下文
let context;
// 启动后获取resourceManager
let manager;

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
        it('assertContain',0, function () {
            /**
             * 打开应用
             */
            it(BUNDLE + 'StartAbility_001', 0, async function (done) {
                let abilityDelegator = AbilityDelegatorRegistry.getAbilityDelegator()
                try {
                    await abilityDelegator.startAbility({
                        bundleName: 'ohos.samples.two_videoPlayer',
                        abilityName: '.MainAbility'
                    })

                    context = await abilityDelegatorRegistry.getAppContext();
                    manager = context.resourceManager;

                    done();
                } catch (exception) {
                    expect(0).assertEqual(exception.code);
                    done();
                }
            })
            // 通过 ID 滑动 slider
            it('Slider_001', 0, async function () {
                let driver: Driver = Driver.create();
                let atom = "Slider";
                await driver.assertComponentExist(ON.id(atom));
                let slider_01 = await driver.findComponent(ON.id(atom));
                let rect = await slider_01.getBoundsCenter();
                await driver.drag(rect.x - 100, rect.y, rect.x + 100, rect.y, 800)

            })
            // 通过 ID 点击 暂停 播放
            it('Btn_001', 0, async function () {
                let driver: Driver = Driver.create();
                await driver.delayMs(DELAY_TIME);
                await driver.assertComponentExist(ON.id("Play"));
                let button = await driver.findComponent(ON.id("Play"));
                await button.click();
                await driver.delayMs(DELAY_TIME);

            })

        })
    })
}
