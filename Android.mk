LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libren
LOCAL_CFLAGS += $(GLOBAL_CFLAGS)
LOCAL_C_INCLUDES += $(GLOBAL_C_INCLUDES)
LOCAL_SRC_FILES :=  _ren.cpp

include $(BUILD_STATIC_LIBRARY)
