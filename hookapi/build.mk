LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS) 
LOCAL_MODULE := libhookapi
LOCAL_C_INCLUDES:= \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/public

LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/src/hookapi.cpp 

LOCAL_CXXFLAGS:= -std=c++11 -fPIC
LOCAL_LDLIBS:= -lpthread -ldl
	#-Wl,-wrap,malloc,-wrap,realloc,-wrap,calloc,-wrap,free,-wrap,memset,-wrap,memcpy,-wrap,memmove
LOCAL_SHARED_LIBRARIES:= 
LOCAL_STATIC_LIBRARIES:= 
include $(BUILD_SHARED_LIBRARY)  



include $(CLEAR_VARS) 
LOCAL_MODULE := testlibhookapi
LOCAL_C_INCLUDES:= \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/public

LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/test/main.cpp

LOCAL_CXXFLAGS:= -include hookapi.h
LOCAL_LDLIBS:= -lpthread -ldl
LOCAL_SHARED_LIBRARIES:= 
LOCAL_STATIC_LIBRARIES:= 
include $(BUILD_EXECUTABLE)  
# run command:  
#
# make libhookapi testlibhookapi
#
# export LD_PRELOAD=out/shared/libhookapi.so
# ./out/bin/testlibhookapi
#
# export LD_PRELOAD=
#