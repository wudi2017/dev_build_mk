
# find all lev1 dir
$(info ----------------------------------------)
curdir = $(shell pwd)
dirs := $(shell find $(curdir) -maxdepth 1 -type d)
$(info dirs = $(dirs))

# nodirs
$(info ----------------------------------------)
nodirs = $(notdir $(dirs))
$(info nodirs = $(nodirs))

# wildcard
$(info ----------------------------------------)
wfiles = $(wildcard src/*.cpp output/*.o) 
$(info wfiles = $(wfiles))

# patsubst
$(info ----------------------------------------)
OBJ = $(patsubst %.c %.o $(wfiles)) 

# basename
$(info ----------------------------------------)
basename = $(basename xxx.g) 
$(info basename = $(basename))

# filter-out 
$(info ----------------------------------------)
filterlist = $(filter-out x%.c, x1.c x23.c y1.c) 
$(info filterlist = $(filterlist))

# foreach wildcard
$(info ----------------------------------------)
# find all src in sub dirs
PROJROOT := .
SRC_IDRS := \
	src
SRC_FILE_LIST := $(foreach n,$(SRC_IDRS) , $(wildcard $(PROJROOT)/$(n)/*.cpp)) 
$(info SRC_FILE_LIST = $(SRC_FILE_LIST))

# build
$(info ----------------------------------------)
$(info build...)

OUTPUT := ./output

.PHONY:all
all: mko $(OUTPUT)/maintest $(OUTPUT)/testcompiler $(OUTPUT)/wintest

.PHONY:mko
mko:
	$(if $(wildcard $(OUTPUT)),,mkdir -p $(OUTPUT))
	@echo hahaha 

.PHONY : clean
clean:
	rm -rf $(OUTPUT)

$(OUTPUT)/maintest:$(OUTPUT)/main.o $(OUTPUT)/testcase.o $(OUTPUT)/baselib.o 
	@g++ -o $(OUTPUT)/maintest $(OUTPUT)/main.o $(OUTPUT)/testcase.o $(OUTPUT)/demo_test.o $(OUTPUT)/baselib.o -lpthread -std=c++11

$(OUTPUT)/main.o:$(OUTPUT)/testcase.o $(OUTPUT)/demo_test.o $(OUTPUT)/baselib.o ./src/main.cpp 
	@g++ -c ./src/main.cpp -o $(OUTPUT)/main.o -std=c++11

$(OUTPUT)/demo_test.o:./src/demo_test.cpp
	@g++ -c ./src/demo_test.cpp -o $(OUTPUT)/demo_test.o -I/usr/include -std=c++11

$(OUTPUT)/testcase.o:$(OUTPUT)/baselib.o ./src/testcase.cpp ./src/testcase.h
	@g++ -c ./src/testcase.cpp -o $(OUTPUT)/testcase.o -std=c++11

$(OUTPUT)/baselib.o:./src/baselib.cpp ./src/baselib.h
	@g++ -c ./src/baselib.cpp -o $(OUTPUT)/baselib.o -std=c++11

$(OUTPUT)/wintest:./src/wintest.cpp
	@g++ -c ./src/wintest.cpp -o $(OUTPUT)/wintest.o -I/usr/X11R6/include -I/usr/include -L/usr/X11R6/lib -lX11
	@g++ $(OUTPUT)/wintest.o -o $(OUTPUT)/wintest -lX11

$(OUTPUT)/testcompiler:./src/testcompiler.cpp
	@ccache /usr/bin/clang++-3.8 ./src/testcompiler.cpp -o $(OUTPUT)/testcompiler -DBOOST_ATOMIC_NO_LIB -DBOOST_ERROR_CODE_HEADER_ONLY -DBOOST_SYSTEM_NO_DEPRECATED -DCOMP_OPT_LITTLE_ENDIAN -DCOMP_OPT_USE_CURL -D_GLIBCXX_PERMIT_BACKWARD_HASH -D_LINUX -D_LINUX -g -DHAVE_LINUX_NATIVEX86 -D__USES_INITFINI__ -Dlibnxeventsys_EXPORTS  -frtti -fexceptions -std=gnu++11 -m64  -g -fPIC -fvisibility=hidden  -fsigned-char  -fexceptions -std=c++11

