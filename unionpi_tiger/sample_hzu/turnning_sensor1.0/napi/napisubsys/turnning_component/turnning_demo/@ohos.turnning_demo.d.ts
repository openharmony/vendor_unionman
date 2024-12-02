/**
 * unionman a311d
 * @since 8
 * @devices phone, tablet
 * @import import gpioLed from '@ohos.gpioLed';
 * @permission N/A
 */

declare namespace turnning {
   
    /**
     * 直接返回转动格数turn_click的值，这个函数包含一个死循环，目前不知道应该如何
     */
    function getStatusWithPromise(callback: (result: number) => void): void;
}

export default turnning;
