declare namespace fish_tank
{
    /**
     * 在指定秒数之后开启水泵
     * @param time 秒；
     * @returns Promise<number>，number始终为0
     */
    function setPumpWithPromise(time: number): Promise<number>;

    /**
      * 在指定秒数之后开启舵机
      * @param time 秒；
      * @returns Promise<number>，number始终为0
      */
    function setSteeringWithPromise(time: number): Promise<number>;

    /**
     * 获取水位
     * @returns 水位
     */
    function getWaterLevelWithPromise(): Promise<number>;
}

export default fish_tank;