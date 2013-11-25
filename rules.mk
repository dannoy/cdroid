

DEBUG = n
CDEBUG = n
CFLAGS := -m32 -I$(TOPDIR)/include -I$(current_dir) $(local_cflags) -include config/linux-$(ARCH).h
LDFLAGS := -L$(TOPDIR)/lib $(local_ldflags)
#OBJ = $(TOPDIR)/obj
OBJ = obj

ifeq ($(DEBUG),y)
    Q =
else
    Q = @
endif

ifeq ($(CDEBUG),y)
    CFLAGS += -g
else
    CFLAGS += -DLOG_NDEBUG
endif

all:

& = $(filter-out %.h %.hpp %.d %.a %.so,$^)
&2 = $(filter-out %.h %.hpp %.d %.so,$^)

%.lex.cpp: %.l
	@echo  -e "\tLEX" $@
	$(Q) flex -o $@ $<

%.tab.cpp: %.y
	@echo -e "\tYACC" $@
	$(Q) bison -d $< -o $@

.PRECIOUS:%.lex.c %.tab.c

define compile.prog
	$(Q) $(2) -m32 -o $$@ $$(&2) $(LDFLAGS) -Wl,-Map,$(OBJ)/$(1).map
endef

define compile.slib
	$(Q) $(2) -m32 -shared -Wl,-soname,$$(@F) -o $$@  $$^ $(LDFLAGS)
endef

define compile.prog.o
	$(Q) $(2) $(CFLAGS) -c -o $$@ $(1)
endef

define compile.slib.o
	$(Q) $(2) -fPIC $(CFLAGS) -c -o $$@ $(1)
endef

define compile.d
	$(Q) $(2) -MM $(CFLAGS) $$< | sed -n "H;$$$$ {g;s@.*:\(.*\)@$$< := \$$$$\(wildcard\1\)\n$${@:%.d=%.o} $$@: $$$$\($$<\)@;p}" > $$@
endef

#$(OBJ)/$(1:%.c=%.o):$(1) $(1:%.c=%.h)
define prog_object_template
$(filter %.o,$(OBJ)/$(1:%.cpp=%.o) $(OBJ)/$(1:%.c=%.o)):$(1) $(filter %.d,$(OBJ)/$(1:%.cpp=%.d) $(OBJ)/$(1:%.c=%.d))
	$(Q) mkdir -p $$(@D)
	$(Q) echo -e "\tCC $$<"
$(call compile.prog.o, $$&,$(if $(filter %.cpp,$(1)),$(CXX),$(CC)))

$(filter %.d,$(OBJ)/$(1:%.cpp=%.d) $(OBJ)/$(1:%.c=%.d)):$(1)
	$(Q) echo -e "\tDEP $$<"
	@mkdir -p $$(@D)
$(call compile.d,$(1),$(if $(filter %.cpp,$(1)),$(CXX),$(CC)))
endef

#$(OBJ)/$(1:%.cpp=%.o):$(1)
define slib_object_template
$(filter %.o,$(OBJ)/$(1:%.cpp=%.o) $(OBJ)/$(1:%.c=%.o)):$(1) $(filter %.d,$(OBJ)/$(1:%.cpp=%.d) $(OBJ)/$(1:%.c=%.d))
	$(Q) mkdir -p $$(@D)
	$(Q) echo -e "\tSCC $$<"
$(call compile.slib.o, $$&,$(if $(filter %.cpp,$(1)),$(CXX),$(CC)))

$(filter %.d,$(OBJ)/$(1:%.cpp=%.d) $(OBJ)/$(1:%.c=%.d)):$(1)
	$(Q) echo -e "\tSDEP $$<"
	@mkdir -p $$(@D)
$(call compile.d,$(1),$(if $(filter %.cpp,$(1)),$(CXX),$(CC)))
endef

define prog_template
bin/$(1): $($(1)_dep) $(filter %.o,$($(1)_src:%.cpp=$(OBJ)/%.o) $($(1)_src:%.c=$(OBJ)/%.o))
	$(Q) mkdir -p $$(@D)
	$(Q) echo -e "\tLD $(1)"
$(call compile.prog,$(1),$(if $(filter %.cpp,$($(1)_src)),$(CXX),$(CC)))

$(foreach src,$($(1)_src),$(eval $(call prog_object_template,$(src))))

ifeq ($(filter clean distclean tags,$(MAKECMDGOALS)),)
-include $(filter %.d,$($(1)_src:%.cpp=$(OBJ)/%.d) $($(1)_src:%.c=$(OBJ)/%.d))
endif

progs_target : bin/$(1)
endef

#lib/$(1).so:$(OBJ)/$($(1)_src:%.c=%.o) $(OBJ)/$($(1)_src:%.cpp=%.o)
#$(foreach src,$($(1)_src),$(info $(call slib_object_template,$(src))))
define slib_template

lib/$(1).so: $($(1)_dep) $(filter %.o,$($(1)_src:%.cpp=$(OBJ)/%.o) $($(1)_src:%.c=$(OBJ)/%.o))
	$(Q) mkdir -p $$(@D)
	$(Q) echo -e "\tShared lib/$(1).so"
$(call compile.slib,$(1),$(if $(filter %.cpp,$($(1)_src)),$(CXX),$(CC)))

$(foreach src,$($(1)_src),$(eval $(call slib_object_template,$(src))))

ifeq ($(filter clean distclean tags,$(MAKECMDGOALS)),)
-include $(filter %.d,$($(1)_src:%.cpp=$(OBJ)/%.d) $($(1)_src:%.c=$(OBJ)/%.d))
endif

shared_libs_target : lib/$(1).so
endef

$(foreach prog,$(progs),$(eval $(call prog_template,$(prog))))
#$(foreach prog,$(progs),$(info $(call prog_template,$(prog))))
$(foreach slib,$(shared_libs),$(eval $(call slib_template,$(slib))))
#$(foreach slib,$(shared_libs),$(info $(call slib_template,$(slib))))

define preCalled
prepare:$(OBJ)
$(OBJ):
	$(Q) mkdir -p $(OBJ)
clean:
	$(Q) find src -name "*.tab*" -exec rm -rf '{}' \;
	$(Q) find src -name "*.lex*" -exec rm -rf '{}' \;
	$(Q) rm -rf $(TOPDIR)/{bin,lib,obj}
distclean:
	$(Q) rm -rf $(TOPDIR)/{bin,lib,obj} cscope* tags
tags:
	$(Q) ctags -R .
	$(Q) cscope -Rbq
.PHONY:clean distclean tags prepare
endef

define includeSubDir
$(eval current_dir := $(1)
	progs :=
	shared_libs :=
	local_cflags :=
	local_ldflags :=
	include $(1)/Makefile)
endef

progs_target:
shared_libs_target:

all:progs_target shared_libs_target
