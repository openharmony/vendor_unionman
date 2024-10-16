1、DlnaPlayer目录: 
    1）、hap代码，通过点击"Start Dmr"启动DMR，初始化upnp等
    2）、libupnp.a和libixml.a文件，编译方式和更新见第2部分
    3）、libsec_static.a文件，编译方式和更新见第3部分

2、libupnp目录：编译方式为将代码放到SDK跟目录的third_party/下,用命令：./build.sh --product-name rk3568 --target-cpu arm64 --build-target libupnp --jobs 64 ，编译出64位的libupnp.a和libixml.a文件(编译命令根据实际的环境平台修改)，然后将库文件放到hap目录下的entry\src\main\cpp\upnp目录，重新编译运行hap即可

3、libsec_static.a：使用命令./build.sh --product-name rk3568 --target-cpu arm64 --build-target libsec_static --jobs 64，将编译生成的libsec_static.a文件放到hap目录下的entry\src\main\cpp\securec目录下，重新编译运行hap即可
