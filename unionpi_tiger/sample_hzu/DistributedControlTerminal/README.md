## 分布式控制终端

### 介绍

本示例展示了在eTS中分布式数据管理的使用，包括KVManager对象实例的创建和KVStore数据流转的使用。

通过设备管理接口[@ohos.distributedDeviceManager](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-distributedservice-kit/js-apis-distributedDeviceManager.md) ，实现设备之间的kvStore对象的数据传输交互；通过@ohos.i2cnapidemo、@ohos.adc_napi、@ohos.pwmtest等接口，实现实时温湿度显示、智能灯光功能和摄像头转动。

### 效果预览

![](..\DistributedControlTerminal\main.gif)

使用说明

依次介绍怎么连接两台设备，sample具有什么功能，怎样使用这些功能，以及使用后会达到什么效果;示例如下:

1.在两台设备的界面中弹出”是否允许“Kvstore"使用多设备协同?“按钮，在弹窗中选择“允许”。

2.两台设备连上相同网络。

3.拉起对端设备后，点击两台控制设备界面中的“温度/湿度”按钮，在一台设备界面上选择”本地“按钮，将其设为当前设备，在另外一台设备界面上选择”远程“按钮，将其设为对端设备；显示连接上之后，可以看到对端设备和当前设备界面温湿度数据保持一致。

4.退出“温度/湿度"界面后，点击两台控制设备界面中的“摄像头”按钮，当前设备的旋转角度便会反映到对端设备上。

5.操作对端设备，当前设备界面也会保持和对端设备界面显示一致。

### 工程目录

|---pages
|   |---index.ets                                        //首页
|   |---Lamp.ets                                       //灯光页面
|   |---camera.ets                                   //摄像头页面
|   |---Tem_Hum.ets                             //温湿度页面                   
|---model                                  
|   |---DeviceDataModel.ets               // 设备数据定义
|   |---KvStoreModel.ets                     // kvstore对象操作类
|   |---NoteDataModel.ets                 // 笔记数据定义
|   |---RemoteDeviceModel.ets        // 远程设备操作类
|---common                                  
|   |---BasicDataSource.ets                 // 笔记数据源操作                              
|   |---Dialog.ets                                   // 设备管理对话框
|   |---NoteItem.ets                            // 笔记组件
|   |---TitleBar.ets                               // title组件                             

### 具体实现

#### 管理Kvstore

1、页面初始化时获取此应用所需能力：引入@ohos.data.distributedKVStore初始化kvstore数据库并对使用kvstore.on数据change进行监听，通过appstorge判断获取相应的key判断是否是分布式节点，[源码](https://gitee.com/openharmony/applications_app_samples/blob/master/code/SuperFeature/DistributedAppDev/Kvstore/entry/src/main/ets/pages/Index.ets) 。 

2、如果是分布式节点，如果数据发生变化，处理数据并使用.noteDataSource()进行reload数据。

3、页面通过kvStore对象进行增删改查会触发其他已连接设备的kvStore.on监听。

#### 管理分布式设备（节点）

1、创建设备管理对象，并指定参数kvstore应用包deviceManager.createDeviceManager("ohos.samples.kvstore", (error, value) => {})

2、获取可信设备列表，"this.deviceManager.getTrustedDeviceListSync())" 。 

3、监听设备状态，"this.deviceManager.on('deviceStateChange', (data) => {})"，从而对可信设备列表管理。

### 相关权限

允许不同设备间的数据交换：[ohos.permission.DISTRIBUTED_DATASYNC](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/security/AccessToken/permissions-for-all.md#ohospermissiondistributed_datasync) 。

允许系统应用获取分布式设备的认证组网能力：[ohos.permission.ACCESS_SERVICE_DM](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/security/AccessToken/permissions-for-system-apps.md#ohospermissionaccess_service_dm)

### 依赖

无依赖

### 约束与限制

1.本示例仅支持标准系统上运行。

3.本示例已适配API version 9版本FULL SDK，版本号：3.2.11.9,使用Full SDK时需要手动从镜像站点获取，并在DevEco Studio中替换，具体操作可参考[替换指南](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/faqs/full-sdk-switch-guide.md) 。

4.本示例需要使用DevEco Studio 3.1 Beta2 (Build Version: 3.1.0.400, built on April 7, 2023)及以上版本才可编译运行。

3.本示例需要使用[@ohos.distributedDeviceManager](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/apis-distributedservice-kit/js-apis-distributedDeviceManager.md) 系统权限的系统接口。

### 下载

如需单独下载本工程，执行如下命令：

```
git init
git config core.sparsecheckout true
echo code/SuperFeature/DistributedAppDev/Kvstore/ > .git/info/sparse-checkout
git remote add origin https://gitee.com/openharmony/vendor_unionman.git
git pull origin master
```