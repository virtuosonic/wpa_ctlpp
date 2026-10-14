cmake_minimum_required(VERSION 3.16)

project(wpa_ctrl_cpp LANGUAGES C CXX)

add_library(wpa_ctrl
    src/wpa_ctrl.c
    src/wpa_ctrl.cpp
	src/wpa_debug.c
	src/wpabuf.c
	src/trace.c
	src/utils/common.c
	src/utils/os_posix.c
)
add_library(wpa_ctrl::wpa_ctrl ALIAS wpa_ctrl)

target_include_directories(wpa_ctrl
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/utils
)
target_compile_features(wpa_ctrl PUBLIC cxx_std_17)
set_target_properties(wpa_ctrl PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED YES)

if(WIN32)
    target_compile_definitions(wpa_ctrl PRIVATE
        CONFIG_CTRL_IFACE
        CONFIG_CTRL_IFACE_NAMED_PIPE
        CONFIG_NATIVE_WINDOWS
    )
elseif(UNIX)
    target_compile_definitions(wpa_ctrl PRIVATE
        CONFIG_CTRL_IFACE
        CONFIG_CTRL_IFACE_UNIX
        _POSIX_C_SOURCE=200809L
    )
else()
    message(FATAL_ERROR "wpa_ctrl supports Unix sockets and Windows named pipes")
endif()