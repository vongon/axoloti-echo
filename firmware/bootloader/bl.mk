# MIT License
#
# Copyright 2021 Tyler Coy
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

STACK_SIZE := 1024
include bl_config.inc.mk

TARGET := bl.elf
SOURCES := bl/*.cpp bl/hal/*.c bl/drivers/*.cpp
LD_SCRIPT := bl/app.ld

-include ../env.inc.mk
TGT_CC := $(TOOLCHAIN_PATH)arm-none-eabi-gcc
TGT_CXX := $(TOOLCHAIN_PATH)arm-none-eabi-g++

TGT_DEFS := \
	STM32F429xx \
	HSE_VALUE=8000000 \
	USE_FULL_ASSERT \
	USE_FULL_LL_DRIVER \
	STACK_SIZE=$(STACK_SIZE) \
	BOOTLOADER_SIZE=$(BOOTLOADER_SIZE) \
	SAMPLE_RATE=$(BOOTLOADER_SAMPLE_RATE) \
	SYMBOL_RATE=$(BOOTLOADER_SYMBOL_RATE) \
	PACKET_SIZE=$(BOOTLOADER_PACKET_SIZE) \
	BLOCK_SIZE=$(BOOTLOADER_BLOCK_SIZE) \
	CRC_SEED=$(BOOTLOADER_CRC_SEED) \

ARCHFLAGS := \
	-mthumb \
	-mthumb-interwork \
	-mcpu=cortex-m4 \
	-mfpu=fpv4-sp-d16 \
	-mfloat-abi=hard \
	-mfp16-format=alternative \

WARNFLAGS := \
	-Wall \
	-Wextra \
	-Wundef \

OPTFLAGS := \
	-ffunction-sections \
	-fdata-sections \
	-fshort-enums \
	-finline-functions-called-once \
	-ffast-math \
	-fsingle-precision-constant \
	-finline-functions \

TGT_INCDIRS  := . bl bl/hal

TGT_CFLAGS   := -Os -g $(ARCHFLAGS) $(OPTFLAGS) $(WARNFLAGS) -std=c11
TGT_CXXFLAGS := -Os -g $(ARCHFLAGS) $(OPTFLAGS) $(WARNFLAGS) -std=c++17 \
	-fno-exceptions -fno-rtti -Wno-register
TGT_LDLIBS   := -lm -lc -lgcc
TGT_LDFLAGS  := $(ARCHFLAGS) \
	-Wl,--gc-sections \
	-specs=nosys.specs \
	-specs=nano.specs \
	-T$(LD_SCRIPT) \
	-Wl,--defsym,STACK_SIZE=$(STACK_SIZE) \
	-Wl,--defsym,BOOTLOADER_SIZE=$(BOOTLOADER_SIZE) \

TARGET_ELF := $(TARGET_DIR)/$(TARGET)
TARGET_LSS := $(TARGET_ELF:.elf=.lss)
TARGET_HEX := $(TARGET_ELF:.elf=.hex)

.PHONY: bl
bl: $(TARGET_HEX) $(TARGET_LSS)

%.lss: %.elf
	$(TOOLCHAIN_PATH)arm-none-eabi-objdump -CdhtS $< > $@

%.hex: %.elf
	$(TOOLCHAIN_PATH)arm-none-eabi-objcopy --gap-fill 0xFF -O ihex $< $@

TGT_POSTCLEAN := $(RM) $(TARGET_LSS)

define TGT_POSTMAKE
@echo 'BINARY SIZE:' \
	$$($(TOOLCHAIN_PATH)arm-none-eabi-size $(TARGET_DIR)/$(TARGET) | tail -n1 | \
	awk '{ printf "0x%05X %d", $$1+$$2, $$1+$$2 }')
@echo 'RAM USAGE:  ' \
	$$($(TOOLCHAIN_PATH)arm-none-eabi-size $(TARGET_DIR)/$(TARGET) | tail -n1 | \
	awk '{ printf "0x%05X %d", $$2+$$3, $$2+$$3 }')
endef

.PHONY: sym
sym: $(TARGET_DIR)/$(TARGET)
	$(TOOLCHAIN_PATH)arm-none-eabi-nm -CnS $< | less

.PHONY: top
top: $(TARGET_DIR)/$(TARGET)
	$(TOOLCHAIN_PATH)arm-none-eabi-nm -CrS --size-sort $< | less

# ------------------------------------------------------------------------------
# Firmware flashing
# ------------------------------------------------------------------------------

OPENOCD_CMD := $(OPENOCD_PATH)openocd -c "debug_level 1" $(PGM_INTERFACE) \
	-f target/stm32f4x.cfg \
	-c "stm32f4x.cpu configure -event reset-init {}"

.PHONY: load
load: $(TARGET_DIR)/$(TARGET)
	$(OPENOCD_CMD) -c "program $< verify reset exit"

.PHONY: verify
verify:
	$(OPENOCD_CMD) -c init \
		-c "flash verify_bank 0 bl/data.bin $(BOOTLOADER_SIZE)" -c exit

.PHONY: erase
erase:
	$(OPENOCD_CMD) -c init -c halt -c "flash erase_sector 0 0 last" \
		-c reset -c exit
