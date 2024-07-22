/* Copyright 2023 Unionman Technology Co., Ltd.
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

import http from '@ohos.net.http'
import hilog from '@ohos.hilog'

const request = http.createHttp()

const baseUrl = 'http://192.168.150.192:8080'

export interface RequestTask {
  init: (url: string,
         options: http.HttpRequestOptions,
         callback: (err: Error, res: http.HttpResponse) => void) => void
  request: () => void
}

export function sendRequest(url: string,
                            options: http.HttpRequestOptions) {
  hilog.debug(0x0000, 'Test Tag', '发送http请求, url为: ' + baseUrl + url)
  return http.createHttp().request(baseUrl + url, options)
}

export class Request implements RequestTask {
  private url: string
  private options: http.HttpRequestOptions
  private callback: (err: Error, res: http.HttpResponse) => void

  init(
    url: string,
    options: http.HttpRequestOptions,
    callback: (err: Error, res: http.HttpResponse) => void
  ) {
    this.url = baseUrl + url
    this.options = options
    this.callback = callback
  }

  request() {
    hilog.debug(0x0000, 'Test Tag', '发送http请求, url为: ' + this.url)
    request.request(this.url, this.options, this.callback)
  }
}

export interface ResponseResult {
  code: number
  msg: string
  result: object
}