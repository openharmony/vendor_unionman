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
// @ts-nocheck
import web_webview from '@ohos.web.webview'
import prompt from '@system.prompt';

@Component
export struct WebComponent {
  webController: WebController = new WebController();
  @State webTitle: string = "https://www.arkui.club";
  @State hitTest: string = "";
  @State message: string = "";
  @State inputValue: string = "";
  isShowHtml: boolean = true;
  controller: web_webview.WebviewController = new web_webview.WebviewController();
  ports: web_webview.WebMessagePort[];
  @State sendFromEts: string = 'Send this message from ets to HTML';
  @State receivedFromHtml: string = 'Display received message send from HTML';

  build() {
    Column() {
      if (this.isShowHtml === true) {
        Text(this.message)
          .fontSize(20)
        Row({ space: 10 }) {
          Button("刷新")
            .id("update")
            .onClick(() => {
              this.webController.refresh();
            })
          Button("前进")
            .id("go")
            .onClick(() => {
              this.webController.forward();
            })

          Button("后退")
            .id("back")
            .onClick(() => {
              this.webController.backward();
            })

          Button("清除记录")
            .id("clean")
            .onClick(() => {
              this.webController.clearHistory();
            })
        }

        Row({ space: 10 }) {
          Button('getCookie')
            .id("getCookie")
            .onClick(() => {
              try {
                let value = web_webview.WebCookieManager.getCookie(this.webTitle);
                console.log("value: " + value);
                this.message = "getCookie success value: " + value;
              } catch (error) {
                console.error(`ErrorCode: ${error.code},  Message: ${error.message}`);
                this.message = "getCookie: ErrorCode:" + error.code + ",  Message: " + error.message;
              }
            })

          Button('清除会话cookie')
            .id("cleanCookie")
            .onClick(() => {
              web_webview.WebCookieManager.deleteSessionCookie();
            })
          TextInput({ placeholder: "输入发送Cookie内容" })
            .layoutWeight(1)
            .height(35)
            .type(InputType.Normal)
            .fontColor(Color.Brown)
            .enterKeyType(EnterKeyType.Next)
            .caretColor(Color.Red)
            .placeholderColor(Color.Green)
            .placeholderFont({
              size: 20,
              style: FontStyle.Italic,
              weight: FontWeight.Bold
            })
            .onChange((value) => {
              this.inputValue = value;
            })
          Button('发送')
            .id("send")
            .onClick(() => {
              try {
                web_webview.WebCookieManager.setCookie(this.webTitle, this.inputValue);
                this.message = "发送成功"
              } catch (error) {
                console.error(`ErrorCode: ${error.code},  Message: ${error.message}`);
                this.message = "发送失败: ErrorCode:" + error.code + ",  Message: " + error.message;
              }
            })

        }

        Button('保存当前页面')
          .id("save")
          .onClick(() => {
            let webAsyncController = new web_webview.WebAsyncController(this.webController);
            webAsyncController.storeWebArchive("/data/storage/el2/base/", true)
              .then(filename => {
                if (filename != null) {
                  console.info(`save web archive success: ${filename}`)
                  this.message = "页面保存在: data/app/el2/100/base/com.ohos.myapplication" + filename;
                  //实际存储路径：data/app/el2/100/base/com.ohos.myapplication/index.mht
                }
              })
          })
        Web({
          src: this.webTitle, // 默认加载 www.arkui.club 网址
          controller: this.webController
        })
          .onPageEnd((event) => {
            this.webTitle = event.url;
            this.message = "当前网站：" + this.webTitle;
          })
      }
      else {
        // 展示接收到的来自HTML的内容
        Text(this.receivedFromHtml)
        // 输入框的内容发送到html
        TextInput({ placeholder: 'Send this message from ets to HTML' })
          .onChange((value: string) => {
            this.sendFromEts = value;
          })

        Button('postMessage')
          .id("postMessage")
          .onClick(() => {
            try {
              // 1、创建两个消息端口。
              this.ports = this.controller.createWebMessagePorts();
              // 2、在应用侧的消息端口(如端口1)上注册回调事件。
              this.ports[1].onMessageEvent((result: web_webview.WebMessage) => {
                var msg = 'Got msg from HTML:';
                if (typeof (result) == "string") {
                  console.log("received string message from html5, string is:" + result);
                  msg = msg + result;
                } else if (typeof (result) == "object") {
                  if (result instanceof ArrayBuffer) {
                    console.log("received arraybuffer from html5, length is:" + result.byteLength);
                    msg = msg + "lenght is " + result.byteLength;
                  } else {
                    console.log("not support");
                  }
                } else {
                  console.log("not support");
                }
                this.receivedFromHtml = msg;
              })
              // 3、将另一个消息端口(如端口0)发送到HTML侧，由HTML侧保存并使用。
              this.controller.postMessage('__init_port__', [this.ports[0]], '*');
            } catch (error) {
              console.error(`ErrorCode: ${error.code},  Message: ${error.message}`);
            }
          })

        // 4、使用应用侧的端口给另一个已经发送到html的端口发送消息。
        Button('SendDataToHTML')
          .id("SendDataToHTML")
          .onClick(() => {
            try {
              if (this.ports && this.ports[1]) {
                this.ports[1].postMessageEvent(this.sendFromEts);
              } else {
                console.error(`ports is null, Please initialize first`);
              }
            } catch (error) {
              console.error(`ErrorCode: ${error.code}, Message: ${error.message}`);
            }
          })
        Web({ src: $rawfile('1.html'), controller: this.controller })
      }
    }
    .width('100%')
    .height("100%")
    .padding(10)
  }
}