
progs := cdroidruntime
cdroidruntime_src = $(wildcard $(current_dir)/*.cpp) $(wildcard $(current_dir)/*.c)
cdroidruntime_dep = lib/libutils.so lib/libbinder.so lib/liblog.so lib/libcutils.so lib/libcorkscrew.so
local_cflags =
local_ldflags = -lutils -lbinder -llog -lcutils -lcorkscrew -lz -ldl -lpthread -lrt


include $(RULESMK)
