LDEBUG = y

shared_libs = libcutils
libcutils_src = $(wildcard $(current_dir)/*.cpp) $(wildcard $(current_dir)/*.c)
libcutils_dep = lib/liblog.so
local_cflags = -I$(TOPDIR)/include
local_ldflags =

ifeq ($(LDEBUG), y)
    local_cflags +=  -g
endif



include $(RULESMK)
