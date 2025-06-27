#include "joint_task.h"
#include "FreeRTOS.h"
#include "joint_manager.h"
#include "queue_manager.h"
#include "task.h"
#include "task_manager.h"
#include "tim.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static joint_manager_t joint_managers[JOINT_NUM] = {
    [JOINT_NUM_1] = {.delta_timer = &htim2,
                     .pwm_timer = &htim1,
                     .pwm_channel = TIM_CHANNEL_4,
                     .dir_pin = GPIO_PIN_10,
                     .dir_port = GPIOA},
    [JOINT_NUM_2] = {},
    [JOINT_NUM_3] = {},
    [JOINT_NUM_4] = {},
    [JOINT_NUM_5] = {},
    [JOINT_NUM_6] = {},
};

static joint_manager_config_t joint_manager_configs[JOINT_NUM] = {
    [JOINT_NUM_1] = {.kp = 10.0F,
                     .ki = 0.0F,
                     .kd = 0.0F,
                     .kc = 0.0F,
                     .min_angle = 0.0F,
                     .max_angle = 359.0F,
                     .min_speed = 10.0F,
                     .max_speed = 500.0F,
                     .current_limit = 2.0F,
                     .step_change = 1.8F},
    [JOINT_NUM_2] = {},
    [JOINT_NUM_3] = {},
    [JOINT_NUM_4] = {},
    [JOINT_NUM_5] = {},
    [JOINT_NUM_6] = {},
};

static TaskHandle_t joint_tasks[JOINT_NUM];
static StaticTask_t joint_task_buffers[JOINT_NUM];
static StackType_t joint_task_stacks[JOINT_NUM][1024U / sizeof(StackType_t)];

static QueueHandle_t joint_queues[JOINT_NUM];
static StaticQueue_t joint_queue_buffers[JOINT_NUM];
static uint8_t joint_queue_storages[JOINT_NUM][1U * sizeof(joint_event_t)];

static void joint_task_func(void* param)
{
    joint_num_t num = (joint_num_t)param;

    joint_manager_initialize(&joint_managers[num], &joint_manager_configs[num]);

    uint32_t notify;
    joint_event_t event;

    while (1) {
        if (xTaskNotifyWait(0, JOINT_NOTIFY_ALL, &notify, pdMS_TO_TICKS(1))) {
            joint_manager_notify_handler(&joint_managers[num], notify);
        }

        if (xQueuePeek(joint_queues[num], &event, pdMS_TO_TICKS(1))) {
            joint_manager_event_handler(&joint_managers[num], &event);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

joint_err_t joint_task_initialize(joint_num_t num)
{
    joint_tasks[num] = xTaskCreateStatic(joint_task_func,
                                         "joint_task",
                                         sizeof(joint_task_stacks[num]) / sizeof(StackType_t),
                                         (void*)num,
                                         1U,
                                         joint_task_stacks[num],
                                         &joint_task_buffers[num]);

    if (!joint_tasks[num]) {
        return JOINT_ERR_FAIL;
    }

    task_manager_set((TaskType_t)num, joint_tasks[num]);

    return JOINT_ERR_OK;
}

joint_err_t joint_queue_initialize(joint_num_t num)
{
    joint_queues[num] = xQueueCreateStatic(1U,
                                           sizeof(joint_event_t),
                                           joint_queue_storages[num],
                                           &joint_queue_buffers[num]);

    if (!joint_queues[num]) {
        return JOINT_ERR_FAIL;
    }

    queue_manager_set((TaskType_t)num, joint_queues[num]);

    return JOINT_ERR_OK;
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
