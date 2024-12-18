/*
 * Copyright 2023 Unionman Technology Co., Ltd.
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

/**
 * unionman a311d
 * @since 8
 * @devices phone, tablet
 * @import import keyTest from '@ohos.keyTest';
 * @permission N/A
 */

declare namespace spitest {
  /**
   * 获取电源键按键状态
   * @param callback
   *     回调函数，返回0：蓝牙接口测试不合格；返回1：蓝牙接口测试合格
   */
  function UM_spi_test(): Promise<number>;
}

export default spitest;