declare namespace ipcnapi {
    function EexecuteCommandnapi(a:string): string;
    function EexecuteCommandNapiWithPromise(a:string): Promise<string>;
}

export default ipcnapi;