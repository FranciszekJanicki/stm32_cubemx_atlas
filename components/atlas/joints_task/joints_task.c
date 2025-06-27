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

#define JOINTS_TASK_STACK_DEPTH (1024U / sizeof(StackType_t))
#define JOINTS_TASK_PRIORITY (1U)

#define JOINTS_QUEUE_ITEMS (1U)
#define JOINTS_QUEUE_ITEM_SIZE (sizeof(joints_event_t))
#define JOINTS_QUEUE_STORAGE_SIZE (JOINTS_QUEUE_ITEMS * JOINTS_QUEUE_ITEM_SIZE)

static joints_config_t joints_config = {
    .joints_ctxs = {

        [JOINT_NUM_1] = {.manager = {.delta_timer = &htim2,
                                     .pwm_timer = &htim1,
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
                                    .max_speed = 500.0F,
                                    .current_limit = 2.0F,
                                    .step_change = 1.8F}},

        [JOINT_NUM_2] = {.manager = {.delta_timer = NULL,
                                     .pwm_timer = NULL,
                                     .pwm_channel = 0x0000U,
                                     .dir_pin = 0x0000U,
                                     .dir_port = NULL},
                         .config = {.kp = 0.0F,
                                    .ki = 0.0F,
                                    .kd = 0.0F,
                                    .kc = 0.0F,
                                    .min_angle = 0.0F,
                                    .max_angle = 0.0F,
                                    .min_speed = 0.0F,
                                    .max_speed = 0.0F,
                                    .current_limit = 0.0F,
                                    .step_change = 0.0F}},

        [JOINT_NUM_3] = {.manager = {.delta_timer = NULL,
                                     .pwm_timer = NULL,
                                     .pwm_channel = 0x0000U,
                                     .dir_pin = 0x0000U,
                                     .dir_port = NULL},
                         .config = {.kp = 0.0F,
                                    .ki = 0.0F,
                                    .kd = 0.0F,
                                    .kc = 0.0F,
                                    .min_angle = 0.0F,
                                    .max_angle = 0.0F,
                                    .min_speed = 10.0F,
                                    .max_speed = 0.0F,
                                    .current_limit = 0.0F,
                                    .step_change = 0.0F}},

        [JOINT_NUM_4] = {.manager = {.delta_timer = NULL,
                                     .pwm_timer = NULL,
                                     .pwm_channel = 0x0000U,
                                     .dir_pin = 0x0000U,
                                     .dir_port = NULL},
                         .config = {.kp = 0.0F,
                                    .ki = 0.0F,
                                    .kd = 0.0F,
                                    .kc = 0.0F,
                                    .min_angle = 0.0F,
                                    .max_angle = 0.0F,
                                    .min_speed = 0.0F,
                                    .max_speed = 0.0F,
                                    .current_limit = 0.0F,
                                    .step_change = 0.0F}},

        [JOINT_NUM_5] = {.manager = {.delta_timer = NULL,
                                     .pwm_timer = NULL,
                                     .pwm_channel = 0x0000U,
                                     .dir_pin = 0x0000U,
                                     .dir_port = NULL},
                         .config = {.kp = 0.0F,
                                    .ki = 0.0F,
                                    .kd = 0.0F,
                                    .kc = 0.0F,
                                    .min_angle = 0.0F,
                                    .max_angle = 0.0F,
                                    .min_speed = 0.0F,
                                    .max_speed = 0.0F,
                                    .current_limit = 0.0F,
                                    .step_change = 0.0F}},

        [JOINT_NUM_6] = {.manager = {.delta_timer = NULL,
                                     .pwm_timer = NULL,
                                     .pwm_channel = 0x0000U,
                                     .dir_pin = 0x0000U,
                                     .dir_port = NULL},
                         .config = {.kp = 0.0F,
                                    .ki = 0.0F,
                                    .kd = 0.0F,
                                    .kc = 0.0F,
                                    .min_angle = 0.0F,
                                    .max_angle = 0.0F,
                                    .min_speed = 0.0F,
                                    .max_speed = 0.0F,
                                    .current_limit = 0.0F,
                                    .step_change = 0.0F}}

    }};

static joints_manager_t joints_manager = {};

static void joints_task_func(void*)
{
    joints_manager_initialize(&joints_manager, &joints_config);

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

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef* htim)
{
    BaseType_t task_woken = pdFALSE;

    if (htim->Instance == TIM1) {
        xTaskNotifyFromISR(joints_config.joints_ctxs[JOINT_NUM_1].task,
                           JOINT_NOTIFY_PWM_PULSE,
                           eSetBits,
                           &task_woken);
    }

    portYIELD_FROM_ISR(task_woken);
}