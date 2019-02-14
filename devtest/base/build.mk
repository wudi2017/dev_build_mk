LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS) 
LOCAL_MODULE := basetest
LOCAL_C_INCLUDES:= \
	$(LOCAL_PATH) 
LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/main.cpp \
	$(LOCAL_PATH)/baselib.cpp \
	$(LOCAL_PATH)/testcase.cpp \
	$(LOCAL_PATH)/classtest.cpp \
	$(LOCAL_PATH)/demo_test.cpp 
LOCAL_CXXFLAGS:= -std=c++11 
LOCAL_LDLIBS:= -lpthread -rdynamic -g
LOCAL_SHARED_LIBRARIES:= 
LOCAL_STATIC_LIBRARIES:= 
include $(BUILD_EXECUTABLE)  
