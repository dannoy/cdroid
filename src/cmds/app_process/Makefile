progs := application_process
application_process_src = $(wildcard $(current_dir)/*.cpp) $(wildcard $(current_dir)/*.c)
application_process_dep = lib/libutils.so lib/libbinder.so lib/liblog.so lib/libcutils.so lib/libcorkscrew.so lib/libcdroid_server.so lib/libcdroid_runtime.so
local_cflags =
local_ldflags = -lcdroid_server -lcdroid_runtime -lutils -lbinder -llog -lcutils -lcorkscrew -lz -ldl -lpthread -lrt


include $(RULESMK)
