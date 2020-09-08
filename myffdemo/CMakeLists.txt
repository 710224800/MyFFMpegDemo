cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_VERBOSE_MAKEFILE on) #显示详细的原始编译信息，主要用于定位一些链接错误，看看库路径什么的是否配置对。

#获取上级目录
get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} PATH)
#使用单个库版本
set(LIBFFMPEG_DIR ${PARENT_DIR}/ffmpeg-single)
#使用单个yuv库
set(LIBYUV_DIR ${PARENT_DIR}/libyuv-single)


add_library(ffmpeg-single
        SHARED
        IMPORTED)
set_target_properties(ffmpeg-single
        PROPERTIES IMPORTED_LOCATION
        ${LIBFFMPEG_DIR}/libs/${ANDROID_ABI}/libffmpeg.so
        )


add_library(libyuv
        SHARED
        IMPORTED)
set_target_properties(libyuv
        PROPERTIES IMPORTED_LOCATION
        ${LIBYUV_DIR}/libs/${ANDROID_ABI}/libyuv.so
        )

# libyuv库头文件
include_directories(${LIBYUV_DIR}/libs/${ANDROID_ABI}/include)

# libffmpeg库头文件
include_directories(${LIBFFMPEG_DIR}/libs/${ANDROID_ABI}/include)

add_library( native-lib
             SHARED
             src/main/cpp/native-lib.cpp )


target_link_libraries( native-lib
                        ffmpeg-single
                        libyuv
                        android #这个应该是android的一些系统库
                       log )