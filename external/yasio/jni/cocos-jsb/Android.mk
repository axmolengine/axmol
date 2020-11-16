LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := yasio_static

LOCAL_MODULE_FILENAME := libyasio

LOCAL_CPPFLAGS := -std=c++11 -pthread -frtti -fexceptions

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../ $(LOCAL_PATH)/../../../spidermonkey/include/android $(LOCAL_PATH)/../../../../cocos

LOCAL_SRC_FILES := ../../bindings/yasio_jsb.cpp

include $(BUILD_STATIC_LIBRARY)
