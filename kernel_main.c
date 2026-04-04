#include "uart.h"
#include "i2c.h"
#include "max30102.h"
#include "mpu6050.h"
#include "irq.h"
#include "timer.h"
#include "scheduler.h"
#include "app_tasks.h"

extern volatile uint64_t ticks;

uint32_t get_now_ms(void) {
    return (uint32_t)ticks;
}

void delay_ms(uint32_t ms) {
    uint32_t start = get_now_ms();
    while ((get_now_ms() - start) < ms) {
        asm volatile("wfi");
    }
}

void kernel_main(void) {
    uart_init();
    i2c_init();
    timer_init();
    irq_init();
    mpu6050_init();
    max30102_init();

    scheduler_init();
    scheduler_create_task("sensor", sensor_task, 0);
    scheduler_create_task("bpm", bpm_task, 0);
    scheduler_create_task("idle", idle_task, 0);

    uart_send_string("\n\n========================================\n");
    uart_send_string(" PI 3B+ HEART MONITOR RTOS KERNEL   \n");
    uart_send_string("========================================\n");

    uintptr_t first_sp = scheduler_first_sp();
    if (first_sp == 0) {
        uart_send_string("No tasks created. Halting.\r\n");
        for (;;) {
            asm volatile("wfi");
        }
    }

    rtos_start_first_task(first_sp);

    for (;;) {
        asm volatile("wfi");
    }
}
