shared_libs := AppThree
AppThree_src = $(wildcard $(current_dir)/*.cpp) $(wildcard $(current_dir)/*.c)
AppThree_dep = lib/libcutils.so lib/libutils.so
local_cflags = -I$(topdir)/include
local_ldflags = -lcdroid_server -lcdroid_runtime


include $(RULESMK)
