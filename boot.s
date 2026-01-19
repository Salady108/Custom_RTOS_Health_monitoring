.section .text.boot
.globl _start

.extern _stack_top
.extern __bss_start
.extern __bss_end

_start:
    // Set up stack
    ldr x5, =_stack_top
    mov sp, x5

    // Clear .bss
    ldr x5, =__bss_start
    ldr x6, =__bss_end

1:
    cmp x5, x6
    b.hs 2f
    str xzr, [x5], #8
    b 1b

2:
    // Jump to C
    bl kernel_main

halt:
    wfe
    b halt
