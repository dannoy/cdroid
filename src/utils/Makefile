LDEBUG = y

shared_libs = libutils
#libutils_src = $(wildcard $(current_dir)/*.cpp) $(current_dir)/aaaaaaaaaaa.c
libutils_src = $(wildcard $(current_dir)/*.cpp)
local_cflags = -I$(TOPDIR)/include
local_ldflags =

ifeq ($(LDEBUG), y)
    local_cflags +=  -g
endif



include $(RULESMK)
