/*
 * Copyright 2022 Unionman Technology Co., Ltd.
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

import geolocation from '@ohos.geolocation';

let date = new Date

export function getFullYear(): string {
  return date.getFullYear() + '年'
}

export function getDate(): string {
  return date.getMonth() + 1 + '月' + date.getDate() + '日'
}

export function getTime(): string {
  return (date.getHours() < 10 ? '0' + date.getHours() : date.getHours()) + ':' + (date.getMinutes() < 10 ? '0' + date.getMinutes() : date.getMinutes())
}

export function getSeconds(): string {
  return ':' + (date.getSeconds() < 10 ? '0' + date.getSeconds() : date.getSeconds())
}