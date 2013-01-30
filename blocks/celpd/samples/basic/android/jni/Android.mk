LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE     := BasicAudio

CELAUDIO_SRC = ../../../../src

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../src $(LOCAL_PATH)/../../../../src $(LIBPD_PATH)/pure-data/src
LOCAL_SRC_FILES := ../../src/basicAudioApp.cpp \
	              $(CELAUDIO_SRC)/celpd.cpp \
				  $(CELAUDIO_SRC)/libpd_dl.c \
				  $(CELAUDIO_SRC)/opensl.cpp

LOCAL_LDFLAGS := -Wl,--as-needed
LOCAL_LDLIBS  := -landroid -lOpenSLES
LOCAL_STATIC_LIBRARIES := cinder boost_system boost_filesystem boost_thread freeimage ft2 android_native_app_glue 

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
$(call import-module,cinder)
$(call import-module,boost)
$(call import-module,libpd)

