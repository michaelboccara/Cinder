LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE     := TextureFont

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../src
LOCAL_SRC_FILES := ../../src/TextureFontApp.cpp

LOCAL_LDLIBS    := -landroid 
LOCAL_STATIC_LIBRARIES := cinder-es2 boost_system boost_filesystem freeimage ft2 android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
$(call import-module,cinder-es2)
$(call import-module,boost)

