#include "joints_task.h"
#include "FreeRTOS.h"
#include "joint_task.h"
#include "joints_manager.h"
#include "queue.h"
#include "queue_manager.h"
#include "task.h"
#include "task_manager.h"
#include "tim.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#define JOINTS_TASK_STACK_DEPTH (4096U / sizeof(StackType_t))
#define JOINTS_TASK_PRIORITY (1U)

#define JOINTS_QUEUE_ITEMS (10U)
#define JOINTS_QUEUE_ITEM_SIZE (sizeof(joints_event_t))
#define JOINTS_QUEUE_STORAGE_SIZE (JOINTS_QUEUE_ITEMS * JOINTS_QUEUE_ITEM_SIZE)

static void joints_task_func(void*)
{
    joints_manager_t joints_manager;
    joints_manager_initialize(&joints_manager);

    while (1) {
        joints_manager_process(&joints_manager);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void joints_task_initialize(void)
{
    static StaticTask_t joints_task_buffer;
    static StackType_t joints_task_stack[JOINTS_TASK_STACK_DEPTH];

    TaskHandle_t joints_task = xTaskCreateStatic(joints_task_func,
                                                 "joints_task",
                                                 JOINTS_TASK_STACK_DEPTH,
                                                 NULL,
                                                 JOINTS_TASK_PRIORITY,
                                                 joints_task_stack,
                                                 &joints_task_buffer);

    task_manager_set(TASK_TYPE_JOINTS, joints_task);
}

void joints_queue_initialize(void)
{
    static StaticQueue_t joints_queue_buffer;
    static uint8_t joints_queue_storage[JOINTS_QUEUE_STORAGE_SIZE];

    QueueHandle_t joints_queue = xQueueCreateStatic(JOINTS_QUEUE_ITEMS,
                                                    JOINTS_QUEUE_ITEM_SIZE,
                                                    joints_queue_storage,
                                                    &joints_queue_buffer);

    queue_manager_set(QUEUE_TYPE_JOINTS, joints_queue);
}

#undef JOINTS_TASK_STACK_DEPTH
#undef JOINTS_TASK_PRIORITY

#undef JOINTS_QUEUE_ITEMS
#undef JOINTS_QUEUE_ITEM_SIZE
#undef JOINTS_QUEUE_STORAGE_SIZE