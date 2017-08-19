####################################################################
#  THIS FILE IS 100% GENERATED; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
####################################################################

# Toolchain executables
CROSS_COMPILE = 
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE = $(CROSS_COMPILE)size
NM = $(CROSS_COMPILE)nm

# Include files
INCLUDES = \
    -Iinclude \
    -I../../core/include \
    -I../../trace/include \
    -I../../platform/linux/include \

OPT_FLAGS = -O0

# Global defines
CFLAGS += \

# Global C flags
CFLAGS += -std=gnu99 -g -Werror \

# Global linker flags

# Global libraries

THIS_MAKEFILE := $(lastword $(MAKEFILE_LIST))
CFLAGS += $(INCLUDES)

SOURCES_smokers = \
    core/src/esm.c \
    core/src/esm_list.c \
    core/src/esm_timer.c \
    platform/linux/src/main.c \
    trace/src/rb.c \
    trace/src/trace.c \
    src/smoker.c \
    src/table.c \

C_OBJ_smokers = $(patsubst %.c, obj/%.o, $(filter %.c, $(SOURCES_smokers)))
ASM_OBJ_smokers = $(patsubst %.S, obj/%.o, $(filter %.S, $(SOURCES_smokers)))

vpath %.c \
    ../../ \

vpath %.S \
    ../../ \

V ?= 0
Q_0 := @
Q_1 := 
Q = $(Q_$(V))

.SUFFIXES:
.PHONY: all clean

define loginfo
      $(Q)echo "[INFO] $1"
endef

define logerror
      $(Q)echo "[ERR ] $1"
endef

all: build

build: smokers

smokers: $(C_OBJ_smokers) $(ASM_OBJ_smokers) $(THIS_MAKEFILE)
	$(call loginfo,"Linking exec" $@)
	$(Q)$(CC) $(LDFLAGS) -Wl,-M=bin/$@.map $(CFLAGS) \
		$(C_OBJ_smokers) $(ASM_OBJ_smokers) $(LIBS) -o bin/$@.elf
	$(Q)$(NM) bin/$@.elf >bin/$@.elf.txt
	$(Q)$(OBJCOPY) -O binary bin/$@.elf bin/$@.bin
	$(Q)$(SIZE) $(C_OBJ_smokers)
	$(Q)$(SIZE) --format=SysV bin/$@.elf

$(C_OBJ_smokers) $(ASM_OBJ_smokers) : | bin obj

bin:
	$(Q)mkdir -p $@

obj:
	$(Q)mkdir -p $@

obj/%.o: %.c $(THIS_MAKEFILE)
	$(call loginfo,"Compiling C file" $<)
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) -MT $@ -MMD -MP -MF obj/$*.Td $(OPT_FLAGS) $(CFLAGS) -c $< -o $@
	$(Q)mv -f obj/$*.Td obj/$*.d

obj/%.o: %.S $(THIS_MAKEFILE)
	$(call loginfo,"Compiling ASM file" $<)
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) -MT $@ -MMD -MP -MF obj/$*.Td $(OPT_FLAGS) $(CFLAGS) -c $< -o $@
	$(Q)mv -f obj/$*.Td obj/$*.d

-include esm.mk

clean:
	$(Q)rm -Rf bin
	$(Q)rm -Rf obj

obj/%.d: ;
.PRECIOUS: obj/%.d

-include $(patsubst %,obj/%.d,$(basename $(SOURCES_smokers)))
