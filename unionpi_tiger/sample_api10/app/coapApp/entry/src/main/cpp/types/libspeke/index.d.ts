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
type Binary = ArrayBuffer|DataView

export class Bigint {
    constructor(source:number);
    constructor(source:string);
    constructor(source:ArrayBuffer);

    powm(exp:Bigint , p:number):Bigint;
    powm(exp:number , p:number):Bigint;

    toBuffer():ArrayBuffer;
    toNumber():number;
    toString():string;
}


export function randomBytes(byteLen:number):ArrayBuffer;
export function stringToBytes(str:string):ArrayBuffer
export function hkdf(ikm:Binary,salt:Binary,info:string,keylen:number):ArrayBuffer
export function hmac(key:Binary,msg:Binary):ArrayBuffer;
export function encrypt( key:ArrayBuffer, plaintext:ArrayBuffer ):ArrayBuffer
export function decrypt( key:ArrayBuffer, ciphertext:ArrayBuffer ):ArrayBuffer