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

declare namespace adc_gpio_napi {
    /**
     * 同步获取摇杆的 X, Y, Z 值。
     * @returns {object} 包含 X, Y, Z 值的对象：
     *                   - `x` {number}: 摇杆的 X 轴值。
     *                   - `y` {number}: 摇杆的 Y 轴值。
     *                   - `z` {number}: Z 按键的状态，1 表示按下，0 表示未按下。
     *                   如果读取失败，返回 -1。
     */
    function sync_get_joystick_values(): {
        x: number;
        y: number;
        z: number;
    };

    /**
     * 同步检查是否具有访问 ADC 设备的权限。
     * @returns {number} 权限检查的结果。
     *                   返回 0 表示权限通过，非 0 值表示失败。
     */
    function sync_get_permission(): number;
}

export default adc_gpio_napi;
