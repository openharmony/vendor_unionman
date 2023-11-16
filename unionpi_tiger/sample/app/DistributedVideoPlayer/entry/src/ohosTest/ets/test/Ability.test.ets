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
import hilog from '@ohos.hilog';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from '@ohos/hypium'
import abilityDelegatorRegistry from '@ohos.app.ability.abilityDelegatorRegistry';
import { Driver, ON } from '@ohos.UiTest';

const BUNDLE = "com.unionman.distributedvideoplayer";
const ABILITY_NAME = "EntryAbility";
const TAG = "DistributedvideoplayerTest";
const DOMAIN = 0xF811;

export default function abilityTest() {
  let driver = Driver.create();
  let delegator = abilityDelegatorRegistry.getAbilityDelegator();
  describe('ActsAbilityTest', function () {
    // Defines a test suite. Two parameters are supported: test suite name and test suite function.
    beforeAll(function () {
      // Presets an action, which is performed only once before all test cases of the test suite start.
      // This API supports only one parameter: preset action function.
      delegator.startAbility({
        bundleName: BUNDLE,
        abilityName: ABILITY_NAME
      }, (err) => {
        hilog.info(DOMAIN, TAG, `${BUNDLE} startAbility end err ${JSON.stringify(err)}`);
      })
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
    //测试进入设置页面并设置分布式流转模式
    it("setDistributedMode", 0, async (done) => {
      hilog.info(DOMAIN, TAG, "setDistributedMode_0 start");
      try {
        //进入时等待3s,方便点击页面上的授权按钮
        await driver.delayMs(3000);
        //找到并点击设置页面按钮
        await driver.assertComponentExist(ON.id("setting"));
        let settingBtn = await driver.findComponent(ON.id("setting"));
        await settingBtn.click();
        //等待1s来打开设置页面
        await driver.delayMs(1000);

        //找到并点击多端协同按钮
        await driver.assertComponentExist(ON.id("synchronization"));
        let synchronizationRdo = await driver.findComponent(ON.id("synchronization"));
        await synchronizationRdo.click();
        //等待1s反应
        await driver.delayMs(1000);

        //找到并点击左上角的返回按钮
        await driver.assertComponentExist(ON.id("back"));
        let backBtn = await driver.findComponent(ON.id("back"));
        await backBtn.click();
        //等待1s以返回主页面
        await driver.delayMs(1000);
      } catch (err) {
        hilog.error(DOMAIN, TAG, "throw error! error code:%{public}d error info:%{public}s", err.code, err.message);
        expect().assertFail();
      }
      hilog.info(DOMAIN, TAG, "setDistributedMode_0 end");
      done();
    })
    //测试选择分布式流转设备
    it("selectDistributedDevice", 1, async (done) => {
      hilog.info(DOMAIN, TAG, "setDistributedMode_1 start");
      try {
        //找到并点击选择设备按钮
        await driver.assertComponentExist(ON.id("selDevBtn"));
        let selDevBtn = await driver.findComponent(ON.id("selDevBtn"));
        await selDevBtn.click();
        //等待1s以打开对话框
        await driver.delayMs(1000);

        /*
        * 点击第二个设备，由于对话框的子控件暂时没找到办法获取
        * 所以用了绝对坐标进行模拟点击
        */
        await driver.click(608, 266);
        //等待1s给他时间反应
        await driver.delayMs(1000);
        done();
      } catch (err) {
        hilog.error(DOMAIN, TAG, "throw error! error code:%{public}d error info:%{public}s", err.code, err.message);
        expect().assertFail();
      }
      hilog.info(DOMAIN, TAG, "setDistributedMode_1 end");
    })
    //测试分布式视频流转播放
    it("startDistributedPlay", 2, async (done) => {
      hilog.info(DOMAIN, TAG, "startDistributedPlay_2 start");
      try {
        //找到并点击选择播放键
        await driver.assertComponentExist(ON.id("play"));
        let playBtn = await driver.findComponent(ON.id("play"));
        await playBtn.click();
        //等待1s反应
        await driver.delayMs(1000);
        done();
      } catch (err) {
        hilog.error(DOMAIN, TAG, "throw error! error code:%{public}d error info:%{public}s", err.code, err.message);
        expect().assertFail();
      }
      hilog.info(DOMAIN, TAG, "startDistributedPlay_2 end");
    })
    //测试分布式视频速度设置
    it("setSpeed", 3, async (done) => {
      hilog.info(DOMAIN, TAG, "setSpeed_3 start");
      try {
        //找到并点击速度按钮
        await driver.assertComponentExist(ON.id("speed"));
        let speedSel=await driver.findComponent(ON.id("speed"));
        await speedSel.click();
        //等待1s反应
        await driver.delayMs(1000);

        /*
        * 点击倍速2。0，由于选择框的子控件暂时没找到办法获取
        * 所以用了绝对坐标进行模拟点击
        */
        await driver.click(649,409);
        //等待400ms反应
        await driver.delayMs(400);
        done();
      } catch (err) {
        hilog.error(DOMAIN, TAG, "throw error! error code:%{public}d error info:%{public}s", err.code, err.message);
        expect().assertFail();
      }
      hilog.info(DOMAIN, TAG, "setSpeed_3 end");
    })
    //测试分布式视频截图功能
    it("startScreenshot", 4, async (done) => {
      hilog.info(DOMAIN, TAG, "startScreenshot_4 start");
      try {
        //找到并点击截屏按钮
        await driver.assertComponentExist(ON.id("screenshot"));
        let scBtn = await driver.findComponent(ON.id("screenshot"));
        await scBtn.click();
        //等待1s反应
        await driver.delayMs(1000);
        done();
      } catch (err) {
        hilog.error(DOMAIN, TAG, "throw error! error code:%{public}d error info:%{public}s", err.code, err.message);
        expect().assertFail();
      }
      hilog.info(DOMAIN, TAG, "startScreenshot_4 end");
    })
  })
}