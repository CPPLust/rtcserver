﻿cmake_minimum_required(VERSION 3.8)

project(windemo)

include_directories(
	${XRTC_DIR}
	${XRTC_THIRD_PARTY_DIR}/include
	${XRTC_DIR}/../xrtc-third_party/NIM_Duilib_Framework
)

link_directories(
	${XRTC_THIRD_PARTY_DIR}/lib
	${XRTC_DIR}/../xrtc-third_party/out/build/x64-Debug/Windows/lib
)

file(GLOB all_src
	./src/*.cpp
)

add_executable(windemo ${all_src})
target_link_libraries(windemo base duilib Imm32 comctl32 xrtc)
set_target_properties(windemo PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /SAFESEH:NO")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})