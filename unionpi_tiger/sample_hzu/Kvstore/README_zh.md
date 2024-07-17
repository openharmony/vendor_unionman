### 工程目录
```
entry/src/main/ets/
|---pages
|   |---index.ets                           // 首页
|---model                                  
|   |---DeviceDataModel.ets                 // 设备数据定义
|   |---KvStoreModel.ets                    // kvstore对象操作类
|   |---NoteDataModel.ets                   // 笔记数据定义
|   |---RemoteDeviceModel.ets               // 远程设备操作类
|---common                                  
|   |---BasicDataSource.ets                 // 笔记数据源操作                              
|   |---Dialog.ets                          // 设备管理对话框
|   |---NoteItem.ets                        // 笔记组件
|   |---TitleBar.ets                        // title组件                             
```
### 具体实现
1、页面初始化时获取此应用所需能力：引入@ohos.data.distributedKVStore初始化kvstore数据库并对使用kvstore.on数据change进行监听，通过app
storge判断获取相应的key判断是否是分布式节点，[源码](entry/src/main/ets/pages/Index.ets) 。
2、如果是分布式节点，如果数据发生变化，处理数据并使用.noteDataSource()进行reload数据。
#### 管理分布式设备（节点）
1、创建设备管理对象，并指定参数kvstore应用包deviceManager.createDeviceManager("ohos.samples.kvstore", (error, value) => {})  
2、获取可信设备列表，"this.deviceManager.getTrustedDeviceListSync())"  。
3、监听设备状态，"this.deviceManager.on('deviceStateChange', (data) => {})"，从而对可信设备列表管理
### 相关权限
允许不同设备间的数据交换
### 约束与限制
1.本示例仅支持标准系统上运行。
3.本示例已适配API version 9版本FULL SDK，版本号：3.2.11.9,使用Full SDK时需要手动从镜像站点获取，并在DevEco Studio中替换，
4.本示例需要使用DevEco Studio 3.1 Beta2 (Build Version: 3.1.0.400, built on April 7, 2023)及以上版本才可编译运行。
3.本示例需要使用系统权限的系统接口。