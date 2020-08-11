
include ../../../../config.mk
include ../../../../../Makefile.inc

################################################################################
## set flags for golobal compile and link setting.
################################################################################

CONFIG_FOR_COMPILE = $(LOCAL_CFLAGS)
CONFIG_FOR_LINK = 


################################################################################

BuildPath      = ./build
ObjectPath     = $(BuildPath)/obj
OutputPath     = .
DependFilePath = $(BuildPath)/dep
Target         = $(OutputPath)/libjpeg_soft_sysdep.a   ## output target.

ifneq ($(BuildPath),wildcard($(BuildPath)))
a := $(shell mkdir -p $(BuildPath))
endif
ifneq ($(ObjectPath),wildcard($(ObjectPath)))
a := $(shell mkdir -p $(ObjectPath))
endif
ifneq ($(DependFilePath),wildcard($(DependFilePath)))
a := $(shell mkdir -p $(DependFilePath))
endif


################################################################################
## set the source files, object files and dependency files
################################################################################
## set the source path to VPATH.

## set the source files.
SourceFiles  = jmemnobs.c

## set the object files.
ObjectFiles = $(addprefix $(ObjectPath)/, $(addsuffix .o ,$(basename $(notdir $(SourceFiles)))))

## set the dependency files.
DependFiles = $(addprefix $(DependFilePath)/, $(addsuffix .d ,$(notdir $(basename $(SourceFiles)))))


################################################################################
## set flags for compile and link
################################################################################

## set the include path for compile flags.
SourceIncludePath = $(foreach dir,$(SourcePath),-I$(dir)) -I../../include/ -I../../ -I../include/

## set compile flags
CompileFlags := $(CONFIG_FOR_COMPILE) $(SourceIncludePath) -O3 -fPIC -ldl

## set link flags
LoadFlags = $(CONFIG_FOR_LINK) 


################################################################################
## make commands, all/clean/cleanall
################################################################################

## define commands for make, sush as all, clean
.PHONY: all clean cleantarget cleanall
all:$(Target)

clean:
	-rm -f $(ObjectPath)/*
	-rm -rf $(Target)

cleanall: clean
	-rm -f $(DependFilePath)/*
	-rm -rf $(BuildPath)



################################################################################
## define target dependencies.
################################################################################

## compile source files to object files.
$(ObjectPath)/%.o:%.c
	$(CC) $(CompileFlags) -o $@ -c $<

$(ObjectFiles):$(ObjectPath)/%.o:%.c 
	$(CC) $(CompileFlags) -o $@ -c $<

## link object files to the target share library.
$(Target):$(ObjectFiles)
	$(AR) -r $@ $^ $(LoadFlags)

## set rules to generate .d files.
$(DependFilePath)/%.d:%.c
	set -e; rm -f $@; \
	$(CC) -MM $(CompileFlags) $< > $@.$$$$; \
	sed 's,\($*\)\.o[:]*,$(ObjectPath)/\1.o $@: ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

## include the .d files to set dependency rules.
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),cleantarget)
ifneq ($(MAKECMDGOALS),cleanall)
-include $(DependFiles)
endif
endif
endif


