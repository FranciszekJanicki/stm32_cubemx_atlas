#include "task_manager.h"
#include <assert.h>

static TaskHandle_t tasks[TASK_TYPE_TASK_NUM];

void task_manager_set(TaskType_t type, TaskHandle_t task)
{
    assert(task);
    assert(type < TASK_TYPE_TASK_NUM);
    assert(!tasks[type]);

    tasks[type] = task;
}

TaskHandle_t task_manager_get(TaskType_t type)
{
    assert(type < TASK_TYPE_TASK_NUM);
    assert(tasks[type]);

    return tasks[type];
}
