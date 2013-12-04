progs := app_process
app_process_src = $(wildcard $(current_dir)/*.cpp) $(wildcard $(current_dir)/*.c)
app_process_dep = lib/libutils.so lib/libbinder.so lib/liblog.so lib/libcutils.so lib/libcorkscrew.so
local_cflags =
local_ldflags = -lutils -lbinder -llog -lcutils -lcorkscrew -lz -ldl -lpthread -lrt


include $(RULESMK)
