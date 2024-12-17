#/*
# * Copyright 2023 Unionman Technology Co., Ltd.
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# * http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
# */
#!/bin/bash

set -e

########################################################
# establish build environment and build options value
# Please modify the following items according your build environment

ARCH=arm-amlogic

if [ -z $1 ]; then
    echo "error:arm or arm64 not set"
	echo "./build_vx.sh  arm"
	exit 1
fi
echo "build $1"

export AQROOT=/homeF/junwen.dai/a311d/buildroot_sdk_6487/buildroot_sdk
export SDK_DIR=$AQROOT/build/sdk

export VIVANTE_SDK_DIR=$SDK_DIR
export VIVANTE_SDK_INC=$SDK_DIR/include
#export VIVANTE_SDK_LIB=$SDK_DIR/drivers_64_exportdata
export OVXLIB_DIR=/$AQROOT/acuity-ovxlib-dev

case "$ARCH" in

arm-amlogic)
	echo $BUILD_OPTIONS_COMPILER_NBG_CASE
	if [ $1 = "arm64" ]; then
		export ARCH_TYPE=arm64
		export CPU_TYPE=cortex-a53
		export CPU_ARCH=armv8-a
		export FIXED_ARCH_TYPE=arm64
		export CROSS_COMPILE=aarch64-linux-gnu-

		export VIVANTE_SDK_LIB=$SDK_DIR/../so/drivers_64
		cp $VIVANTE_SDK_LIB/libovxlib.so $OVXLIB_DIR/lib/libovxlib.so
		cp $VIVANTE_SDK_LIB/libjpeg.a $OVXLIB_DIR/lib/libjpeg.a
			
	else
		export ARCH_TYPE=arm
		export CPU_TYPE=cortex-a9
		export CPU_ARCH=armv7-a
		export FIXED_ARCH_TYPE=arm-gnueabihf
		export CROSS_COMPILE=arm-linux-gnueabihf-
	
		export VIVANTE_SDK_LIB=$SDK_DIR/../so/drivers_32
		cp $VIVANTE_SDK_LIB/libovxlib.so $OVXLIB_DIR/lib/libovxlib.so
		cp $VIVANTE_SDK_LIB/libjpeg.a $OVXLIB_DIR/lib/libjpeg.a

	fi
;;

*)
   echo "ERROR: Unknown $ARCH, or not support so far"
   exit 1
;;

esac;

BUILD_OPTION_DEBUG=0
BUILD_OPTION_ABI=0
BUILD_OPTION_LINUX_OABI=0
BUILD_OPTION_NO_DMA_COHERENT=0
BUILD_OPTION_USE_VDK=1
if [ -z $BUILD_OPTION_EGL_API_FB ]; then
    BUILD_OPTION_EGL_API_FB=1
fi
if [ -z $BUILD_OPTION_EGL_API_DFB ]; then
    BUILD_OPTION_EGL_API_DFB=0
fi
if [ -z $BUILD_OPTION_EGL_API_DRI ]; then
    BUILD_OPTION_EGL_API_DRI=0
fi
if [ -z $BUILD_OPTION_X11_DRI3 ]; then
    BUILD_OPTION_X11_DRI3=0
fi
if [ -z $BUILD_OPTION_EGL_API_WL ]; then
    BUILD_OPTION_EGL_API_WL=0
fi
if [ -z $BUILD_OPTION_EGL_API_NULLWS ]; then
    BUILD_OPTION_EGL_API_NULLWS=0
fi
BUILD_OPTION_gcdSTATIC_LINK=0
BUILD_OPTION_CUSTOM_PIXMAP=0
BUILD_OPTION_USE_3D_VG=1
if [ -z $BUILD_OPTION_USE_OPENCL ]; then
    BUILD_OPTION_USE_OPENCL=0
fi
if [ -z $BUILD_OPTION_USE_OPENVX ]; then
    BUILD_OPTION_USE_OPENVX=1
fi
if [ -z $BUILD_OPTION_USE_VULKAN ]; then
    BUILD_OPTION_USE_VULKAN=0
fi
BUILD_OPTION_USE_FB_DOUBLE_BUFFER=0
BUILD_OPTION_USE_PLATFORM_DRIVER=1
if [ -z $BUILD_OPTION_ENABLE_GPU_CLOCK_BY_DRIVER ]; then
    BUILD_OPTION_ENABLE_GPU_CLOCK_BY_DRIVER=0
fi

if [ -z $BUILD_YOCTO_DRI_BUILD ]; then
    BUILD_YOCTO_DRI_BUILD=0
fi


BUILD_OPTION_USE_VXC_BINARY=1
BUILD_OPTION_GPU_CONFIG="vip9000nanodi_pid0xBE"

BUILD_OPTION_CONFIG_DOVEXC5_BOARD=0
BUILD_OPTION_FPGA_BUILD=0
BUILD_OPTIONS="NO_DMA_COHERENT=$BUILD_OPTION_NO_DMA_COHERENT"
BUILD_OPTIONS="$BUILD_OPTIONS USE_VDK=$BUILD_OPTION_USE_VDK"
BUILD_OPTIONS="$BUILD_OPTIONS EGL_API_FB=$BUILD_OPTION_EGL_API_FB"
BUILD_OPTIONS="$BUILD_OPTIONS EGL_API_DFB=$BUILD_OPTION_EGL_API_DFB"
BUILD_OPTIONS="$BUILD_OPTIONS EGL_API_DRI=$BUILD_OPTION_EGL_API_DRI"
BUILD_OPTIONS="$BUILD_OPTIONS X11_DRI3=$BUILD_OPTION_X11_DRI3"
BUILD_OPTIONS="$BUILD_OPTIONS EGL_API_NULLWS=$BUILD_OPTION_EGL_API_NULLWS"
BUILD_OPTIONS="$BUILD_OPTIONS gcdSTATIC_LINK=$BUILD_OPTION_gcdSTATIC_LINK"
BUILD_OPTIONS="$BUILD_OPTIONS EGL_API_WL=$BUILD_OPTION_EGL_API_WL"
BUILD_OPTIONS="$BUILD_OPTIONS ABI=$BUILD_OPTION_ABI"
BUILD_OPTIONS="$BUILD_OPTIONS LINUX_OABI=$BUILD_OPTION_LINUX_OABI"
BUILD_OPTIONS="$BUILD_OPTIONS DEBUG=$BUILD_OPTION_DEBUG"
BUILD_OPTIONS="$BUILD_OPTIONS CUSTOM_PIXMAP=$BUILD_OPTION_CUSTOM_PIXMAP"
BUILD_OPTIONS="$BUILD_OPTIONS USE_3D_VG=$BUILD_OPTION_USE_3D_VG"
BUILD_OPTIONS="$BUILD_OPTIONS USE_OPENCL=$BUILD_OPTION_USE_OPENCL"
BUILD_OPTIONS="$BUILD_OPTIONS USE_OPENVX=$BUILD_OPTION_USE_OPENVX"
BUILD_OPTIONS="$BUILD_OPTIONS USE_VULKAN=$BUILD_OPTION_USE_VULKAN"
BUILD_OPTIONS="$BUILD_OPTIONS USE_FB_DOUBLE_BUFFER=$BUILD_OPTION_USE_FB_DOUBLE_BUFFER"
BUILD_OPTIONS="$BUILD_OPTIONS USE_PLATFORM_DRIVER=$BUILD_OPTION_USE_PLATFORM_DRIVER"
BUILD_OPTIONS="$BUILD_OPTIONS ENABLE_GPU_CLOCK_BY_DRIVER=$BUILD_OPTION_ENABLE_GPU_CLOCK_BY_DRIVER"
BUILD_OPTIONS="$BUILD_OPTIONS CONFIG_DOVEXC5_BOARD=$BUILD_OPTION_CONFIG_DOVEXC5_BOARD"
BUILD_OPTIONS="$BUILD_OPTIONS FPGA_BUILD=$BUILD_OPTION_FPGA_BUILD"
BUILD_OPTIONS="$BUILD_OPTIONS YOCTO_DRI_BUILD=$BUILD_YOCTO_DRI_BUILD"
BUILD_OPTIONS="$BUILD_OPTIONS _GLIBCXX_USE_CXX11_ABI=1"
BUILD_OPTIONS="$BUILD_OPTIONS USE_VXC_BINARY=$BUILD_OPTION_USE_VXC_BINARY"
BUILD_OPTIONS="$BUILD_OPTIONS GPU_CONFIG=$BUILD_OPTION_GPU_CONFIG"

BUILD_OPTIONS="$BUILD_OPTIONS USE_VSC_LITE=1"

export PATH=$TOOLCHAIN/bin:$PATH

make -f makefile.linux $BUILD_OPTIONS V_TARGET=clean
make -f makefile.linux $BUILD_OPTIONS V_TARGET=install  2>&1 | tee $AQROOT/linux_build_sample.log

########################################################
# other build/clean commands to build/clean specified items, eg.
#
# cd $AQROOT; make -f makefile.linux $BUILD_OPTIONS gal_core V_TARGET=clean || exit 1
# cd $AQROOT; make -f makefile.linux $BUILD_OPTIONS gal_core V_TARGET=install || exit 1

