# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(DlnaPlayer)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# build lib dlna_dmr
ADD_SUBDIRECTORY(dlna)

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include
                    ${NATIVERENDER_ROOT_PATH}/upnp/include
                    ${NATIVERENDER_ROOT_PATH}/upnp/include/inc
                    ${NATIVERENDER_ROOT_PATH}/upnp/threadutil
                    ${NATIVERENDER_ROOT_PATH}/upnp/ixml/include
                    ${NATIVERENDER_ROOT_PATH}/dlna/include
                    ${NATIVERENDER_ROOT_PATH}/dlna/include/common
                    ${NATIVERENDER_ROOT_PATH}/dlna/include/dmr
                    #securec
                    ${NATIVERENDER_ROOT_PATH}/securec
                    #json
                    ${NATIVERENDER_ROOT_PATH}/json/include/
                    ${NATIVERENDER_ROOT_PATH}/json/include/nlohmann

                    ${NATIVERENDER_ROOT_PATH}/dlna_manager)

add_library(entry SHARED hello.cpp
                dlna_manager/dlna_manager.cpp
                dlna_manager/dlnaplayer_listener.cpp
                dlna_manager/common_napi.cpp)

find_library( hilog-lib
              hilog_ndk.z )
target_link_libraries(entry PUBLIC libace_napi.z.so)
target_link_libraries(entry PUBLIC ${hilog-lib})
target_link_libraries(entry PUBLIC libuv.so)
target_link_libraries(entry PUBLIC dlna_dmr)
target_link_libraries(entry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/upnp/libixml.a)
target_link_libraries(entry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/upnp/libupnp.a)
target_link_libraries(entry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/json/libnlohmann_json_static.a)
target_link_libraries(entry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/securec/libsec_static.a)