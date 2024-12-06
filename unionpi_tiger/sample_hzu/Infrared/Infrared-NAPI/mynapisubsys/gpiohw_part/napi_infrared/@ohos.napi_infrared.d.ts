// @ohos.napi_infrared.d.ts

declare module '@ohos.napi_infrared' {
    /**
     * 发送红外信号。
     * @param signal 要发送的信号，0 或 1，代表低电平或高电平。
     * @returns Promise<void> 表示异步操作的完成。
     */
    export function sendInfraredSignalWithPromise(signal: number): Promise<void>;

    /**
     * 接收红外信号。
     * @returns Promise<number> 表示接收到的信号，0 或 1。
     */
    export function receiveInfraredSignalWithPromise(): Promise<number>;
}