LOCAL_PATH := $(my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libbadass

LOCAL_SRC_FILES := LOCK_Sequence.cpp \
                                   MapThread.cpp \
                                   LOCK_SyncObj.cpp \
                                   MapFile.cpp \

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := liblog

LOCAL_LDLIBS := -lpthread -lrt -ldl


LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT)/lib/

include $(BUILD_SHARED_LIBRARY)

