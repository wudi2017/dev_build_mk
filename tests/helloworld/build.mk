LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS) 
LOCAL_MODULE := helloworld1
LOCAL_C_INCLUDES:= \
	$(GLOBAL_ROOT)/tests/commonlib/libmath_add_sub/include \
	$(GLOBAL_ROOT)/tests/commonlib/mul/include \
	$(GLOBAL_ROOT)/tests/commonlib/div/include
LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/helloworld.cpp
LOCAL_SHARED_LIBRARIES:= libmathas
LOCAL_STATIC_LIBRARIES:= libmul libdiv
include $(BUILD_EXECUTABLE)  


include $(CLEAR_VARS) 
LOCAL_MODULE := helloworld2
LOCAL_C_INCLUDES:= $(GLOBAL_ROOT)/tests/commonlib\
	$(GLOBAL_ROOT)/tests/commonlib/libmath_add_sub/include \
	$(GLOBAL_ROOT)/tests/commonlib/mul/include \
	$(GLOBAL_ROOT)/tests/commonlib/div/include
LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/helloworld.cpp \
	$(LOCAL_PATH)/submodule/test.cpp \
	$(LOCAL_PATH)/test.cpp 
LOCAL_SHARED_LIBRARIES:= libmathas
LOCAL_STATIC_LIBRARIES:= libmul libdiv
include $(BUILD_EXECUTABLE)  