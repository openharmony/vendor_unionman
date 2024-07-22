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
import { Request, RequestTask } from './request'

interface TaskOption {
  start: (
    requestTask: RequestTask
  ) => void
  clear: () => void
}


class Task implements TaskOption {
  private timer: number
  private duration: number

  constructor() {
    this.duration = 1500
    this.timer = null
  }

  setDuation(duration: number) {
    this.duration = duration
  }

  getTime() {
    return this.timer
  }

  start(requestTask: RequestTask) {
    this.timer = setInterval(() => {
      hilog.debug(0x0000, 'request', '发起请求')
      requestTask.request()
    }, this.duration)
  }

  clear() {
    clearInterval(this.timer)
  }
}

interface RequestOptions {
  url: string
  options: http.HttpRequestOptions
  callback: (err: Error, res: http.HttpResponse) => void
  duartion?: number
}

export function runRequest(params: RequestOptions): number {
  let task: Task = new Task()
  if (params.duartion) {
    task.setDuation(params.duartion)
  }
  const request = new Request()
  request.init(
    params.url,
    params.options,
    params.callback
  )
  task.start(request)

  return task.getTime()
}