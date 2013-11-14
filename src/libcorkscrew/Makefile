generic_src_files := \
	backtrace.c \
	backtrace-helper.c \
	demangle.c \
	map_info.c \
	ptrace.c \
	symbol_table.c
arm_src_files := \
	arch-arm/backtrace-arm.c \
	arch-arm/ptrace-arm.c

x86_src_files := \
	arch-x86/backtrace-x86.c \
	arch-x86/ptrace-x86.c
LOCAL_SRC_FILES := $(generic_src_files)
ifeq ($(ARCH),arm)
LOCAL_SRC_FILES += $(arm_src_files)
LOCAL_CFLAGS += -DCORKSCREW_HAVE_ARCH
endif
ifeq ($(ARCH),x86)
LOCAL_SRC_FILES += $(x86_src_files)
LOCAL_CFLAGS += -DCORKSCREW_HAVE_ARCH
endif
ifeq ($(ARCH),mips)
LOCAL_SRC_FILES += \
	arch-mips/backtrace-mips.c \
	arch-mips/ptrace-mips.c
LOCAL_CFLAGS += -DCORKSCREW_HAVE_ARCH
endif

progs :=
shared_libs := libcorkscrew
libcorkscrew_src = $(addprefix $(current_dir)/,$(LOCAL_SRC_FILES))
libcorkscrew_dep = lib/liblog.so lib/libcutils.so
local_cflags = -I$(TOPDIR)/include -DANDROID_SMP -D$(ARCHD) -D_GNU_SOURCE $(LOCAL_CFLAGS) -std=gnu99
local_ldflags =


include $(RULESMK)
