# 0penHarmony 软总线的分布式视频播放器  
## 简介
分布式视频播放器是一个基于openHarmony的应用程序，它允许用户在多个设备上播放和同步视频内容。
## 使用说明
### 功能介绍
1. 远程设备连接: 通过DeviceDialog组件选择远程设备。
2. 数据流转: 支持视频列表数据在多个设备间同步。
3. 播放控制: 包括播放、暂停、快进、快退等操作。
### 使用方法
1. 启动应用后，点击“设置”进入设置页面；
2. 在“设备列表”中选择要连接的远程设备；
3. 根据需要配置“流转模式”（跨端迁移、多端协同模式）；
4. 开始播放视频。
## 工程目录
```
entry/src/main/ets/
|---common
|   |---BasicDataSource.ets                 // 懒加载数据
|   |---DeviceDialog.ets                    // 分布式设备列表弹窗
|   |---TitleBar.ets                        // 应用标题栏组件，包含导航和设置按钮
|---model
|   |---DistributedDataModel.ts             // 封装分布式数据类
|   |---Logger.ts                          // 日志工具
|   |---KvStoreModel.ts                    // kvstore对象操作类
|   |---RemoteDeviceModel.ts               // 远程设备操作类
|---utils
|   |---AVPlayerUtils.ts                    // 封装AVPlayer工具类
|   |---DateTimeUtil.ts                    // 日期时间工具类
|   |---globalThis.ts                       
|   |---MediaUtils.ts                       // 媒体库工具类
|   |---utils.ts                           // 自定义工具类
|---pages
|   |---videolist.ets                       // 视频列表页面，展示视频资源
|   |---Settings.ets                        // 设置页
|---Viewmodel
|   |---itemCategory.ets                      
```
## 效果预览

| 首页                            | 视频                          |
|-------------------------------| ------------------------------- |
| ![img.png](img.png)           |![img_1.png](img_1.png) |
| **设备弹窗**                      | **设置**                        |
| ![img_2.png](img_2.png)       | ![img_3.png](img_3.png)  |

## 核心代码
````
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
    Logger.info(TAG, `startDeviceDiscovery ${SUBSCRIBE_ID}`)
    try {
      this.deviceManager.startDeviceDiscovery(info)
    } catch (error) {
      Logger.error(TAG, `startDeviceDiscovery throw error, code=${error.code} message=${error.message}`)
    }
  }
````
代码定义了一个名为 startDeviceDiscovery 的方法,作用是初始化并启动一个设备发现过程，通过生成一个随机的订阅ID和配置一组特定的搜索参数，以便在分布式网络环境中搜索并识别可用的设备。此方法调用 deviceManager 的 startDeviceDiscovery 接口，尝试发现设备。
````
 try {
        this.deviceManager.authenticateDevice(device, authParam, (err, data) => {
          if (err) {
            Logger.error(TAG, `authenticateDevice error: ${JSON.stringify(err)}`)
            this.authCallback = null
            return
          }
          Logger.info(TAG, `authenticateDevice succeed: ${JSON.stringify(data)}`)
          this.authCallback = callBack
          if (this.authCallback !== null) {
            this.authCallback(device)
            this.authCallback = null
          }
        })
      } catch (error) {
        Logger.error(TAG, `authenticateDevice throw error, code=${error.code} message=${error.message}`)
      }
    }
  }
````
## 下载
如需单独下载本工程，执行如下命令：

```
git init
git config core.sparsecheckout true
echo unionpi_tiger/sample_api10/app/DistributedVideoPlayer/ > .git/info/sparse-checkout
git remote add origin https://gitee.com/openharmony/vendor_unionman.git

```
