
LOCAL_SRC_FILES := \
	service_manager.c \
	binder.c

progs := servicemanager
servicemanager_src = $(addprefix $(current_dir)/,$(LOCAL_SRC_FILES))
servicemanager_dep = lib/liblog.so
local_cflags = 
local_ldflags = -llog


include $(RULESMK)
