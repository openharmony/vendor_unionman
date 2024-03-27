declare namespace uartnapi {
    function uart_init_napi(): number;
    function uart_close_napi(): string;
    function uartcallback(callback: (result: string) => void): number;
}

export default uartnapi;
