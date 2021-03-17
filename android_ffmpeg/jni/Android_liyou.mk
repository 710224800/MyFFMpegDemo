LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := ffmpeg
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/libffmpeg.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE :=videoDecoder
LOCAL_SRC_FILES := h264decoder.c h265decoder.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_CFLAGS := -std=c99 -Wall -fvisibility=hidden
LOCAL_LDLIBS += -llog -lz -landroid -lGLESv2
LOCAL_SHARED_LIBRARIES:= ffmpeg
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := mp4muxer.c
LOCAL_LDLIBS += -llog -lz -landroid
LOCAL_MODULE :=mp4muxer
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_C_FLAGS += -mfpu=neon -mfloat-abi=softfp -fPIC -fstack-protector
LOCAL_SHARED_LIBRARIES:= ffmpeg
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_SRC_FILES := mp4read.c
LOCAL_LDLIBS += -llog -lz -landroid
LOCAL_MODULE :=mp4read
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_C_FLAGS += -mfpu=neon -mfloat-abi=softfp -fPIC -fstack-protector
LOCAL_SHARED_LIBRARIES:= ffmpeg
include $(BUILD_SHARED_LIBRARY)