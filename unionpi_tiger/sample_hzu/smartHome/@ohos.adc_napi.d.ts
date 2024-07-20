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

declare namespace adc_napi {
    /**
     * 获取光敏电阻的ADC值
     * @returns 光敏电阻的ADC值
     */
    function sync_get_photoresistor_value(): number;

    /**
     * 获取温度传感器的ADC值
     * @returns 温度传感器的ADC值
     */
    function sync_get_temp_sensor_value(): number;

    /**
     * 获取指定ADC通道的权限
     * @param channel ADC通道号
     * @returns 权限获取结果
     */
    function sync_get_permission(channel: number): number;
}

export default adc_napi;
