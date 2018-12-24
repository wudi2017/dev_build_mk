LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS) 
LOCAL_MODULE := libmemorytraker
LOCAL_C_INCLUDES:= \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/public

LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/src/MTNew.cpp \
	$(LOCAL_PATH)/src/MTMemoryPrivate.cpp \
	$(LOCAL_PATH)/src/MTMemoryAllocatePolicy.cpp \
	$(LOCAL_PATH)/src/MTProtectMemoryAllocator.cpp \
	$(LOCAL_PATH)/src/MTThreadLock.cpp \
	$(LOCAL_PATH)/src/MTRecorder.cpp \
	$(LOCAL_PATH)/src/MTLog.cpp \
	$(LOCAL_PATH)/src/MTMonitor.cpp

LOCAL_CXXFLAGS:= -std=c++11 -fPIC
LOCAL_LDLIBS:= -lpthread
LOCAL_SHARED_LIBRARIES:= 
LOCAL_STATIC_LIBRARIES:= 
include $(BUILD_SHARED_LIBRARY)  



include $(CLEAR_VARS) 
LOCAL_MODULE := testlibmemorytraker
LOCAL_C_INCLUDES:= \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/public

LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/test/main.cpp

LOCAL_CXXFLAGS:= -include MTNew.h 
LOCAL_LDLIBS:= -lpthread
LOCAL_SHARED_LIBRARIES:= 
LOCAL_STATIC_LIBRARIES:= 
include $(BUILD_EXECUTABLE)  
# run command:  
#
# make libmemorytraker testlibmemorytraker
#
# export LD_PRELOAD=out/shared/libmemorytraker.so
# ./out/bin/test-memorytracker
#
# export LD_PRELOAD=
#