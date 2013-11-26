LOCAL_SRC_FILES := \
	ITProcess_aidl.cpp \
	TProcessService.cpp \
	main_tprocess.cpp

progs := tprocess_service
tprocess_service_src = $(addprefix $(current_dir)/,$(LOCAL_SRC_FILES))
tprocess_service_dep = $(current_dir)/ITProcess_aidl.cpp bin/aidl lib/liblog.so
local_cflags =
local_ldflags = -llog


include $(RULESMK)
