LOCAL_SRC_FILES := \
	logcat.cpp

progs := logcat
logcat_src = $(addprefix $(current_dir)/,$(LOCAL_SRC_FILES))
logcat_dep = lib/liblog.so
local_cflags = 
local_ldflags = -llog


include $(RULESMK)
