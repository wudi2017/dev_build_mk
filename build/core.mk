#$(info GLOBAL_ROOT=$(GLOBAL_ROOT))

#------------------------------------------------------------------------------------
#
# you could change 
#
#------------------------------------------------------------------------------------

# param
#$(info VERBOSE=$(VERBOSE))

# config compiler

CC ?= gcc
CXX ?= g++
AR ?= ar

# config global vars

GLOBAL_C_INCLUDES := 
GLOBAL_CXXFLAGS := 
GLOBAL_LDFLAGS := 
GLOBAL_LDLIBS := 


#------------------------------------------------------------------------------------
#
# DO NOT CHANGE
#
#------------------------------------------------------------------------------------

ifeq ($(VERBOSE),1)
hide := 
else
hide := @
endif

# all target
.PHONY: all
all: 

# output dir

GLOBAL_OUTPUT := $(GLOBAL_ROOT)/out
GLOBAL_OUTPUT_BIN := $(GLOBAL_OUTPUT)/bin
GLOBAL_OUTPUT_SHARED := $(GLOBAL_OUTPUT)/shared
GLOBAL_OUTPUT_STATIC := $(GLOBAL_OUTPUT)/static
GLOBAL_OUTPUT_OBJ := $(GLOBAL_OUTPUT)/obj

# function mk

CLEAR_VARS := $(GLOBAL_ROOT)/build/clear_vars.mk
BUILD_STATIC_LIBRARY := $(GLOBAL_ROOT)/build/build_static_library.mk
BUILD_SHARED_LIBRARY := $(GLOBAL_ROOT)/build/build_shared_library.mk
BUILD_EXECUTABLE := $(GLOBAL_ROOT)/build/build_executable.mk


define my-dir
$(strip $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST)))))
endef

# clear & define local vars

include $(CLEAR_VARS) 

# include all
 
DIRS = $(shell find $(GLOBAL_ROOT) -maxdepth 10 -type d)  
BUILD_MAKEFILES := $(foreach dir, $(DIRS), $(wildcard $(dir)/build.mk))  
$(foreach mkfile, $(BUILD_MAKEFILES), $(info include $(mkfile)))

include $(BUILD_MAKEFILES)

.PHONY: clean
clean:
	@-rm -rf $(GLOBAL_OUTPUT)

