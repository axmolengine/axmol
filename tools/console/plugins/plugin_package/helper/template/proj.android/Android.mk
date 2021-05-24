
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := __PACKAGE_NAME___static
LOCAL_MODULE_FILENAME := lib__PACKAGE_NAME__

LOCAL_SRC_FILES := \
    $(LOCAL_PATH)/../package___PACKAGE_NAME___register.cpp

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/.. 

LOCAL_C_INCLUDES := $(LOCAL_EXPORT_C_INCLUDES) \
                    $(LOCAL_PATH)/../../../frameworks/cocos2d-x/cocos \
                    $(LOCAL_PATH)/../../../frameworks/cocos2d-x/external

include $(BUILD_STATIC_LIBRARY)
