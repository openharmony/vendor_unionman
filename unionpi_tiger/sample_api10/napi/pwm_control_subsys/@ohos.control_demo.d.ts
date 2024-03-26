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