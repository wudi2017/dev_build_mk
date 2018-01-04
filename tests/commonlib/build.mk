LOCAL_PATH := $(call my-dir)

#---------------------------------------------------------------------------------
# static libadd libsub
# shared libmathas

include $(CLEAR_VARS) 
LOCAL_MODULE := libmathas
LOCAL_C_INCLUDES:= $(GLOBAL_ROOT)/tests/commonlib\
	$(LOCAL_PATH)/libmath_add_sub/include \
	$(LOCAL_PATH)/add/include \
	$(LOCAL_PATH)/sub/include
LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/libmath_add_sub/addsub.cpp
LOCAL_STATIC_LIBRARIES:= libadd libsub
LOCAL_CXXFLAGS:=-fPIC
include $(BUILD_SHARED_LIBRARY)  

include $(CLEAR_VARS) 
LOCAL_MODULE := libadd
LOCAL_C_INCLUDES:= $(GLOBAL_ROOT)/tests/commonlib\
	$(LOCAL_PATH)/add/include
LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/add/add.cpp \
	$(LOCAL_PATH)/add/impl2/add_impl2.cpp
include $(BUILD_STATIC_LIBRARY)  

include $(CLEAR_VARS) 
LOCAL_MODULE := libsub
LOCAL_C_INCLUDES:= $(GLOBAL_ROOT)/tests/commonlib\
	$(LOCAL_PATH)/sub/include
LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/sub/sub.cpp
include $(BUILD_STATIC_LIBRARY)  


#---------------------------------------------------------------------------------
# static libmul libdiv

include $(CLEAR_VARS) 
LOCAL_MODULE := libmul
LOCAL_C_INCLUDES:= $(GLOBAL_ROOT)/tests/commonlib\
	$(LOCAL_PATH)/mul/include
LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/mul/mul.cpp
include $(BUILD_STATIC_LIBRARY)  

include $(CLEAR_VARS) 
LOCAL_MODULE := libdiv
LOCAL_C_INCLUDES:= $(GLOBAL_ROOT)/tests/commonlib\
	$(LOCAL_PATH)/div/include
LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/div/div.cpp
include $(BUILD_STATIC_LIBRARY)  


