LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := mijia_ffmpeg
LOCAL_SRC_FILES := h264decoder.c h265decoder.c  mp4muxer.c mp4read.c
TARGET_ARCH_ABI = armeabi

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_LDFLAGS := -L$(LOCAL_PATH)/lib/$(APP_ABI)

LOCAL_CFLAGS := -std=c99 -Wall -fvisibility=hidden

LOCAL_LDFLAGS := $(LOCAL_LDFLAGS) -Wl,--gc-sections
LOCAL_LDLIBS := -llog -lavformat -lavcodec -lswscale -lavutil -lswresample -lm -lz

LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib  -ljnigraphics -lGLESv2

include $(BUILD_SHARED_LIBRARY)