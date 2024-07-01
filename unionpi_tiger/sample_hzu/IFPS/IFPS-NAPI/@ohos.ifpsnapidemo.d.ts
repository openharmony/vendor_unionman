/* Copyright 2023 Unionman Technology Co., Ltd.
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
declare namespace ifpsnapidemo {

    /**
    * 温湿度传感器NAPI接口
    * 型号 DIS-SHT-3X
    */

    // 传感器软重启
    function Sht3xSoftReset(): number;

    /**
     * 设置传感器测量模式
     * @param mps 取值为[0,3] 对应mps=0.5、1、2、4、10Hz
     * @param repeatability 取值为[0,2] 对应低刷新率、中刷新率、高刷新率
     */
    function Sht3xModeSet(mps: number, repeatability: number): number;

    // 读取温湿度数据
    function Sht3xReadData(): number;
    // 使用下列三个接口时必须先调用ReadData
    function Sht3xReadTemperatureC(): number;
    function Sht3xReadHumidity(): number;
    function Sht3xReadTemperatureF(): number;

    /**
    * 烟雾传感器NAPI接口
    * 型号 MQ-2 
    */
    function Mq2GetAdcValue(): number;
    function Mq2GetPermission(): number;

    /**
     * 火焰传感器NAPI接口
     * 型号 FC-01
     */
    function FlameSensorGetAdcValue(): number;
    function FlameSensorGetPermission(): number;


    /**
     *  舵机NAPI接口
     *  型号 MG996R
     */

    /**
    * 设置pwm的开关状态
    * @param pwmPex pwm引脚
    * @param pwmAngle pwm角度
    */
    function SetPwmStatus(pwmPex: number, pwmAngle: number): Promise<number>;

}

export default ifpsnapidemo;