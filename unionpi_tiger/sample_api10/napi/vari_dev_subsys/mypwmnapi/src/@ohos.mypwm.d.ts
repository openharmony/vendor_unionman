/* Copyright 2024 Unionman Technology Co., Ltd.
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
 * MyPWM 命名空间提供了与 PWM 控制相关的 NAPI 接口。
 */
declare namespace mypwm {
    /**
     * 设置 PWM 的状态，并返回设置结果。
     * @param {number} pwmPex PWM 端口号。
     * @param {number} pwmAngle PWM 角度值。
     * @returns {Promise<number>} 设置结果的 Promise回调，成功时返回舵机当前的角度值，失败时返回错误码。
     */
    function setPwmStatus(pwmPex: number, pwmAngle: number): Promise<number>;
}

export default mypwm;