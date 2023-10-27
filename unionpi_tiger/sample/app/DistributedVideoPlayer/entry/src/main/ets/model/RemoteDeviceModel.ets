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

import deviceManager from '@ohos.distributedHardware.deviceManager';

const RANDOM: number = 65536;
let SUBSCRIBE_ID = 100;

export class RemoteDeviceModel {
  public deviceLists: Array<deviceManager.DeviceInfo> = []
  public discoverLists: Array<deviceManager.DeviceInfo> = []
  private callback: () => void = null
  private authCallback: (device: deviceManager.DeviceInfo) => void = null
  private deviceManager: deviceManager.DeviceManager = undefined

  constructor() {
  }

  registerDeviceListCallback(callback) {
    if (typeof (this.deviceManager) !== 'undefined') {
      this.registerDeviceListCallbackImplement(callback)
      return
    }
    try {
      deviceManager.createDeviceManager("com.unionman.distributedvideoplayer", (error, value) => {
        if (error) {
          return
        }
        this.deviceManager = value
        this.registerDeviceListCallbackImplement(callback)
      })
    } catch (error) {
    }
  }

  deviceStateChangeActionOffline(device) {
    if (this.deviceLists.length <= 0) {
      this.callback()
      return
    }
    for (let j = 0; j < this.deviceLists.length; j++) {
      if (this.deviceLists[j].deviceId === device.deviceId) {
        this.deviceLists[j] = device
        break
      }
    }
    this.callback()
  }

  changeStateOnline(device) {
    this.deviceLists[this.deviceLists.length] = device
    this.callback()
  }

  registerDeviceListCallbackImplement(callback) {
    this.callback = callback
    if (this.deviceManager === undefined) {
      this.callback()
      return
    }
    try {
      let list = this.deviceManager.getTrustedDeviceListSync()
      if (typeof (list) != 'undefined' && typeof (list.length) != 'undefined') {
        this.deviceLists = list
      }
    } catch (error) {
    }

    this.callback()
    try {
      this.deviceManager.on('deviceStateChange', (data) => {
        if (data === null) {
          return
        }
        switch (data.action) {
          case deviceManager.DeviceStateChangeAction.ONLINE:
            this.changeStateOnline(data.device)
            break;
          case deviceManager.DeviceStateChangeAction.READY:
            this.discoverLists = []
            this.deviceLists.push(data.device)
            this.callback()
            try {
              let list = this.deviceManager.getTrustedDeviceListSync()
              if (typeof (list) !== 'undefined' && typeof (list.length) !== 'undefined') {
                this.deviceLists = list
              }
            } catch (error) {
            }
            this.callback()
            break;
          case deviceManager.DeviceStateChangeAction.OFFLINE:
          case deviceManager.DeviceStateChangeAction.CHANGE:
            this.deviceStateChangeActionOffline(data.device)
            try {
              let list = this.deviceManager.getTrustedDeviceListSync()
              if (typeof (list) !== 'undefined' && typeof (list.length) !== 'undefined') {
                this.deviceLists = list
              }
            } catch (error) {
            }
            this.callback()
            break
          default:
            break
        }
      })
      this.deviceManager.on('deviceFound', (data) => {
        if (data === null) {
          return
        }
        this.deviceFound(data)
      })
      this.deviceManager.on('discoverFail', (data) => {
      })
      this.deviceManager.on('serviceDie', () => {
      })
    } catch (error) {
    }
    this.startDeviceDiscovery()
  }

  deviceFound(data) {
    for (var i = 0;i < this.discoverLists.length; i++) {
      if (this.discoverLists[i].deviceId === data.device.deviceId) {
        return
      }
    }
    this.discoverLists[this.discoverLists.length] = data.device
    this.callback()
  }

  startDeviceDiscovery() {
    SUBSCRIBE_ID = Math.floor(RANDOM * Math.random())
    let info = {
      subscribeId: SUBSCRIBE_ID,
      mode: 0xAA,
      medium: 2,
      freq: 2,
      isSameAccount: false,
      isWakeRemote: true,
      capability: 0
    }
    try {
      this.deviceManager.startDeviceDiscovery(info)
    } catch (error) {
    }
  }

  authenticateDevice(device, callBack) {
    for (let i = 0; i < this.discoverLists.length; i++) {
      if (this.discoverLists[i].deviceId !== device.deviceId) {
        continue
      }
      let extraInfo = {
        'targetPkgName': 'com.unionman.distributedvideoplayer',
        'appName': 'Distributed VideoPlayer',
        'appDescription': 'Distributed VideoPlayer',
        'business': '0'
      }
      let authParam = {
        'authType': 1,
        'appIcon': '',
        'appThumbnail': '',
        'extraInfo': extraInfo
      }
      try {
        this.deviceManager.authenticateDevice(device, authParam, (err, data) => {
          if (err) {
            this.authCallback = null
            return
          }
          this.authCallback = callBack
          if (this.authCallback !== null) {
            this.authCallback(device)
            this.authCallback = null
          }
        })
      } catch (error) {
      }
    }
  }
}