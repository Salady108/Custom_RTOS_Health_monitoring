#include "scheduler.h"

#define MAX_TASKS 4
#define TASK_STACK_WORDS 512
#define TASK_SPSR_EL1 0x5

typedef struct {
    uint64_t x0_to_x30[31];
    uint64_t elr_el1;
    uint64_t spsr_el1;
    uint64_t pad;
} task_frame_t;

typedef struct {
    uintptr_t sp;
    uint64_t stack[TASK_STACK_WORDS] __attribute__((aligned(16)));
    task_entry_t entry;
    void *arg;
    const char *name;
    int used;
} task_t;

static task_t tasks[MAX_TASKS];
static int task_count = 0;
static int current_task = 0;

static void task_exit_trap(void) {
    for (;;) {
        asm volatile("wfi");
    }
}

void scheduler_init(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].sp = 0;
        tasks[i].entry = 0;
        tasks[i].arg = 0;
        tasks[i].name = 0;
        tasks[i].used = 0;
    }
    task_count = 0;
    current_task = 0;
}

static task_frame_t *make_initial_frame(task_t *task) {
    uintptr_t top = (uintptr_t)&task->stack[TASK_STACK_WORDS];
    top &= ~(uintptr_t)0xF;
    task_frame_t *frame = (task_frame_t *)(top - sizeof(task_frame_t));

    for (int i = 0; i < 31; i++) {
        frame->x0_to_x30[i] = 0;
    }
    frame->x0_to_x30[0] = (uint64_t)task->arg;
    frame->x0_to_x30[30] = (uint64_t)task_exit_trap;
    frame->elr_el1 = (uint64_t)task->entry;
    frame->spsr_el1 = TASK_SPSR_EL1;
    frame->pad = 0;
    return frame;
}

int scheduler_create_task(const char *name, task_entry_t entry, void *arg) {
    if (task_count >= MAX_TASKS) {
        return -1;
    }

    task_t *task = &tasks[task_count];
    task->entry = entry;
    task->arg = arg;
    task->name = name;
    task->used = 1;
    task->sp = (uintptr_t)make_initial_frame(task);
    task_count++;
    return task_count - 1;
}

uintptr_t scheduler_first_sp(void) {
    if (task_count == 0) {
        return 0;
    }
    current_task = 0;
    return tasks[0].sp;
}

uintptr_t scheduler_tick(uintptr_t current_sp) {
    if (task_count == 0) {
        return current_sp;
    }

    tasks[current_task].sp = current_sp;
    current_task = (current_task + 1) % task_count;
    return tasks[current_task].sp;
}
