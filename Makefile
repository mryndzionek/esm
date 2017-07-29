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

OPT_FLAGS = -O0

# Global defines
CFLAGS += \
    -DESM \

# Global C flags
CFLAGS += -std=gnu99 -g \

# Global linker flags

# Global libraries

THIS_MAKEFILE := $(lastword $(MAKEFILE_LIST))
CFLAGS += $(INCLUDES)

SOURCES_esm_test = \
    core/esm.c \
    core/esm_list.c \
    core/esm_timer.c \
    src/blink.c \
    src/main.c \

C_OBJ_esm_test = $(patsubst %.c, obj/%.o, $(filter %.c, $(SOURCES_esm_test)))
ASM_OBJ_esm_test = $(patsubst %.S, obj/%.o, $(filter %.S, $(SOURCES_esm_test)))

vpath %.c \

vpath %.S \

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

build: esm_test

esm_test: $(C_OBJ_esm_test) $(ASM_OBJ_esm_test) $(THIS_MAKEFILE)
	$(call loginfo,"Linking exec" $@)
	$(Q)$(CC) $(LDFLAGS) -Wl,-M=bin/$@.map $(CFLAGS) \
		$(C_OBJ_esm_test) $(ASM_OBJ_esm_test) $(LIBS) -o bin/$@.elf
	$(Q)$(NM) bin/$@.elf >bin/$@.elf.txt
	$(Q)$(OBJCOPY) -O binary bin/$@.elf bin/$@.bin
	$(Q)$(SIZE) $(C_OBJ_esm_test)
	$(Q)$(SIZE) --format=SysV bin/$@.elf

$(C_OBJ_esm_test) $(ASM_OBJ_esm_test) : | bin obj

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

-include $(patsubst %,obj/%.d,$(basename $(SOURCES_esm_test)))
