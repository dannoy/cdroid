LOCAL_GEN_FILES := \
	ITProcess_aidl.cpp
LOCAL_SRC_FILES := \
	TProcessService.cpp \
	TResult.cpp \
	main_tprocess.cpp

LOCAL_CLIENT_SRC_FILES := \
	main_tprocess_client.cpp

progs := tprocess_service tprocess_client
tprocess_service_src = $(addprefix $(current_dir)/,$(LOCAL_SRC_FILES) $(LOCAL_GEN_FILES))
$(addprefix $(current_dir)/,$(LOCAL_SRC_FILES)) : $(current_dir)/ITProcess_aidl.cpp
$(current_dir)/ITProcess_aidl.cpp : bin/aidl
tprocess_service_dep = lib/liblog.so
local_cflags =
local_ldflags = -lutils -lbinder -llog -lcutils -lcorkscrew -lz -ldl -lpthread -lrt

#tprocess_client_src = $(addprefix $(current_dir)/,$(LOCAL_CLIENT_SRC_FILES) $(LOCAL_GEN_FILES))
tprocess_client_src = $(addprefix $(current_dir)/,$(LOCAL_CLIENT_SRC_FILES))
tprocess_client_dep = obj/$(current_dir)/ITProcess_aidl.o obj/$(current_dir)/TResult.o


include $(RULESMK)
