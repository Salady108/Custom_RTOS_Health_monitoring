# =========================
# Bare-metal Makefile for Raspberry Pi 3B+ (AArch64)
# =========================

# ---- Toolchain ----
CC      = aarch64-linux-gnu-gcc
OBJCOPY = aarch64-linux-gnu-objcopy

# ---- Architecture flags ----
ARCH_FLAGS = -march=armv8-a -mcpu=cortex-a53 -mgeneral-regs-only

# ---- Compiler flags ----
CFLAGS  = -Wall -O0 -ffreestanding -nostdlib -nostartfiles \
          $(ARCH_FLAGS) -I.

# ---- Linker flags ----
LDFLAGS = -T linker.ld -nostdlib

# ---- Sources ----
ASM_SRCS = boot.s
C_SRCS=uart.c gpio.c kernel_main.c max30102.c i2c.c irq.c irq_handler.c timer.c

# ---- Objects ----
OBJS = $(ASM_SRCS:.s=.o) $(C_SRCS:.c=.o)

# ---- Target ----
TARGET = kernel

# ---- Default target ----
all: $(TARGET)8.img

# ---- Assemble (.s) ----
%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

# ---- Compile C ----
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ---- Link ----
$(TARGET).elf: $(OBJS) linker.ld
	$(CC) $(ARCH_FLAGS) $(LDFLAGS) -o $@ $(OBJS)

# ---- ELF → BIN ----
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# ---- Pi kernel image ----
$(TARGET)8.img: $(TARGET).bin
	cp $< $@

# ---- Clean ----
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).bin $(TARGET)8.img
