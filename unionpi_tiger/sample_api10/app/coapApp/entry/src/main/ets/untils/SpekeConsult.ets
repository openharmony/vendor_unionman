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
import { Bigint, hkdf, hmac, randomBytes, stringToBytes } from 'libspeke.so';
import SpekeRequest2 from './model/SpekeRequest2';
import SpekeResponse1 from './model/SpekeResponse1';

export default class SpekeConsult {
  static p: number = 87683;
  public sessionId: number
  public dataEncKey!: ArrayBuffer;
  private pin: string
  private salt!: ArrayBuffer
  private challenge1!: ArrayBuffer
  private challenge2!: ArrayBuffer
  private pk1!: Bigint;
  private sk2: Bigint;
  private sessionKey1!: DataView;
  private sessionKey2!: DataView;

  constructor(pin: string) {
    this.sessionId = Math.random() * 65536
    this.pin = pin;
    this.challenge2 = randomBytes(16)

    this.sk2 = new Bigint(randomBytes(32));
  }

  static hexStrToBytes(str: string): ArrayBuffer {
    let bytesLen = str.length / 2;
    let bytes = new Uint8Array(bytesLen);
    for (let i = 0;i < bytesLen; i++) {
      let byteStr = str.slice(i * 2, i * 2 + 2);
      bytes[i] = Number.parseInt(byteStr, 16)
    }
    return bytes.buffer
  }

  static bytesToHexStr(bytes: ArrayBuffer): string {
    let bytesLen = bytes.byteLength;
    let u8Bytes = new Uint8Array(bytes)
    let str = "";
    for (let i = 0;i < bytesLen; i++) {
      let byteStr = u8Bytes[i].toString(16);
      if (byteStr.length == 1) {
        byteStr = "0" + byteStr
      }
      str = str.concat(byteStr);
    }
    return str;
  }

  step1(resp: SpekeResponse1): SpekeRequest2 {
    let req = new SpekeRequest2();
    this.challenge1 = SpekeConsult.hexStrToBytes(resp.challenge);
    this.pk1 = new Bigint(SpekeConsult.hexStrToBytes(resp.pk));
    this.salt = SpekeConsult.hexStrToBytes(resp.salt);

    let secret = hkdf(stringToBytes(this.pin), this.salt, "hichain_speke_base_info", 32);
    let base = new Bigint(secret).powm(2, SpekeConsult.p);
    let pk2 = base.powm(this.sk2, SpekeConsult.p);
    let SharedSecret = this.pk1.powm(this.sk2, SpekeConsult.p);
    console.log(SharedSecret.toNumber().toString());
    let sessionKey = hkdf(SharedSecret.toBuffer(), this.salt, "hichain_speke_sessionkey_info", 32);
    this.sessionKey1 = new DataView(sessionKey, 0, 16)
    this.sessionKey2 = new DataView(sessionKey, 16, 16)

    let msg2 = new Uint8Array(32);
    msg2.set(new Uint8Array(this.challenge2));
    msg2.set(new Uint8Array(this.challenge1), 16);
    let kcfData2 = hmac(this.sessionKey2, msg2.buffer);

    req.challenge = SpekeConsult.bytesToHexStr(this.challenge2);
    console.log("challenge2: " + req.challenge)
    req.pk = SpekeConsult.bytesToHexStr(pk2.toBuffer());
    req.kcfData = SpekeConsult.bytesToHexStr(kcfData2);

    return req;
  }

  step2(kcfData1: string): ArrayBuffer | undefined {
    let msg1 = new Uint8Array(32);
    msg1.set(new Uint8Array(this.challenge1));
    msg1.set(new Uint8Array(this.challenge2), 16);
    let my_kcfData1 = hmac(this.sessionKey2, msg1.buffer);
    if (SpekeConsult.bytesToHexStr(my_kcfData1) == kcfData1) {
      this.dataEncKey = hkdf(this.sessionKey1, this.salt, "hichain_return_key", 16);
      return this.dataEncKey;
    }
    else {
      return undefined
    }
  }
}