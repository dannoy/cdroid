LDEBUG = y

shared_libs = libbinder
libbinder_src = $(wildcard $(current_dir)/*.cpp)
local_cflags = -I$(TOPDIR)/include
local_ldflags =

ifeq ($(LDEBUG), y)
    local_cflags +=  -g
endif



include $(RULESMK)
