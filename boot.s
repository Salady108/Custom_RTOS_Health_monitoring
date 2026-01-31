.section .text.boot
.globl _start
.align 11
.extern _stack_top
.extern __bss_start
.extern __bss_end
.extern kernel_main
.extern irq_handler
msr VBAR_EL1, x0
isb
.global vectors
vectors:
b .
b .
b .
b .

b .
b .
b .
b .

b .
b .
b irq_entry 
b .

b .
b .
b .
b .
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

.global irq_entry
irq_entry:
sub sp,sp, #(16*8)

stp x0,  x1,  [sp, #(16*0)]
stp x2,  x3,  [sp, #(16*1)]
stp x4,  x5,  [sp, #(16*2)]
stp x6,  x7,  [sp, #(16*3)]
stp x8,  x9,  [sp, #(16*4)]
stp x10, x11, [sp, #(16*5)]
stp x12, x13, [sp, #(16*6)]
stp x14, x15, [sp, #(16*7)]

bl irq_handler

ldp x14, x15, [sp, #(16*7)]
ldp x12, x13, [sp, #(16*6)]
ldp x10, x11, [sp, #(16*5)]
ldp x8,  x9,  [sp, #(16*4)]
ldp x6,  x7,  [sp, #(16*3)]
ldp x4,  x5,  [sp, #(16*2)]
ldp x2,  x3,  [sp, #(16*1)]
ldp x0,  x1,  [sp, #(16*0)]

add sp,sp, #(16*8)
eret