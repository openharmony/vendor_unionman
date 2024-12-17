#手势识别及触摸传感器串口读取demo

## 购买外设

[**Gravity: 手势识别带触摸传感器**](https://www.dfrobot.com.cn/goods-1994.html)

![](../figures/uart/1.jpg)

## 连线

- 红线 -- 3.3V
- 黑线 -- GND
- 绿线 -- RX

连线实物图

![](../figures/uart/2.jpg)

## 编译运行

1) 编译 打包 见https://gitee.com/openharmony/device_board_unionman/tree/master/unionpi_tiger#%E7%BC%96%E8%AF%91%E4%B8%8E%E8%B0%83%E8%AF%95

3. 打开`cmd窗口`利用hdc_std工具传输文件 (获取hdc_std工具可参考https://ost.51cto.com/posts/10190)

   `hdc_std file send Z:\openharmony\OpenHarmony-3.1-release\out\a311d\device_a311d\unionman_products\uart_test /data`

4. 与开发板交互

   `hdc_std shell`

5. 修改权限
`chmod 777 data/uart_test`

6. 运行！
`./data/uart_test`

## 结果

对着传感器挥动手，会打印如下信息

![](../figures/uart/3.png)
