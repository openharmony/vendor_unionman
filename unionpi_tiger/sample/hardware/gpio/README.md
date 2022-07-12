
gpio控制需要在root用户模式下进行
su
本例子只是模拟一个gpio口（GPIO_09）来做设置out模式，且将其设置为高电平，有需要的话可以自己设置程序来控制gpio
以操作
echo 386 > /sys/class/gpio/export 
echo out > /sys/class/gpio/gpio386/direction
echo 1 > /sys/class/gpio/gpio386/value
echo 0 > /sys/class/gpio/gpio386/value
