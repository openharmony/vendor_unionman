/*
 * Copyright (c) 2024 Unionman Technology Co., Ltd.
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
declare namespace control_demo{

    /**
     * @description 获取串口数据(异步)
     * @param callback >0 -> 成功，-1 -> 失败
     */
    function uart_get(callback:(result:number)=>void):void;
    /**
     * @description 初始化串口
     * @returns 状态，0：成功；-1：失败；
     */
    function uart_init(): number;
    /**
     * @description 设置传感器休眠
     * @param value 1 -> 进入休眠 ， 0 -> 退出休眠
     * @returns 状态，0：成功；-1：失败；
     */
    function uart_sleep(value:number):number;
    /**
     * @description 设置舵机转动值
     * @param value 转动速度（-50 - +50）
     * @returns 状态，默认为0
     */
    function pwm_set(value:number):number;
    
}

export default control_demo;