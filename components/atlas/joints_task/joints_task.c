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

static char const* const TAG = "joints_task";

#define JOINTS_TASK_STACK_DEPTH (4096U / sizeof(StackType_t))
#define JOINTS_TASK_PRIORITY (2U)

#define JOINTS_QUEUE_ITEMS (10U)
#define JOINTS_QUEUE_ITEM_SIZE (sizeof(joints_event_t))
#define JOINTS_QUEUE_STORAGE_SIZE (JOINTS_QUEUE_ITEMS * JOINTS_QUEUE_ITEM_SIZE)

static joint_ctx_t joint_ctxs[JOINT_NUM] = {
    [JOINT_NUM_1] = {.manager = {.pwm_timer = &htim1,
                                 .pwm_channel = TIM_CHANNEL_4,
                                 .dir_pin = GPIO_PIN_10,
                                 .dir_port = GPIOA},
                     .config = {.kp = 10.0F,
                                .ki = 0.0F,
                                .kd = 0.0F,
                                .kc = 0.0F,
                                .min_angle = 0.0F,
                                .max_angle = 359.0F,
                                .min_speed = 10.0F,
                                .max_speed = 500.0F}}};

static joints_manager_t joints_manager = {.delta_timer = &htim2};

static void joints_task_func(void*)
{
    joints_manager_initialize(&joints_manager);

    while (1) {
        LOG_ON_ERROR(TAG, joints_manager_process(&joints_manager));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void joint_tasks_initialize(void)
{
    static StackType_t joint_task_stacks[JOINT_NUM][JOINT_TASK_STACK_DEPTH];
    static StaticTask_t joint_task_buffers[JOINT_NUM];

    for (uint8_t num = 0; num < JOINT_NUM; ++num) {
        joints_manager.joint_ctxs[num].task = joint_task_initialize(&joint_ctxs[num],
                                                                    &joint_task_buffers[num],
                                                                    &joint_task_stacks[num]);
    }
}

void joint_queues_initialize(void)
{
    static uint8_t joint_queue_storages[JOINT_NUM][JOINT_QUEUE_STORAGE_SIZE];
    static StaticQueue_t joint_queue_buffers[JOINT_NUM];

    for (uint8_t num = 0; num < JOINT_NUM; ++num) {
        joints_manager.joint_ctxs[num].queue = joint_queue_initialize(&joint_ctxs[num],
                                                                      &joint_queue_buffers[num],
                                                                      &joint_queue_storages[num]);
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

    joint_tasks_initialize();
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

    joint_queues_initialize();
}

#undef JOINTS_TASK_STACK_DEPTH
#undef JOINTS_TASK_PRIORITY

#undef JOINTS_QUEUE_ITEMS
#undef JOINTS_QUEUE_ITEM_SIZE
#undef JOINTS_QUEUE_STORAGE_SIZE