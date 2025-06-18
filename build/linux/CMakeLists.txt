cmake_minimum_required(VERSION 2.6)

project(xrtcserver)

set( TOP_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../.. )
set( SRC_PATH ${TOP_PATH}/rtcserver/src)


message(${TOP_PATH})
message(${SRC_PATH})

include_directories( ${SRC_PATH}/ 
					 ${SRC_PATH}/base/
	 				"${TOP_PATH}/depend/centos/third_party"
	 				"${TOP_PATH}/depend/centos/third_party/libyuv/include"
    					"${TOP_PATH}/depend/centos/absl/include"
    					"${TOP_PATH}/depend/centos"
    					"${TOP_PATH}/depend/centos/yaml/include/"
   					"${TOP_PATH}/depend/centos/ev/include"
   					"${TOP_PATH}/depend/centos/openssl/include"
					 ${TOP_PATH}/3rdpart/libwebrtc/src 
					 )

message(${CMAKE_CURRENT_SOURCE_DIR})

SET(CMAKE_EXE_LINKER_FLAGS " -no-pie")

add_definitions("-g -pipe -W -Wall -fPIC -std=c++14 -DOS_LINUX  -DWEBRTC_POSIX -DWEBRTC_LINUX")


file(GLOB all_src
        "${SRC_PATH}/*.cpp"
        "${SRC_PATH}/base/*.cpp"
        "${SRC_PATH}/server/*.cpp"
        "${SRC_PATH}/stream/*.cpp"
        "${SRC_PATH}/pc/*.cpp"
        "${SRC_PATH}/ice/*.cpp"
)


link_directories(
        "${TOP_PATH}/3rdpart/libwebrtc/out/"
        "${TOP_PATH}/depend/centos/yaml/lib64"
        "${TOP_PATH}/depend/centos/third_party/jsoncpp/"
        "${TOP_PATH}/depend/centos/absl/lib64"
        "${TOP_PATH}/depend/centos/openssl/lib"
        "${TOP_PATH}/depend/centos/ev/lib"
        )


add_executable(xrtcserver ${all_src})


target_link_libraries(xrtcserver  libyaml-cpp.a  libjsoncpp.a     libwebrtc.a  libev.a libabsl_strings.a libabsl_throw_delegate.a  libssl.a libcrypto.a  -lpthread -ldl)


