import web_webview from '@ohos.web.webview';
import { BusinessError } from '@ohos.base';
import { Event } from '@ohos.worker';
class WebObj {
  constructor() {
  }
  webTest(): string {
    console.log('Web test');
    return "Web test";
  }
  webString(): void {
    console.log('Web test toString');
  }
}
@Component
export struct JavaScriptWebComponent {
  @State webResult: string = ''
  @State webTestObj: WebObj = new WebObj()
  controller: web_webview.WebviewController = new web_webview.WebviewController()
  build() {
    Column() {
      Text(this.webResult)
        .fontSize(20)
      Button('refresh')
        .onClick(() => {
          try {
            this.controller.refresh();
            this.webResult = "refresh success";
          } catch (error) {
            console.error(`Errorcode: ${error.code}, Message: ${error.message}`);
            this.webResult = "refresh Errorcode:" + error.code + ", Message:" + error.message;
          }
        })
      Button('Register JavaScript To Window')
        .onClick(() => {
          try {
            this.controller.registerJavaScriptProxy(this.webTestObj, "objTestName", ["webTest", "webString"]);
            this.webResult = "Register JavaScript To Window success";
          } catch (error) {
            console.error(`Errorcode: ${error.code}, Message: ${error.message}`);
            this.webResult = "Register Errorcode:" + error.code + ", Message:" + error.message;
          }
        })
      Web({ src: $rawfile('1.html'), controller: this.controller })
        .javaScriptAccess(true)
        .onPageEnd((e) => {
          try {
            this.controller.runJavaScript('test()')
              .then((result) => {
                console.log('result: ' + result);
                this.webResult = 'runJavaScript result: ' + result;
              })
              .catch((error: BusinessError) => {
                console.error("error: " + error);
                this.webResult = "runJavaScript error: " + error;
              })
            console.info('url: ', JSON.stringify(e?.url));
          } catch (error) {
            console.error(`Errorcode: ${error.code}, Message: ${error.message}`);
            this.webResult = "runJavaScript Errorcode:" + error.code + ", Message:" + error.message
          }
        })
    }
    .width('100%')
    .height("100%")
    .padding(10)
  }
}