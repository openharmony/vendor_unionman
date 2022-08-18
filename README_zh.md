# vendor_unionman

## 介绍

该仓库托管Unionman开发板(或产品)，编译框架适配、解决方案参考代码和脚本。

## 约束

支持Unionpi_Tiger (Amlogic A311D)。

## 目录

```
vendor/unionman/          # vendor_unionman 仓的仓库路径
├── LICENSE
├── OAT.xml
├── README_zh.md
└── unionpi_tiger         # Unionpi Tiger开发板目录
    ├── audio             # audio配置目录
    ├── config.json       # 产品模块详细配置目录
    ├── hals              # hal层相关配置目录
    ├── hdf_config        # hdf相关hcs目录
    ├── power_config      # 电源配置目录
    ├── product.gni
    └── resourceschedule
```

## 开发环境搭建

Unionpi Tiger使用参考 [使用教程](https://gitee.com/openharmony/device_board_unionman/blob/master/unionpi_tiger/README_zh.md)

## 参与贡献

[如何参与](https://gitee.com/openharmony/docs/blob/HEAD/zh-cn/contribute/%E5%8F%82%E4%B8%8E%E8%B4%A1%E7%8C%AE.md)

[Commit message规范](https://gitee.com/openharmony/device_qemu/wikis/Commit%20message%E8%A7%84%E8%8C%83?sort_id=4042860)

## 相关仓

- vendor_unionman

- [device_board_unionman](https://gitee.com/openharmony/device_board_unionman)

- [device_soc_amlogic](https://gitee.com/openharmony/device_soc_amlogic)
