ENTRY(_start)

SECTIONS {
    /* Pi3B+ firmware loads kernel8.img at 0x80000 */
    . = 0x8000; /* means current location counter, this is the location where the kernel will be loaded*/

    /* Boot code (reset vector) */
    .text.boot : {
        KEEP(*(.text.boot))
    }

    /* Exception vector table (must exist and be 2KB aligned) */
    .vectors : {
        /* ensure 2KB alignment required by ARM */
        . = ALIGN(2048);
        KEEP(*(.vectors)) /*exact section name*/
        KEEP(*(.vectors.*)) /*subsection names*/
        . = ALIGN(2048);
    }

    /* Regular code */
    .text : {
        *(.text*)
    }

    /* Read-only data */
    .rodata : {        /*all normal code like C fn*/
        *(.rodata*)
    }

    /* Initialized data */
    .data : {
        *(.data*)
    }

    /* Uninitialized data (zero at boot) */
    .bss : {
        __bss_start = .;
        *(.bss*)
        __bss_end = .;
    }

    /* Define top of address stack (16MB in RAM, well above kernel) */
    _stack_top = 0x01000000;   /* adjust if needed */
}