# 一、配置cmake编译环境：
​	1、下载cmake
​		https://cmake.org/download/
​		注：版本要高于3.5.1
​	2、 安装cmake

```
tar -xvf cmake-3.*.*.tar
cd cmake-3.*.*
./bootstrap 
make
make install
```


​	3、 验证是否安装成功

```
cmake --version
```


​		显示cmake 版本即为成功

# 二、配置编译工具链
​	1、 下载工具链
​		64位：wget https://releases.linaro.org/components/toolchain/binaries/6.3-2017.05/aarch64-linux-gnu/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu.tar.xz
​		32位：wget https://releases.linaro.org/components/toolchain/binaries/6.3-2017.05/arm-linux-gnueabihf/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf.tar.xz
​	2、 aarch64-linux-gnu安装过程
​		a、解压，并放置在自己需要的文件夹内 
​			tar -xvJf ***.tar.xz
​		b、编辑bash.bashrc文件 
​			sudo vi ~/.bashrc
​		c、添加变量   
​			export PATH=path_to/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin:$PATH
​		d、更新环境变量   
​			source ~/.bashrc
​		e、检查环境是否加入成功   
​			echo $PATH     
​			下方出现刚添加的PATH即为成功
​		f、运行aarch64-linux-gnu-gcc -v，查看gcc安装版本

# 三、准备头文件、库、模型、模型输入文件

资源获取路径：

https://gitee.com/umspark/aml_npu_sdk

移动相关文件如下所示：

工作目录结构

```
├── linux_nn_sdk_V1.9
│   ├── include
│   │   ├── nn_sdk.h
│   │   └── nn_util.h
│   └── lib
│       ├── lib32
│       └── lib64
└── yolo
    ├── CMakeLists.txt
    ├── include
    │   ├── jconfig.h
    │   ├── jmorecfg.h
    │   └── jpeglib.h
    ├── main.cpp
    ├── nn_demo.h
    ├── opencv
    │   ├── include
    │   ├── lib32
    │   └── lib64
    └── postprocess_util.cpp
```

# 四、可执行文件编译
​	1、修改CMakeLists.txt
​		A、修改GCC_PATH：
​			修改line 6 :SET(GCC_PATH /xxx/xxx)
​			设置gcc-linaro路径：设置为第二步配置工具链中，解压后工具链放置目录。
​			例如，电脑上放置的路径是:/homeF/junwen.dai/a311d/linux_nnsdk_v1.9.4/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu
​			即设置：SET(GCC_PATH /homeF/junwen.dai/a311d/linux_nnsdk_v1.9.4)

		B、编译arm32或者arm64可执行文件：
			line4/line5为设置编译32位或者64位可执行文件。
			SET(ARM_32 arm_32)
			SET(ARM_64 arm_64)
			
			需要编译哪个，就注释掉另外一个即可，如当前需要编译32位的，即修改成：
			SET(ARM_32 arm_32)
			#SET(ARM_64 arm_64)
		D、line29及30修改可执行文件名称
		注：用户自建工程可将CMakeLists.txt文件拷贝到工程main函数同级目录下。
	
	2、编译
		在nnsdk路径下执行命令：
		A、cmake .    //当前路径生成makefile
		B、make      //编译
		生成的可执行文件在当前目录下

# 五、可执行文件运行

​	1、将lib和模型和模型输入图片放到/data目录下

可临时使用环境变量

```
export LD_LIBRARY_PATH=./lib
```

ld-linux-aarch64.so.1放到/lib/ld-linux-aarch64.so.1

给ld-linux-aarch64.so.1和sdk19_64权限

​	2.加载ko

```
insmod galcore.ko 
```

​	3、执行命令：

```
#执行程序 模型 图片
./sdk19_64 ./yolov5s.nb ./bus.jpg
```

打印：

```
# ./sdk19_64 ./yolov5s.nb ./bus.jpg
the input type should be 640*640*3
used time: 0.040822 s; fps: 24.496595
object_num:4
class:bus,prob:0.623733,[left = 88,right = 556,top = 126,bot = 464]
class:person,prob:0.872407,[left = 208,right = 287,top = 246,bot = 505]
class:person,prob:0.852847,[left = 109,right = 229,top = 231,bot = 538]
class:person,prob:0.864583,[left = 480,right = 560,top = 238,bot = 525]
```

