LDEBUG = y

export TOPDIR := $(CURDIR)
export RULESMK := $(CURDIR)/rules.mk
export ARCH := x86
# __i386__, __x86_64__, __arm__, __mips__
export ARCHD := __i386__


all:
world:
	#make -C src/utils
	#make -C src/binder


include $(RULESMK)
$(eval $(call preCalled))
$(eval $(call includeSubDir,src/liblog))
$(eval $(call includeSubDir,src/libcutils))
$(eval $(call includeSubDir,src/libcorkscrew))
$(eval $(call includeSubDir,src/utils))
$(eval $(call includeSubDir,src/binder))

