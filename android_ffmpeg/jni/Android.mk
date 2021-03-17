LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := ffmpeg
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/$(TARGET_ARCH_ABI)/libijkffmpeg.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/ffmpeg
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := h264decoderhome
LOCAL_SRC_FILES := app_h264decoder.c  app_mp4muxer.c app_mp4read.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include/$(TARGET_ARCH_ABI)
#LOCAL_LDFLAGS := -L$(LOCAL_PATH)/lib/$(TARGET_ARCH_ABI)
LOCAL_SHARED_LIBRARIES := ffmpeg
LOCAL_CFLAGS := -std=c99 -Wall -fvisibility=hidden
LOCAL_LDFLAGS := $(LOCAL_LDFLAGS) -Wl,--gc-sections
#LOCAL_LDLIBS := -llog -lavformat -lavcodec -lswscale -lavutil -lswresample -lm -lz
LOCAL_LDLIBS := -llog -lm -lz

LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib  -ljnigraphics -lGLESv2

include $(BUILD_SHARED_LIBRARY)