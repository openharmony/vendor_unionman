# the minimum version of CMake.
cmake_minimum_required(VERSION 3.5.0)
#project(TT2)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

if(DEFINED PACKAGE_FIND_FILE)
    include(${PACKAGE_FIND_FILE})
endif()

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include
                    ${NATIVERENDER_ROOT_PATH}/../upnp/include
                    ${NATIVERENDER_ROOT_PATH}/../upnp/include/inc
                    ${NATIVERENDER_ROOT_PATH}/../upnp/threadutil
                    ${NATIVERENDER_ROOT_PATH}/../upnp/ixml/include
                    include
                    include/common
                    include/dmr
                    #securec
                    ${NATIVERENDER_ROOT_PATH}/../securec
                    #json
                    ${NATIVERENDER_ROOT_PATH}/../json/include/
                    ${NATIVERENDER_ROOT_PATH}/../json/include/nlohmann)

aux_source_directory(./src DIR_SRCS)
add_library(dlna_dmr
                src/dlna_controller.cpp
                src/dmr_listener.cpp
                src/dmr_manager.cpp
                src/dmr/dlna_dmr.cpp
                src/dmr/dmr_av_transport.cpp
                src/dmr/dmr_connection_manager.cpp
                src/dmr/dmr_rendering_control.cpp
                src/common/dlna_buffer_manager.cpp
                src/common/dlna_common.cpp
                src/common/dlna_web_server.cpp
                src/common/udp_server.c
)

#add_library(entry SHARED napi_init.cpp test_manager.cpp
#                dlnaplayer_listener.cpp
#                dlna/src/dlna_controller.cpp
#                dlna/src/dmr_listener.cpp
#                dlna/src/dmr_manager.cpp
#                dlna/src/dmr/dlna_dmr.cpp
#                dlna/src/dmr/dmr_av_transport.cpp
#                dlna/src/dmr/dmr_connection_manager.cpp
#                dlna/src/dmr/dmr_rendering_control.cpp
#                dlna/src/common/dlna_buffer_manager.cpp
#                dlna/src/common/dlna_common.cpp
#                dlna/src/common/dlna_web_server.cpp
#                )

find_library( hilog-lib
              hilog_ndk.z )
target_link_libraries(dlna_dmr PUBLIC libace_napi.z.so)
target_link_libraries(dlna_dmr PUBLIC ${hilog-lib})
target_link_libraries(dlna_dmr PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../upnp/libixml.a)
target_link_libraries(dlna_dmr PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../upnp/libupnp.a)
target_link_libraries(dlna_dmr PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../json/libnlohmann_json_static.a)
target_link_libraries(dlna_dmr PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../securec/libsec_static.a)