LOCAL_SRC_FILES := \
	aidl.cpp \
	aidl_language.cpp \
	aidl_common.cpp \
	options.cpp \
	AST.cpp \
    CAST.cpp \
	Type.cpp \
    CType.cpp \
	generate_java.cpp \
	generate_java_binder.cpp \
	generate_java_rpc.cpp \
	generate_cpp.cpp \
	generate_cpp_binder.cpp \
    caidl.cpp \
    jaidl.cpp

progs := aidl
shared_libs :=
aidl_src = $(addprefix $(current_dir)/,$(LOCAL_SRC_FILES)) $(current_dir)/aidl_language_y.tab.cpp $(current_dir)/aidl_language_l.lex.cpp
$(current_dir)/aidl_language_l.lex.cpp:$(current_dir)/aidl_language_y.tab.cpp
aidl_dep =
local_cflags = -I$(TOPDIR)/include -I$(current_dir)
local_ldflags =


include $(RULESMK)
