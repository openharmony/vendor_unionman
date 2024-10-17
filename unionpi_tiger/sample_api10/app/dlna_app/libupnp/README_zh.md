libupnp

简介
本仓库基于官方版本branch-1.14.x分支
> https://github.com/pupnp/pupnp


libupnp为投屏应用提供了设备发现、控制，事件订阅媒体管理，内容传输功能。

libupnp包括四个组件：

	SSDP协议：实现设备信息的广播（NOTIFY）、发现（M-Search），用组播/单播可以完成设备的上线和离线通知，通过组播完成设备搜索，通过单播响应搜索。
	SOAP协议：完成control和device信息交互，实现设备的控制和响应。
	GENA协议：完成设备事件消息的订阅和推送，实现control和device的状态同步。
	XML生成/解析器：生成和解析XML格式文件。

接口说明

libupnp的一些常用接口说明：

	UpnpInit2：libupnp的初始化调用其他API之前必须要先调用此函数，单实例中只能调用一次。
	UpnpSetWebServerRootDir：设置内部web服务器的文档根目录。
	UpnpRegisterRootDevice，UpnpRegisterRootDevice2，UpnpRegisterRootDevice3，UpnpRegisterRootDevice4：四种注册设备的接口，每种传参有区别。
	UpnpSendAdvertisement：发送本设备和支持的服务的发现通知。
	
更多API详解请参考docs/UPnP_Programming_Guide.pdf或者upnp/inc/upnp.h和ixml/inc/ixml.h。

编译

使用OpenHarmony中编译部件的方式进行编译即可：

1.主干代码下载
```
repo init -u https://gitee.com/openharmony/manifest.git -b master --no-repo-verify
repo sync -c
repo forall -c 'git lfs pull'
```
2.预处理
```
./build/prebuilts_download.sh
```
3.编译
```
./build.sh --product-name rk3568 --build-target libixml
./build.sh --product-name rk3568 --build-target libupnp
```

编译生成物对应路径：
```
./out/rk3568/obj/third_party/libupnp/libixml.a
./out/rk3568/obj/third_party/libupnp/libupnp.a
```