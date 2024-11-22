# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(MetaRtc VERSION 2.6.1 )

#ADD_SUBDIRECTORY(libmetartc7)
#ADD_SUBDIRECTORY(libmetartccore7)
#ADD_SUBDIRECTORY(thirdparty/libsrtp-main)
#ADD_SUBDIRECTORY(thirdparty/usrsctp-master)
#ADD_SUBDIRECTORY(thirdparty/libyuv)
#ADD_SUBDIRECTORY(thirdparty/opus-master)
#ADD_SUBDIRECTORY(metaplayer7)
#ADD_SUBDIRECTORY(metapushstream7)
#ADD_SUBDIRECTORY(render)

set(NATIVE_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${NATIVE_ROOT_PATH}/include
                    ${NATIVE_ROOT_PATH}/render
                    ${NATIVE_ROOT_PATH}/render/include)

link_directories(${NATIVE_ROOT_PATH}/thirdparty/lib)

add_definitions(-DOHOS_PLATFORM)

add_link_options(-Wl,-Bsymbolic)

add_library(entry SHARED
        render/render/egl_core.cpp
        render/render/plugin_render.cpp
        render/render/plugin_push.cpp
        render/manager/plugin_manager.cpp
        render/napi_init.cpp
        render/src/VDecEncNdkSample.cpp
        #render/src/VEncoderNdkSample.cpp
        render/src/videoenc_ndk_sample.cpp
        render/src/AudioWrapper.cc
        render/src/opensles_common.cc
        render/src/opensles_player.cc
        render/src/opensles_recorder.cc
)

find_library(
    # Sets the name of the path variable.
    EGL-lib
    # Specifies the name of the NDK library that
    # you want CMake to locate.
    EGL
)

find_library(
    # Sets the name of the path variable.
    GLES-lib
    # Specifies the name of the NDK library that
    # you want CMake to locate.
    GLESv3
)

find_library(
    # Sets the name of the path variable.
    hilog-lib
    # Specifies the name of the NDK library that
    # you want CMake to locate.
    hilog_ndk.z
)

find_library(
    # Sets the name of the path variable.
    libace-lib
    # Specifies the name of the NDK library that
    # you want CMake to locate.
    ace_ndk.z
)

find_library(
    # Sets the name of the path variable.
    libnapi-lib
    # Specifies the name of the NDK library that
    # you want CMake to locate.
    ace_napi.z
)

find_library(
    # Sets the name of the path variable.
    libuv-lib
    # Specifies the name of the NDK library that
    # you want CMake to locate.
    uv
)

target_link_libraries(entry PUBLIC
    ${EGL-lib} ${GLES-lib} ${hilog-lib} ${libace-lib} ${libnapi-lib} ${libuv-lib})
target_link_libraries(entry PUBLIC libhilog_ndk.z.so libace_napi.z.so)
target_link_libraries(entry PUBLIC libnative_media_codecbase.so)
target_link_libraries(entry PUBLIC libnative_media_core.so)
target_link_libraries(entry PUBLIC libnative_media_venc.so)
target_link_libraries(entry PUBLIC libnative_media_vdec.so)
target_link_libraries(entry PUBLIC libnative_media_adec.so)
target_link_libraries(entry PUBLIC libOpenSLES.so)
target_link_libraries(entry PUBLIC metartccore7 srtp2 usrsctp metartc7
    metartc_player metartc_pushstream ssl crypto hilog speex speexdsp yuv openh264 opus ffmpeg)