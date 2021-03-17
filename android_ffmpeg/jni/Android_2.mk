LOCAL_PATH := $(call my-dir)




include $(CLEAR_VARS)

LOCAL_MODULE := h264decoder
LOCAL_SRC_FILES := h264decoder.c
TARGET_ARCH_ABI = armeabi

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_LDFLAGS := -L$(LOCAL_PATH)/lib

LOCAL_CFLAGS := -std=c99 -Wall -fvisibility=hidden

LOCAL_LDFLAGS := $(LOCAL_LDFLAGS) -Wl,--gc-sections
LOCAL_LDLIBS := -llog -lavformat -lavcodec -lswscale -lavutil

LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib  -ljnigraphics -lGLESv2

include $(BUILD_SHARED_LIBRARY)
