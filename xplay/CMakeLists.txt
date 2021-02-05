cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_VERBOSE_MAKEFILE on) #显示详细的原始编译信息，主要用于定位一些链接错误，看看库路径什么的是否配置对。

#获取上级目录
get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} PATH)
#使用单个库版本
set(LIBFFMPEG_DIR ${PARENT_DIR}/myffmpeglibrary)
#使用单个yuv库
set(LIBYUV_DIR ${PARENT_DIR}/libyuv-single)


add_library(avcodec
        SHARED
        IMPORTED)
set_target_properties(avcodec
        PROPERTIES IMPORTED_LOCATION
        ${LIBFFMPEG_DIR}/libs/${ANDROID_ABI}/libavcodec.so
        )

add_library(avfilter
        SHARED
        IMPORTED)
set_target_properties(avfilter
        PROPERTIES IMPORTED_LOCATION
        ${LIBFFMPEG_DIR}/libs/${ANDROID_ABI}/libavfilter.so
        )

add_library(avformat
        SHARED
        IMPORTED)
set_target_properties(avformat
        PROPERTIES IMPORTED_LOCATION
        ${LIBFFMPEG_DIR}/libs/${ANDROID_ABI}/libavformat.so
        )

add_library(avutil
        SHARED
        IMPORTED)
set_target_properties(avutil
        PROPERTIES IMPORTED_LOCATION
        ${LIBFFMPEG_DIR}/libs/${ANDROID_ABI}/libavutil.so
        )

add_library(swscale
        SHARED
        IMPORTED)
set_target_properties(swscale
        PROPERTIES IMPORTED_LOCATION
        ${LIBFFMPEG_DIR}/libs/${ANDROID_ABI}/libswscale.so
        )

add_library(swresample
        SHARED
        IMPORTED)
set_target_properties(swresample
        PROPERTIES IMPORTED_LOCATION
        ${LIBFFMPEG_DIR}/libs/${ANDROID_ABI}/libswresample.so
        )

#add_library(libyuv
#        SHARED
#        IMPORTED)
#set_target_properties(libyuv
#        PROPERTIES IMPORTED_LOCATION
#        ${LIBYUV_DIR}/libs/${ANDROID_ABI}/libyuv.so
#        )

# libyuv库头文件
include_directories(${LIBYUV_DIR}/libs/${ANDROID_ABI}/include)

# myffmpeglibrary库头文件
include_directories(${LIBFFMPEG_DIR}/libs/${ANDROID_ABI}/include)

add_library( native-lib
             SHARED
        src/main/cpp/native-lib.cpp
        src/main/cpp/IDemux.cpp
        src/main/cpp/FFDemux.cpp
        src/main/cpp/XData.cpp
        src/main/cpp/XLog.cpp
        src/main/cpp/XThread.cpp
        src/main/cpp/IObserver.cpp
        src/main/cpp/XParameter.cpp
        src/main/cpp/IDecode.cpp
        src/main/cpp/FFDecode.cpp
        src/main/cpp/XEGL.cpp
        src/main/cpp/GLVideoView.cpp
        src/main/cpp/IVideoView.cpp
        src/main/cpp/XShader.cpp
        src/main/cpp/XTexture.cpp
        )


target_link_libraries( native-lib
                       avcodec avfilter avformat avutil swscale swresample
#                       libyuv
                       OpenSLES
                       GLESv2
                       EGL
                       android #这个应该是android的一些系统库
                       log )