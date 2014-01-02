progs := benchmark
benchmark_src = $(wildcard $(current_dir)/*.cpp) $(wildcard $(current_dir)/*.c)
benchmark_dep = lib/liblog.so
local_cflags =
local_ldflags = -lutils -lbinder -llog -lcutils -lcorkscrew -lz -ldl -lpthread -lrt

include $(RULESMK)
