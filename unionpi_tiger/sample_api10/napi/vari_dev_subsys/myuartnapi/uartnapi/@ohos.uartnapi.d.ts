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
 * UARTNAPI 命名空间提供了与 UART 相关的 NAPI 接口。
 */
declare namespace uartnapi {
    /**
     * 初始化 UART，返回初始化状态。
     * @returns 初始化状态，0 表示成功，其他值表示失败。
     */
    function uart_init_napi(): number;

    /**
     * 关闭 UART，返回关闭状态。
     * @returns 关闭状态，成功返回字符串 "success"，失败返回具体错误信息。
     */
    function uart_close_napi(): string;

    /**
     * 设置 UART 回调函数，用于接收 UART 数据。
     * @param callback 接收数据回调函数，参数为接收到的数据字符串。
     * @returns 设置状态，0 表示成功，其他值表示失败。
     */
    function uartcallback(callback: (result: string) => void): number;
}

export default uartnapi;
