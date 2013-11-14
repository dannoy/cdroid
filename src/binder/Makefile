LDEBUG = y

progs :=
shared_libs := libbinder
libbinder_src = $(wildcard $(current_dir)/*.cpp) $(wildcard $(current_dir)/*.c)
libbinder_dep = lib/libcutils.so lib/libutils.so
local_cflags = -I$(TOPDIR)/include
local_ldflags =

ifeq ($(LDEBUG), y)
    local_cflags +=  -g
endif



include $(RULESMK)
