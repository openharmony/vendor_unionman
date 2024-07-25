#TV端适配

###介绍
本应用主要是用于tv视频播放和遥控追踪界面，使用XXX等接口和红外驱动，实现了视频播放，暂停，快进跟遥控等功能。
使用说明

###功能
1、在主界面，可以遥控点击推荐、分区、热门、电视剧等按钮进入对应界面，点击视频进入时评播放界面；
2、在播放界面，可以通过遥控确认按钮暂停或播放视频， 左右方向键控制视频进度，按住数字键1收藏视频。

###工程目录
applications_launcher\product\phone\src\main\ets
│  list.txt
│  
├─Application
│      AbilityStage.ts
│      
├─common
│  │  LauncherLayoutPreLoader.ts
│  │  PhoneFolderLayoutConfig.ts
│  │  PhoneFormLayoutConfig.ts
│  │  PhoneLauncherLayoutStyleConfig.ts
│  │  PhonePageDesktopGridStyleConfig.ts
│  │  PhonePresetDockItem.ts
│  │  PhoneSmartDockLayoutConfig.ts
│  │  PhoneSmartDockStyleConfig.ts
│  │  PhoneStage.ts
│  │  
│  ├─configs
│  │      DefaultLayoutConfig.ts
│  │      PhoneFolderLayoutInfo.ts
│  │      PhoneFormLayoutInfo.ts
│  │      
│  ├─constants
│  │      PhonePresetStyleConstants.ts
│  │      StyleConstants.ts
│  │      
│  └─pics
│          icon.png
│          ic_arrow_right.svg
│          ic_desktop_servicewidgets.svg
│          ic_form_center.svg
│          ic_form_mgr_back.png
│          ic_public_add.svg
│          ic_public_add_black.svg
│          ic_public_add_norm.svg
│          ic_public_app.svg
│          ic_public_back.svg
│          ic_public_cancel.svg
│          ic_public_copy.svg
│          ic_public_delete.svg
│          ic_public_delete_full.svg
│          ic_public_edit.svg
│          ic_public_lock.svg
│          ic_public_remove.svg
│          ic_public_settings.svg
│          ic_rename.svg
│          ic_wallpaper_form_manager.jpg
│          ic_wallpaper_recent.jpg
│          img_wallpaper_default.jpg
│          img_wallpaper_default1.jpg
│          
├─compon
│      hea.ets
│      
├─MainAbility
│      MainAbility.ts
│      
└─pages
        EmptyPage.ets
        EntryView.ets
        FormManagerView.ets
        FormServiceView.ets
        phb.ets
        RecentView.ets
        shoucang.ets
        SubDisplayWallpaperPage.ets
        videoview.ets
        




###约束和限制
1、开发环境DevEco Studio for OpenHarmony: 版本号大于3.0.0.900，下载安装OpenHarmony SDK API Version 9。（初始的IDE配置可以参考IDE的使用文档）
2、语言版本ArkTS。
3、本应用涉及系统接口，需要配置系统应用签名跟手动替换Full SDK才能编译通过，具体操作可参考https://gitee.com/openharmony/applications_launcher/tree/OpenHarmony-4.0-Release/。

###注意
本应用涉及到驱动为NEC红外驱动解码。
将uart_test推到hdc 下执行