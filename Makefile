SHELL=bash
#Project settings
PROJECT_NAME = rxlamp_pump
SOURCES = main.c hw.c ws2812.c gpio.c timer.c
BUILD_DIR = build/

OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)%.o)
TARGET_ELF = $(BUILD_DIR)$(PROJECT_NAME).elf
TARGET_BIN = $(TARGET_ELF:%.elf=%.bin)

#Toolchain settings
TOOLCHAIN = arm-none-eabi

CC = $(TOOLCHAIN)-gcc
OBJCOPY = $(TOOLCHAIN)-objcopy
OBJDUMP = $(TOOLCHAIN)-objdump
SIZE = $(TOOLCHAIN)-size

#Target CPU options
CPU_DEFINES = -mthumb -mcpu=cortex-m3 -msoft-float -DSTM32F1

#Compiler options
CFLAGS += -std=gnu99 -Os -Wall -fno-common -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += $(CPU_DEFINES)

INCLUDE_PATHS += -Ilib/libopencm3/include -Iinc

LINK_SCRIPT = stm32f100x6.ld

LINK_FLAGS =  --static -nostartfiles
LINK_FLAGS += -Llib/libopencm3/lib -Llib/libopencm3/lib/stm32/f1 
LINK_FLAGS += -T$(LINK_SCRIPT) 
LINK_FLAGS += -Wl,--start-group -lopencm3_stm32f1 -lm -lgcc -lnosys -Wl,--end-group -Wl,--gc-sections 

LIBS = libopencm3_stm32f1.a

#Not used for now but we should add it
DEBUG_FLAGS = -g   

#Directories
vpath %.c src
vpath %.o $(BUILD_DIR)
vpath %.ld lib/libopencm3/lib/stm32/f1
vpath %.a lib/libopencm3/lib

default: $(TARGET_BIN)

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $(TARGET_ELF) $(TARGET_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET_ELF): $(BUILD_DIR) $(LIBS) $(OBJECTS) $(LINK_SCRIPT)
	$(CC) $(OBJECTS) $(CFLAGS) $(LINK_FLAGS) -o $(TARGET_ELF) 2> >(python cleanup.py)

$(OBJECTS): $(BUILD_DIR)%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDE_PATHS) $^ -o $@ 2> >(python cleanup.py)

$(LINK_SCRIPT): libopencm3_stm32f1.a

libopencm3_stm32f1.a: lib/libopencm3/.git
	cd lib/libopencm3; $(MAKE) lib/stm32/f1

lib/libopencm3/.git:
	cd lib/libopencm3; git submodule init
	cd lib/libopencm3; git submodule update

clean:
	rm -f $(OBJECTS) $(TARGET_ELF) $(TARGET_BIN)

deep-clean: clean
	cd lib/libopencm3; $(MAKE) clean

upload: $(TARGET_BIN)
	stm32flash $(UART) -b 921600 -R cs.rts -B cs.dtr -w $^
	

.PHONY: default clean deep-clean libopencm3 upload

#makefile_debug:
