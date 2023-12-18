# Yolo5sNapi

1.该样例为yolo5s的napi接口，通过ipc的方式调用服务来实现OH应用调用AI功能，使用该接口之前需要配合sample\hardware\ipc_server使用。sample\hardware\ipc_server编译后会在/bin路径下生成执行文件ipc_server，在调用接口之前需要执行ipc_server。



或者在https://gitee.com/umspark/aml_npu_sdk下载AI_server,将文件中的ipc.cfg拷贝到路径device\board\unionman\unionpi_tiger\config下，同时在build.gn中添加

```
+ohos_prebuilt_etc("ipc") {
+  source = "ipc.cfg"
+  module_install_dir = "etc"
+  install_images = [ "system" ]
+  install_enable = true
+  part_name = "unionman_products"
+}
group("init_configs") {
  deps = [
    +":ipc",
    ":50.usb.rules",
    ":distributed_hardware_components_cfg",
    ":fstab.A311D",
    ":fstab.required",
    ":init.A311D.cfg",
    ":init.A311D.sh",
    ":init.A311D.usb.cfg",
    ":install_list_capability_config",
    ":install_list_config",
    ":install_list_permissions_config",
    ":para_for_chip_prod",
    ":para_for_sys_prod",
    ":sdcard_mount.sh",
    ":selinux_config",
    ":udisk_mount.sh",
    ":uninstall_list_config",
  ]
}
```

如果是编译的32位需要在init/arm文件夹下修改init.A311D.cfg，如果编译的是64位则需要在init/arm64文件夹下修改init.A311D.cfg。

```
 "import" : [
            "/vendor/etc/init.A311D.usb.cfg",
+           "/etc/ipc.cfg"
    ],
      "cmds" : [
                "symlink /sys/kernel/debug /d",
                .........
+               "start ipc_shell_server",
                "symlink /dev/block/platform/soc/ffe07000.mmc/by-name /dev/block/by-name"
            ]
```

这样修改后，ipc_sever会在init脚本拉起来。

2.需要内置好AI所需动态库，将路径vendor\unionman\unionpi_tiger\sample\hardware\npu下的BUILD.gn加入到编译链，同时在链接https://gitee.com/umspark/aml_npu_sdk，对应下载ko，lib，并且需要按文件夹下的README.md编译出执行程序在改路径下。