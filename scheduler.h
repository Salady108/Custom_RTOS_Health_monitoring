#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

typedef void (*task_entry_t)(void *arg);

void scheduler_init(void);
int scheduler_create_task(const char *name, task_entry_t entry, void *arg);
uintptr_t scheduler_tick(uintptr_t current_sp);
uintptr_t scheduler_first_sp(void);
void rtos_start_first_task(uintptr_t first_sp);

#endif
