LDEBUG = y

export TOPDIR := $(CURDIR)
export RULESMK := $(CURDIR)/rules.mk
export ARCH := x86
# __i386__, __x86_64__, __arm__, __mips__
export ARCHD := __i386__


include $(RULESMK)
$(eval $(call preCalled))
$(eval $(call includeSubDir,src/lib/liblog))
$(eval $(call includeSubDir,src/lib/libcutils))
$(eval $(call includeSubDir,src/lib/libcorkscrew))
$(eval $(call includeSubDir,src/lib/utils))
$(eval $(call includeSubDir,src/lib/binder))

$(eval $(call includeSubDir,src/tools/aidl))

$(eval $(call includeSubDir,src/runtime))
$(eval $(call includeSubDir,src/service))

$(eval $(call includeSubDir,src/cmds/logcat))
$(eval $(call includeSubDir,src/cmds/servicemanager))
$(eval $(call includeSubDir,src/cmds/service))
$(eval $(call includeSubDir,src/cmds/cdroidruntime))
$(eval $(call includeSubDir,src/cmds/app_process))

$(eval $(call includeSubDir,src/app/Launcher))
$(eval $(call includeSubDir,src/app/AppOne))
$(eval $(call includeSubDir,src/app/AppTwo))

$(eval $(call includeSubDir,test/tprocess))
#$(eval $(call includeSubDir,test/handle))

