/*
 * Copyright 2023 Unionman Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

declare namespace uarttest {
    /**
     * @param device_num 
     * device_num为0 ：ttl设备
     * device_num为1 ：40pin设备
     * device_num为2 ：485设备
     */
    function sync_UM_uart_test(device_num: number): number;
    function UM_uart_test(device_num: number): Promise<number>;
}

export default uarttest;