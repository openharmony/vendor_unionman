## 2024惠州学院综合实训作品

####  介绍
基于OpenHarmony打造的视频播放器，适配TV端。

2024惠州学院综合实训作品


< img src="https://pic.imgdb.cn/item/6684e04cd9c307b7e9a86ace.png" width="200" height="180">
< img src="https://pic.imgdb.cn/item/6684e397d9c307b7e9af28d2.png" width="200" height="180">
< img src="https://pic.imgdb.cn/item/6684e0ddd9c307b7e9a9a09a.png" width="200" height="180">

#### 使用说明
可以使用红外遥控器按钮控制（模拟鼠标键盘输入）需要用到可以接受红外的mcu。
在主页点击左右两边的图片可以进入播放器播放，下方的按钮可以进行各种功能比如快进，暂停播放，显示控制栏和全屏等。
点击历史播放图标可以进入播放列表，选择视频播放。
点击src可以切换视频源播放。



#### 具体实现
Index.ets 主要设计了播放器的主页，以及播放器的进入入口，VideoList.ets是视频播放列表界面，VideoPage.ets主要运用了Video组件的组建属性和事件调用实现播放器功能的完成。

#### 约束与限制
本项目主要聚焦于智能电视设备中OpenHarmony系统对红外遥控功能的增强。在Unionpi Tiger(A311D)开发板上，通过TTL串口与外部基于STC8F1K08S2-28I-TSSOP20单片机的外接红外收发器相连，该单片机具备接收和解码红外遥控信号的能力。编写源码，让界面针对 TV 端设计，支持遥控按键控制，并且添加支持遥控按键控制的视频播放应用。主要用HUAWEI DevEco Studio 4.0 OpenHarmony API 10开发。

 硬件配置： 
（1）开发板：Unionpi Tiger(A311D)开发板 
（2）外接设备：集合红外收发器 STC8F1K08S2-28I-TSSOP20 的 MCU 小开发板 
（3）连接：Unionpi Tiger(A311D)开发板的 TTL 串口与外接 MCU 小开发板连


####  参与贡献
徐再立、胡天宝、严坚钊、王郓君、郑丽贤

#### 参考资料

+ [OpenHarmony官网](https://www.openharmony.cn/)
+ [OpenHarmony知识体系仓库](https://gitee.com/openharmony-sig/knowledge)
+ [OpenHarmony知识体系议题申报](https://docs.qq.com/sheet/DUUNpcWR6alZkUmFO)
+ [OpenHarmony知识体系例会纪要](https://gitee.com/openharmony-sig/sig-content/tree/master/knowlege/meetings)
+ [OpenHarmony开发样例共建说明](https://gitee.com/openharmony-sig/knowledge/blob/master/docs/co-construct_demos/README_zh.md)
+ [OpenHarmony知识体系-智能家居场景](https://gitee.com/openharmony-sig/knowledge_demo_smart_home)
+ [OpenHarmony知识体系-影音娱乐场景](https://gitee.com/openharmony-sig/knowledge_demo_entainment)