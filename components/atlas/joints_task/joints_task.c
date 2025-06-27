#include "joints_task.h"
#include "FreeRTOS.h"
#include "joint_manager.h"
#include "joint_task.h"
#include "queue.h"
#include "queue_manager.h"
#include "task.h"
#include "task_manager.h"
#include "tim.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#define JOINTS_TASK_STACK_DEPTH (1024U / sizeof(StackType_t))
#define JOINTS_TASK_PRIORITY (1U)

static TaskHandle_t joints_task;
static StaticTask_t joints_task_buffer;
static StackType_t joints_task_stack[JOINTS_TASK_STACK_DEPTH];

#define JOINTS_QUEUE_ITEMS (1U)
#define JOINTS_QUEUE_ITEM_SIZE (sizeof(joints_event_t))
#define JOINTS_QUEUE_STORAGE_SIZE (JOINTS_QUEUE_ITEMS * JOINTS_QUEUE_ITEM_SIZE)

static QueueHandle_t joints_queue;
static StaticQueue_t joints_queue_buffer;
static uint8_t joints_queue_storage[JOINTS_QUEUE_STORAGE_SIZE];

static void joints_task_func(void*)
{
    uint32_t notify;
    joints_event_t event;

    joint_event_t joint_events[JOINT_NUM];

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

joint_err_t joint_tasks_initialize(void)
{
    joint_err_t err;

    for (joint_num_t num = 0; num < JOINT_NUM; ++num) {
        err = joint_task_initialize(num);
        if (err != JOINT_ERR_OK) {
            return err;
        }
    }

    return err;
}

joint_err_t joint_queues_initialize(void)
{
    joint_err_t err;

    for (joint_num_t num = 0; num < JOINT_NUM; ++num) {
        err = joint_queue_initialize(num);
        if (err != JOINT_ERR_OK) {
            return err;
        }
    }

    return err;
}

joint_err_t joints_task_initialize(void)
{
    joints_task = xTaskCreateStatic(joints_task_func,
                                    "joints_task",
                                    JOINTS_TASK_STACK_DEPTH,
                                    NULL,
                                    JOINTS_TASK_PRIORITY,
                                    joints_task_stack,
                                    &joints_task_buffer);

    if (!joints_task) {
        return JOINT_ERR_FAIL;
    }

    task_manager_set(TASK_TYPE_JOINTS, joints_task);

    return joint_tasks_initialize();
}

joint_err_t joints_queue_initialize(void)
{
    joints_queue = xQueueCreateStatic(JOINTS_QUEUE_ITEMS,
                                      JOINTS_QUEUE_ITEM_SIZE,
                                      joints_queue_storage,
                                      &joints_queue_buffer);

    if (!joints_queue) {
        return JOINT_ERR_FAIL;
    }

    queue_manager_set(QUEUE_TYPE_JOINTS, joints_queue);

    return joint_queues_initialize();
}