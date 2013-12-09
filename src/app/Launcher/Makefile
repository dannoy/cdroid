shared_libs := launcher
launcher_src = $(wildcard $(current_dir)/*.cpp) $(wildcard $(current_dir)/*.c)
launcher_dep = lib/libcutils.so lib/libutils.so
local_cflags = -I$(topdir)/include
local_ldflags =


include $(RULESMK)
