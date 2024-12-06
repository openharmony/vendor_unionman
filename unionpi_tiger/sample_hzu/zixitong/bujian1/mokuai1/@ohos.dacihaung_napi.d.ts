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

declare namespace dacihuang {
    /**
     * 设置大磁环的工作gpio口并读取数字数据
     * @param pin pin口号，为编号，如gpio1为380
     */
    function dacihuangshuziWithPromise(pin: number): Promise<number>;


    /**
     * 设置大磁环的工作adc口并读取模拟数据
     * @param adc adc通道号，如adc1为1
     */
    function dacihuangmoniWithPromise(adc: number): Promise<number>;
    
}

export default dacihuang;