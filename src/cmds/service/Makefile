LOCAL_SRC_FILES := \
	service.cpp

progs := service
service_src = $(addprefix $(current_dir)/,$(LOCAL_SRC_FILES))
service_dep = lib/libutils.so lib/libbinder.so lib/liblog.so lib/libcutils.so lib/libcorkscrew.so
local_cflags =
local_ldflags = -lutils -lbinder -llog -lcutils -lcorkscrew -lz -ldl -lpthread -lrt


include $(RULESMK)
