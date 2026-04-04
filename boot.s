.section .text.boot
.globl _start

// 1. ENTRY POINT

_start:
    b _entry_point


// 2. VECTOR TABLE (Aligned to 2KB)

.align 11
.global vectors
vectors:
    // --- Current EL with SP0 --- user stack pointer
    .balign 0x80
    b .
    .balign 0x80
    b .
    .balign 0x80
    b .
    .balign 0x80
    b .

    // --- Current EL with SPx (Used in EL1) ---
    .balign 0x80
    b .
    .balign 0x80
    b irq_entry     // IRQ Handler
    .balign 0x80
    b .
    .balign 0x80
    b .

    // (Lower ELs omitted for brevity)


// 3. STARTUP CODE (EL2 Detection)

_entry_point:
    // Check Current Exception Level (CurrentEL)
    mrs x0, CurrentEL
    and x0, x0, #0b1100  // Mask bits 2 and 3
    cmp x0, #8           // Check if EL2 (Value 8 = 1000b)
    b.eq el2_to_el1      // If EL2, drop to EL1
    b _start_el1         // If already EL1, continue

el2_to_el1:
    // Configure HCR_EL2 (Hypervisor Config)
    // Bit 31 (RW) = 1: EL1 is AArch64
    mov x0, #(1 << 31)
    msr hcr_el2, x0

    // Configure SPSR_EL2 (Saved Program Status)
    // Mask DAIF (Interrupts), set mode to EL1h (0b0101)
    mov x0, #0x3c5 
    msr spsr_el2, x0

    // Set return address (ELR_EL2) to our EL1 start label
    adr x0, _start_el1
    msr elr_el2, x0

    // "Return" from EL2, effectively jumping to EL1
    eret

_start_el1:
    // --- Now we are safely in EL1 ---
    
    // Set up stack
    ldr x5, =_stack_top
    mov sp, x5

    // Set VBAR_EL1 to our vector table
    ldr x0, =vectors
    msr VBAR_EL1, x0
    isb

    // Clear .bss
    ldr x5, =__bss_start
    ldr x6, =__bss_end
bss_clear_loop:
    cmp x5, x6
    b.hs kernel_entry
    str xzr, [x5], #8
    b bss_clear_loop

kernel_entry:
    bl kernel_main

halt:
    wfe
    b halt


// 4. IRQ HANDLER WRAPPER

.global irq_entry
irq_entry:
    sub sp, sp, #272

    stp x0,  x1,  [sp, #(16*0)]
    stp x2,  x3,  [sp, #(16*1)]
    stp x4,  x5,  [sp, #(16*2)]
    stp x6,  x7,  [sp, #(16*3)]
    stp x8,  x9,  [sp, #(16*4)]
    stp x10, x11, [sp, #(16*5)]
    stp x12, x13, [sp, #(16*6)]
    stp x14, x15, [sp, #(16*7)]
    stp x16, x17, [sp, #(16*8)]
    stp x18, x19, [sp, #(16*9)]
    stp x20, x21, [sp, #(16*10)]
    stp x22, x23, [sp, #(16*11)]
    stp x24, x25, [sp, #(16*12)]
    stp x26, x27, [sp, #(16*13)]
    stp x28, x29, [sp, #(16*14)]
    str x30, [sp, #(16*15)]

    mrs x1, elr_el1
    mrs x2, spsr_el1
    str x1, [sp, #(16*15) + 8]
    str x2, [sp, #(16*15) + 16]

    mov x0, sp
    bl irq_handler

    mov sp, x0

restore_context:
    ldr x1, [sp, #(16*15) + 8]
    ldr x2, [sp, #(16*15) + 16]
    msr elr_el1, x1
    msr spsr_el1, x2

    ldp x0,  x1,  [sp, #(16*0)]
    ldp x2,  x3,  [sp, #(16*1)]
    ldp x4,  x5,  [sp, #(16*2)]
    ldp x6,  x7,  [sp, #(16*3)]
    ldp x8,  x9,  [sp, #(16*4)]
    ldp x10, x11, [sp, #(16*5)]
    ldp x12, x13, [sp, #(16*6)]
    ldp x14, x15, [sp, #(16*7)]
    ldp x16, x17, [sp, #(16*8)]
    ldp x18, x19, [sp, #(16*9)]
    ldp x20, x21, [sp, #(16*10)]
    ldp x22, x23, [sp, #(16*11)]
    ldp x24, x25, [sp, #(16*12)]
    ldp x26, x27, [sp, #(16*13)]
    ldp x28, x29, [sp, #(16*14)]
    ldr x30, [sp, #(16*15)]

    add sp, sp, #272
    eret

// End of boot code.


.global rtos_start_first_task
rtos_start_first_task:
    mov sp, x0
    b restore_context