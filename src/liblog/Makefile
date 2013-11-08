LDEBUG = y

shared_libs = liblog
#libutils_src = $(wildcard $(current_dir)/*.cpp) $(current_dir)/aaaaaaaaaaa.c
liblog_src = $(wildcard $(current_dir)/*.cpp) $(wildcard $(current_dir)/*.c)
local_cflags = -I$(TOPDIR)/include
local_ldflags =

ifeq ($(LDEBUG), y)
    local_cflags +=  -g
endif



include $(RULESMK)
