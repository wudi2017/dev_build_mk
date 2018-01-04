LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS) 
LOCAL_MODULE := wintest
LOCAL_C_INCLUDES:= \
	$(LOCAL_PATH) \
	/usr/X11R6/include \
	/usr/include
LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/wintest.cpp
LOCAL_CXXFLAGS:= -std=c++11 
LOCAL_LDFLAGS:= -L/usr/X11R6/lib
LOCAL_LDLIBS:= -lpthread -lX11
LOCAL_SHARED_LIBRARIES:= 
LOCAL_STATIC_LIBRARIES:= 
include $(BUILD_EXECUTABLE)  
