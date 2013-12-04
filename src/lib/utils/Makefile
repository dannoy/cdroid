progs :=
shared_libs := libutils
libutils_src = $(wildcard $(current_dir)/*.cpp) $(wildcard $(current_dir)/*.c)
libutils_dep = lib/libcutils.so
local_cflags = -I$(TOPDIR)/include
local_ldflags =

include $(RULESMK)
