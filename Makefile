# =========================
# Bare-metal Makefile for Raspberry Pi 3B+ (AArch64)
# =========================

# ---- Toolchain ----
CC      = aarch64-linux-gnu-gcc
OBJCOPY = aarch64-linux-gnu-objcopy

# ---- Architecture flags (CRITICAL) ----
ARCH_FLAGS = -march=armv8-a -mcpu=cortex-a53 -mgeneral-regs-only

# ---- Compiler flags ----
CFLAGS  = -Wall -O0 -ffreestanding -nostdlib -nostartfiles \
          $(ARCH_FLAGS) \
          -I. -Ihw -Ihw/include -Irtos -Iapp

# ---- Linker flags ----
LDFLAGS = -T linker.ld -nostdlib

# ---- Source files ----
SRC_ASM = /mnt/shared/C++programming/Rpi_practice/boot.s\

SRC_C   = /mnt/shared/C++programming/Rpi_practice/kernel_main.c \
          /mnt/shared/C++programming/Rpi_practice/gpio.c \
          

# ---- Object files (ASM FIRST – VERY IMPORTANT) ----
OBJ_ASM = $(SRC_ASM:.S=.o)
OBJ_C   = $(SRC_C:.c=.o)
OBJ     = $(OBJ_ASM) $(OBJ_C)

# ---- Target names ----
TARGET = kernel

# ---- Default target ----
all: $(TARGET)8.img

# ---- Compile C ----
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ---- Assemble ASM ----
%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

# ---- Link ELF ----
$(TARGET).elf: $(OBJ) linker.ld
	$(CC) $(ARCH_FLAGS) $(LDFLAGS) -o $@ $(OBJ)

# ---- ELF → raw binary ----
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# ---- Raspberry Pi kernel image (AArch64) ----
$(TARGET)8.img: $(TARGET).bin
	cp $< $@

# ---- Clean ----
clean:
	rm -f $(OBJ) $(TARGET).elf $(TARGET).bin $(TARGET)8.img
