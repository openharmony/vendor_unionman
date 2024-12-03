#说明文档

#项目结构

turnning_sensor1.0
|---app                                         //具体页面文件是app/finnal_work/entry/src/main/ets/pages/Index.ets目前只是文本的预览，还没有实现数据的传输
|---hardware
|   |---finnalwork
|   |   |---main.c			                    //纯南向代码，目前实现了旋转编码器记录旋转格数的功能
|   |   |---um_gpio.c
|   |   |---um_gpio.h
|   |   |---BUILD.gn
|   |---BUILD.gn.gn
|---napi
|   |---napisubsys
|   |   |---turnning_component
|   |   |   |---turnning_demo
|   |   |   |   |---turnning.cpp                //南向具体实现，创建了一个线程将死循环封在里面，期望北向调用时可以创建线程在后台一直运行
|   |   |   |   |---um_gpio.c
|   |   |   |   |---um_gpio.h
|   |   |   |   |---@ohos.turnning_demo.d.ts    //目前只有一个接口，变量不知道怎么定义
|   |   |   |   |---BUILD.gn
|---README.md
|---BUILD.gn

#项目简介

目前通过旋转该编码器，可以记录旋转的格数，按下编码器可以将数据清零。后续会新增其他协议以完善功能
