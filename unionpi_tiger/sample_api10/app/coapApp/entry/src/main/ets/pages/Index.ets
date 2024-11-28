/*
 * Copyright (c) 2024 Unionman Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import { CoapClient, CoapRequestMethod, CoapRequestType, CoapResponseCode } from '@ohos/coap';
import { encrypt, stringToBytes } from 'libspeke.so';
import SpekeResponse1 from '../untils/model/SpekeResponse1';
import SpekeConsult from '../untils/SpekeConsult';
import Prompt from '@system.prompt';

@Entry
@Component
struct Index {
  private speke: SpekeConsult = new SpekeConsult("1234")
  private ip:string = "192.168.10.3";
  build() {
    Row() {
      Column() {
        Text("speke协商认证测试端")
          .fontSize(40)
          .fontWeight(FontWeight.Bold)
          .width('90%')
        Row(){
          Text("ip: ")
          TextInput()
            .onChange((value)=>{
              this.ip = value;
            })
            .width("50%")
        }

        Button("设备连接")
          .onClick(() => {
            let coapClient1 = new CoapClient();
            coapClient1.request("coap://"+this.ip+":5683/ohos/v1/device/spekeStart", CoapRequestMethod.POST, CoapRequestType.COAP_MESSAGE_CON)
              .then((data) => {
                if (data.code == CoapResponseCode.SUCCESS) {
                  let coapClient2 = new CoapClient();
                  let resp = JSON.parse(data.message[0]) as SpekeResponse1;
                  let spekeReq = this.speke.step1(resp);
                  coapClient2.setPayload(JSON.stringify(spekeReq))
                  coapClient2.request("coap://"+this.ip+":5683/ohos/v1/device/spekeFinish", CoapRequestMethod.POST, CoapRequestType.COAP_MESSAGE_CON)
                    .then((data) => {
                      if (data.code == CoapResponseCode.SUCCESS) {
                        class Resp2 {
                          kcfData: string

                          constructor() {
                            this.kcfData = ""
                          }
                        }
                        let resp2 = JSON.parse(data.message[0]) as Resp2
                        let kcfData1 = resp2.kcfData;
                        let key = this.speke.step2(kcfData1)
                        console.log(SpekeConsult.bytesToHexStr(key));
                        Prompt.showToast({message:"设备连接成功！"})
                      }
                    })
                }
              })
          })
        Button("启动计算器")
          .onClick(() => {
            class Resp3 {
              operateCode: number
              parms: Object[]
              constructor() {
                this.operateCode = 0
                this.parms = []
              }
            }

            let coapClient3 = new CoapClient();
            let test: Resp3 = { operateCode: 1, parms: [] };
            let plaintext = JSON.stringify(test);
            let ciphertext = encrypt(this.speke.dataEncKey, stringToBytes(plaintext));
            coapClient3.setPayload(SpekeConsult.bytesToHexStr(ciphertext));
            coapClient3.request("coap://"+this.ip+":5683/ohos/v1/device/operate", CoapRequestMethod.POST, CoapRequestType.COAP_MESSAGE_CON)
              .then((data) => {
                if (data.code == CoapResponseCode.SUCCESS) {
                  console.log("msg:" + data.message[0])
                  Prompt.showToast({message:"计算器启动成功！"})
                }
              })
          })
      }
      .height('100%')
    }
  }
}