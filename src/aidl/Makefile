LOCAL_SRC_FILES := \
	aidl.cpp \
	aidl_language.cpp \
	options.cpp \
	search_path.cpp \
	AST.cpp \
	Type.cpp \
	generate_java.cpp \
	generate_java_binder.cpp \
	generate_java_rpc.cpp

progs := caidl
shared_libs :=
caidl_src = $(addprefix $(current_dir)/,$(LOCAL_SRC_FILES)) $(current_dir)/aidl_language_y.tab.cpp $(current_dir)/aidl_language_l.lex.cpp
$(current_dir)/aidl_language_l.lex.cpp:$(current_dir)/aidl_language_y.tab.cpp
caidl_dep =
local_cflags = -I$(TOPDIR)/include -I$(current_dir)
local_ldflags =


include $(RULESMK)
