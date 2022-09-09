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

import prompt from '@ohos.prompt';
import gpioled from '@ohos.gpioled';

export default {
    data: {
        pin: 384,
        status: Boolean
    },
    onInit() {
        this.syncButtonStatus()
    },
    changeGpio(msg) {
        this.pin = Number(msg.newValue)
        this.syncButtonStatus()
        console.info(`Select:${msg.newValue}`);
        console.info(`Select:${this.status}`);
    },
    switchChange(e) {
        if (e.checked) {
            this.addLedEffect()
        } else {
            this.removeLedEffect()
        }
    },

    //开灯
    addLedEffect() {
        gpioled.setLedStatus(this.pin, gpioled.LED_ON).then((result) => {
            if (result === 0) {
                console.info("Turned on LED." + result)
                prompt.showToast({
                    message: "开灯"
                })
            } else {
                console.info("Turned on LED failed." + result)
                prompt.showToast({
                    message: "开灯失败"
                })
            }
        })
    },

    //关灯
    removeLedEffect() {
        gpioled.setLedStatus(this.pin, gpioled.LED_OFF).then((result) => {
            if (result === 0) {
                console.info("Turned off LED." + result)
                prompt.showToast({
                    message: "关灯"
                })
            } else {
                console.info("Turned off LED failed." + result)
                prompt.showToast({
                    message: "关灯失败"
                })
            }
        })
    },

    //同步按钮开关状态
    syncButtonStatus() {
        gpioled.getLedStatus(this.pin).then((result) => {
            this.status = Boolean(result)
        })
    }
}



