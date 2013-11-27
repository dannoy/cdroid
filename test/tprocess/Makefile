LOCAL_GEN_FILES := \
	ITProcess_aidl.cpp
LOCAL_SRC_FILES := \
	TProcessService.cpp \
	TResult.cpp \
	main_tprocess.cpp

progs := tprocess_service
tprocess_service_src = $(addprefix $(current_dir)/,$(LOCAL_SRC_FILES) $(LOCAL_GEN_FILES))
$(addprefix $(current_dir)/,$(LOCAL_SRC_FILES)) : $(current_dir)/ITProcess_aidl.cpp
$(current_dir)/ITProcess_aidl.cpp : bin/aidl
tprocess_service_dep = lib/liblog.so
local_cflags =
local_ldflags = -lutils -lbinder -llog -lcutils -lcorkscrew -lz -ldl -lpthread -lrt


include $(RULESMK)
