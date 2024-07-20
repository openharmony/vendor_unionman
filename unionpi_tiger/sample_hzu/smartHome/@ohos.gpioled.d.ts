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

/**
 * unionman a311d
 * @since 8
 * @devices phone, tablet
 * @import import gpioLed from '@ohos.gpioLed';
 * @permission N/A
 */

declare namespace gpioLed {
    const ON = 1;
    const OFF = 0;

    /**
     * 设置LED灯的开关状态
     * @param pin pin口号
     * @param status 状态，1：开启；0：关闭；
     * @param callback 回调函数
     */
    function setLedStatusWithCallback(pin: number, status: number, callback: (result: number) => void): void;

    /**
     * 设置LED灯的开关状态
     * @param pin pin口号
     * @param status 状态，1：开启；0：关闭；
     */
    function setLedStatusWithPromise(pin: number, status: number): Promise<number>;

    /**
     * 获取LED灯的开关状态
     * @param pin pin口号
     * @param callback 回调函数
     */
    function getLedStatusWithCallback(pin: number, callback: (result: number) => void): void;

    /**
     * 获取LED灯的开关状态
     * @param pin pin口号
     */
    function getLedStatusWithPromise(pin: number): Promise<number>;

    /**
     * 设置风扇的开关状态
     * @param pin pin口号
     * @param status 状态，1：开启；0：关闭；
     * @param callback 回调函数
     */
    function setFanStatusWithCallback(pin: number, status: number, callback: (result: number) => void): void;

    /**
     * 设置风扇的开关状态
     * @param pin pin口号
     * @param status 状态，1：开启；0：关闭；
     */
    function setFanStatusWithPromise(pin: number, status: number): Promise<number>;

    /**
     * 获取风扇的开关状态
     * @param pin pin口号
     * @param callback 回调函数
     */
    function getFanStatusWithCallback(pin: number, callback: (result: number) => void): void;

    /**
     * 获取风扇的开关状态
     * @param pin pin口号
     */
    function getFanStatusWithPromise(pin: number): Promise<number>;

    /**
     * 设置闹钟的开关状态
     * @param pin pin口号
     * @param status 状态，1：开启；0：关闭；
     * @param callback 回调函数
     */
    function setAlarmStatusWithCallback(pin: number, status: number, callback: (result: number) => void): void;

    /**
     * 设置闹钟的开关状态
     * @param pin pin口号
     * @param status 状态，1：开启；0：关闭；
     */
    function setAlarmStatusWithPromise(pin: number, status: number): Promise<number>;

    /**
     * 获取闹钟的开关状态
     * @param pin pin口号
     * @param callback 回调函数
     */
    function getAlarmStatusWithCallback(pin: number, callback: (result: number) => void): void;

    /**
     * 获取闹钟的开关状态
     * @param pin pin口号
     */
    function getAlarmStatusWithPromise(pin: number): Promise<number>;

    /**
     * 设置设备的开关状态
     * @param pin pin口号
     * @param status 状态，1：开启；0：关闭；
     * @param callback 回调函数
     */
    function setStatusWithCallback(pin: number, status: number, callback: (result: number) => void): void;

    /**
     * 设置设备的开关状态
     * @param pin pin口号
     * @param status 状态，1：开启；0：关闭；
     */
    function setStatusWithPromise(pin: number, status: number): Promise<number>;

    /**
     * 获取设备的开关状态
     * @param pin pin口号
     * @param callback 回调函数
     */
    function getStatusWithCallback(pin: number, callback: (result: number) => void): void;

    /**
     * 获取设备的开关状态
     * @param pin pin口号
     */
    function getStatusWithPromise(pin: number): Promise<number>;
}

export default gpioLed;

