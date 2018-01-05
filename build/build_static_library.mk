#$(info LOCAL_MODULE=$(LOCAL_MODULE))
#$(info LOCAL_SRC_FILES=$(LOCAL_SRC_FILES))
#$(info LOCAL_C_INCLUDES=$(LOCAL_C_INCLUDES))
#$(info LOCAL_SHARED_LIBRARIES=$(LOCAL_SHARED_LIBRARIES))
#$(info LOCAL_STATIC_LIBRARIES=$(LOCAL_STATIC_LIBRARIES))

OBJS := $(patsubst %cpp,%o,$(LOCAL_SRC_FILES))
OBJS := $(patsubst $(LOCAL_PATH)/%,$(GLOBAL_OUTPUT_OBJ)/$(LOCAL_MODULE)/%,$(OBJS))
#$(info OBJS=$(OBJS))


all: $(LOCAL_MODULE)

.PHONY: $(LOCAL_MODULE)
$(LOCAL_MODULE): $(GLOBAL_OUTPUT_STATIC)/$(LOCAL_MODULE).a

# define INNER VARS
$(GLOBAL_OUTPUT_STATIC)/$(LOCAL_MODULE).a: INNER_MODULE := $(LOCAL_MODULE)
$(GLOBAL_OUTPUT_STATIC)/$(LOCAL_MODULE).a: INNER_LDFLAGS := -L$(GLOBAL_OUTPUT_SHARED) -L$(GLOBAL_OUTPUT_STATIC) $(LOCAL_LDFLAGS)
$(GLOBAL_OUTPUT_STATIC)/$(LOCAL_MODULE).a: INNER_LDLIBS := $(patsubst lib%,-l%,$(LOCAL_SHARED_LIBRARIES)) $(patsubst lib%,-l%,$(LOCAL_STATIC_LIBRARIES)) $(LOCAL_LDLIBS)
$(GLOBAL_OUTPUT_STATIC)/$(LOCAL_MODULE).a: INNER_OBJS := $(OBJS)
# add dependences for shared libs
$(GLOBAL_OUTPUT_STATIC)/$(LOCAL_MODULE).a: $(addprefix $(GLOBAL_OUTPUT_SHARED)/, $(addsuffix .so,$(LOCAL_SHARED_LIBRARIES)))
# add dependences for static libs
$(GLOBAL_OUTPUT_STATIC)/$(LOCAL_MODULE).a: $(addprefix $(GLOBAL_OUTPUT_STATIC)/, $(addsuffix .a,$(LOCAL_STATIC_LIBRARIES)))
# add dependences for objs
$(GLOBAL_OUTPUT_STATIC)/$(LOCAL_MODULE).a: $(OBJS)
	@echo [$(INNER_MODULE)] INSTALL $@
	@-mkdir -p $(GLOBAL_OUTPUT_STATIC)
	$(hide)$(AR) cr $@ $(INNER_OBJS)

#$(OBJS): OBJSDIR:=$(foreach objfile, $(OBJS), $(dir $(objfile)))
$(OBJS): INNER_MODULE:=$(LOCAL_MODULE)
$(OBJS): INNER_INC_FLAGS:=$(foreach incdir, $(LOCAL_C_INCLUDES), -I$(incdir))
$(OBJS): GLOBAL_INC_FLAGS:=$(foreach incdir, $(GLOBAL_C_INCLUDES), -I$(incdir))
$(OBJS): INNER_CXXFLAGS:= $(LOCAL_CXXFLAGS)
$(OBJS): $(GLOBAL_OUTPUT_OBJ)/$(LOCAL_MODULE)/%.o: $(LOCAL_PATH)/%.cpp
	@echo [$(INNER_MODULE)] CXX $@ '<=' $<
	@-mkdir -p $(dir $@)
	$(hide)$(CXX) $(GLOBAL_CXXFLAG) $(INNER_CXXFLAGS) $(GLOBAL_INC_FLAGS) $(INNER_INC_FLAGS) -MMD -MT $@ -MF $@.d -c $< -o $@

# -MMD -MT $@ -MF $@.d
# generate include,cpp dependences
# add include dependences
DEPENDS := $(addsuffix .d,$(OBJS))
-include $(DEPENDS)