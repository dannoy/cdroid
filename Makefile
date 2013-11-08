LDEBUG = y

export TOPDIR := $(CURDIR)
export RULESMK := $(CURDIR)/rules.mk


all:
world:
	#make -C src/utils
	#make -C src/binder


include $(RULESMK)
$(eval $(call preCalled))
$(eval $(call includeSubDir,src/liblog))
#$(eval $(call includeSubDir,src/utils))
#include src/utils/Makefile

