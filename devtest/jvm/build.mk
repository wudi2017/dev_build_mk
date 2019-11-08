LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS) 
LOCAL_MODULE := testjvm
LOCAL_C_INCLUDES:= \
	$(LOCAL_PATH) \
	/usr/lib/jvm/java-8-openjdk-amd64/include \
	/usr/lib/jvm/java-8-openjdk-amd64/include/linux

LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/main.cpp 
LOCAL_CXXFLAGS:= -std=c++11 
LOCAL_LDLIBS:= -lpthread -rdynamic -g -L/usr/lib/jvm/java-8-openjdk-amd64/jre/lib/amd64/server -ljvm
LOCAL_SHARED_LIBRARIES:= 
LOCAL_STATIC_LIBRARIES:= 
include $(BUILD_EXECUTABLE)  
