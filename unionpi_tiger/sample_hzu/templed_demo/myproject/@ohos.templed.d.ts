declare namespace templed {
    const LED_ON = 1;
    const LED_OFF = 0;

    /**
     * 设置LED灯的开关状态
     * @param pin pin口号
     * @param status 状态，1：开启；0：关闭；
     * @param callback 	回调函数
     */
    function setLedStatusWithCallback(pin: number, status: number, callback: (result: number) => void): void;

    /**
     * 设置LED灯的开关状态
     * @param pin pin口号
     * @param status 状态，1：开启；0：关闭；
     */
    function setLedStatusWithPromise(pin: number, status: number): Promise<number>;

    /**
     * 设置LED灯的开关状态
     * @param pin pin口号
     * @param status 状态，1：开启；0：关闭；
     * @param callback 	回调函数
     */
    function setLedStatus(pin: number, status: number, callback: (result: number) => void): void;

    /**
     * 设置LED灯的开关状态
     * @param pin pin口号
     * @param status 状态，1：开启；0：关闭；
     */
    function setLedStatus(pin: number, status: number): Promise<number>;

    /**
     * 获取LED灯的开关状态
     * @param pin pin口号
     * @param callback 	回调函数
     */
    function getLedStatusWithCallback(pin: number, callback: (result: number) => void): void;

    /**
     * 获取LED灯的开关状态
     * @param pin pin口号
     */
    function getLedStatusWithPromise(pin: number): Promise<number>;

    /**
     * 获取LED灯的开关状态
     * @param pin pin口号
     * @param callback 	回调函数
     */
    function getLedStatus(pin: number, callback: (result: number) => void): void;

    /**
     * 获取LED灯的开关状态
     * @param pin pin口号
     */
    function getLedStatus(pin: number): Promise<number>;

    function sync_get_adc_value(): number;

    function sync_get_temperature_value(): number;

}

export default templed;