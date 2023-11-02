/*
 * Copyright 2023 Unionman Technology Co., Ltd.
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
import web_webview from '@ohos.web.webview';

@Component
export struct  JavaScriptWebComponent {
  webController: WebController = new WebController();
  @State webResult: string = ''
  testObj = {
    test: (data) => {
      return "ArkUI Web Component";
    },
    toString: () => {
      console.log('Web Component toString');
    }
  }

  build() {
    Column() {
      Text(this.webResult)
      .fontSize(20)
      Button('refresh')
        .onClick(() => {
          try {
            this.webController.refresh();
            this.webResult = "refresh success";
          } catch (error) {
            console.error(`Errorcode: ${error.code}, Message: ${error.message}`);
            this.webResult = "refresh Errorcode:" + error.code + ", Message:"+error.message;
          }
        })
      Button('Register JavaScript To Window')
        .onClick(() => {
          try {
            this.webController.registerJavaScriptProxy({object:this.testObj, name:"objName", methodList:["test", "toString"]});
            this.webResult = "Register JavaScript To Window success";
          } catch (error) {
            console.error(`Errorcode: ${error.code}, Message: ${error.message}`);
            this.webResult = "Register Errorcode:" + error.code + ", Message:"+error.message;
          }
        })
      Web({ src: $rawfile('1.html'), controller: this.webController })
        .javaScriptAccess(true)
        .onPageEnd(e => {
          try {
            this.webController.runJavaScript({script:'test()'})
              .then(function (result) {
                console.log('result: ' + result);
                this.webResult = 'runJavaScript result: ' + result;
              })
              .catch(function (error) {
                console.error("error: " + error);
                this.webResult = "runJavaScript error: " + error;
              })
            console.info('url: ', e.url);
          } catch (error) {
            console.error(`Errorcode: ${error.code}, Message: ${error.message}`);
            this.webResult = "runJavaScript Errorcode:" + error.code + ", Message:"+error.message
          }
        })
    }
    .width('100%')
    .height("100%")
    .padding(10)

  }

}