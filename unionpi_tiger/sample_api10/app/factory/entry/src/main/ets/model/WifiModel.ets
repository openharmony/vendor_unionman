/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
 */

import prompt from '@ohos.promptAction'
import wifi from '@ohos.wifiManager'
import Logger from './LoggerEts'

const TAG: string = 'WiFiModel'

export interface WifiType {
  ssid: string,
  bssid: string,
  securityType: wifi.WifiSecurityType,
  rssi: number,
  band: number,
  frequency: number,
  timestamp: number
}

export class WifiModel {
  async getScanInfos(): Promise<Array<WifiType>> {
    Logger.info(TAG, 'scanWifi begin')
    let wifiList: Array<WifiType> = []
    let result: Array<wifi.WifiScanInfo> = []
    try {
      result = await wifi.getScanResults()
    } catch (err) {
      Logger.info(TAG, `scan info err: ${JSON.stringify(err)}`)
      return wifiList
    }
    Logger.info(TAG, `scan info call back: ${result.length}`)
    for (let i = 0; i < result.length; ++i) {
      wifiList.push({
        ssid: result[i].ssid,
        bssid: result[i].bssid,
        securityType: result[i].securityType,
        rssi: result[i].rssi,
        band: result[i].band,
        frequency: result[i].frequency,
        timestamp: result[i].timestamp
      })
    }
    return wifiList
  }

  connectNetwork(scanInfo: wifi.WifiScanInfo, psw: string) {
    prompt.showToast({ message: 'connecting', duration: 5000 })
    Logger.debug(TAG, `connectNetwork bssid=${scanInfo.bssid}`)
    // 这里因为api问题，需要声明为any，已提单
    let deviceConfig: wifi.WifiDeviceConfig = {
      ssid: scanInfo.ssid,
      bssid: scanInfo.bssid,
      preSharedKey: psw,
      isHiddenSsid: false,
      securityType: scanInfo.securityType
    }
    try {
      wifi.connectToDevice(deviceConfig)
      Logger.debug(TAG, `connectToDevice success`)
    } catch (err) {
      Logger.debug(TAG, `connectToDevice fail err is ${JSON.stringify(err)}`)
    }
    try {
      wifi.addDeviceConfig(deviceConfig)
    } catch (err) {
      Logger.debug(TAG, `addDeviceConfig fail err is ${JSON.stringify(err)}`)
    }
  }

  resolveIP(ip: number) {
    let address: string = ip.toString()
    if (address === '0') {
      return '00:00:000:000'
    }
    address.substring(0, 2)
    return `${address.substring(0, 2)}:${address.substring(2, 4)}:${address.substring(4, 7)}:${address.substring(7, 10)}`
  }

  getIpInfo() {
    let ipInfoMap: Map<Resource, string> = new Map<Resource, string>();
    let ipInfo = wifi.getIpInfo()
    Logger.info(`${TAG} getIpInfo=${JSON.stringify(ipInfo)}`)
    ipInfoMap.set($r('app.string.ip_address'), this.resolveIP(ipInfo.ipAddress))
    ipInfoMap.set($r('app.string.gate_way'), this.resolveIP(ipInfo.gateway))
    ipInfoMap.set($r('app.string.net_mask'), this.resolveIP(ipInfo.netmask))
    ipInfoMap.set($r('app.string.primary_dns'), this.resolveIP(ipInfo.primaryDns))
    ipInfoMap.set($r('app.string.second_dns'), this.resolveIP(ipInfo.secondDns))
    ipInfoMap.set($r('app.string.server_ip'), this.resolveIP(ipInfo.serverIp))
    ipInfoMap.set($r('app.string.lease_duration'), ipInfo.leaseDuration.toString())
    return ipInfoMap
  }

  getCountryCode() {
    let countryCodeList: Map<Resource, string> = new Map<Resource, string>();
    let countryCode = wifi.getCountryCode()
    countryCodeList.set($r('app.string.country_code'), countryCode)
    return countryCodeList
  }

  getFeatureSupport() {
    let featureSupportedList: Map<Resource, string> = new Map<Resource, string>();
    featureSupportedList.set($r('app.string.infrastructure_feature'), wifi.isFeatureSupported(0x0001).toString()
    )
    featureSupportedList.set($r('app.string.ghz_feature'), wifi.isFeatureSupported(0x0002).toString())
    featureSupportedList.set($r('app.string.gas_anqp_feature'), wifi.isFeatureSupported(0x0004).toString())
    featureSupportedList.set($r('app.string.wifi_direct'), wifi.isFeatureSupported(0x0008).toString())
    featureSupportedList.set($r('app.string.soft_ap'), wifi.isFeatureSupported(0x0010).toString())
    featureSupportedList.set($r('app.string.wifi_aware'), wifi.isFeatureSupported(0x0040).toString())
    return featureSupportedList
  }
}