LDEBUG = y

export TOPDIR := $(CURDIR)
export RULESMK := $(CURDIR)/rules.mk
export ARCH := x86
# __i386__, __x86_64__, __arm__, __mips__
export ARCHD := __i386__


include $(RULESMK)
$(eval $(call preCalled))
#$(eval $(call includeSubDir,src/liblog))
#$(eval $(call includeSubDir,src/libcutils))
#$(eval $(call includeSubDir,src/libcorkscrew))
#$(eval $(call includeSubDir,src/utils))
#$(eval $(call includeSubDir,src/binder))

$(eval $(call includeSubDir,src/aidl))
#$(eval $(call includeSubDir,src/cmds/logcat))
#$(eval $(call includeSubDir,src/cmds/servicemanager))
#$(eval $(call includeSubDir,src/cmds/service))
$(eval $(call includeSubDir,test/tprocess))

