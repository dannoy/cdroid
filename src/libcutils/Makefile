
progs :=
shared_libs := libcutils
libcutils_src = $(wildcard $(current_dir)/*.cpp) $(wildcard $(current_dir)/*.c)
libcutils_dep = lib/liblog.so
local_cflags = -I$(TOPDIR)/include -DANDROID_SMP -D$(ARCHD)
local_ldflags =

include $(RULESMK)
