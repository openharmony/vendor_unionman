export class DeviceDataModel {
  public deviceId: string
  public deviceName: string
  public deviceType: string
  public checked: boolean
  public networkId: string
  public range: number
  constructor(deviceId: string, deviceName: string, deviceType: string, checked: boolean,
              networkId: string, range: number) {
    this.deviceId = deviceId
    this.deviceName = deviceName
    this.deviceType = deviceType
    this.checked = checked
    this.networkId = networkId
    this.range = range
  }
}