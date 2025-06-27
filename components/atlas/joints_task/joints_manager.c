#include "joints_manager.h"
#include "FreeRTOS.h"
#include "atlas_notify.h"
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

static inline bool joints_manager_send_joint_event(QueueHandle_t queue, joint_event_t const* event)
{
    return xQueueSend(queue, event, pdMS_TO_TICKS(1)) == pdTRUE;
}

static inline bool joints_manager_send_joint_notify(TaskHandle_t task, joint_notify_t notify)
{
    return xTaskNotify(task, notify, eSetBits) == pdTRUE;
}

static inline bool joints_manager_receive_joints_event(joints_event_t* event)
{
    return xQueueReceive(queue_manager_get(QUEUE_TYPE_JOINTS), event, pdMS_TO_TICKS(1)) == pdTRUE;
}

static inline bool joints_manager_receive_joints_notify(joints_notify_t* notify)
{
    return xTaskNotifyWait(0, JOINTS_NOTIFY_ALL, (uint32_t*)notify, pdMS_TO_TICKS(1)) == pdTRUE;
}

static atlas_err_t joints_manager_event_start_handler(joints_manager_t* manager)
{
    printf("joints_manager_event_start_handler\n\r");

    if (manager->is_running) {
        return ATLAS_ERR_ALREADY_RUNNING;
    }

    static joint_event_t event = {.type = JOINT_EVENT_TYPE_START};
    for (joint_num_t num = 0; num < JOINT_NUM; ++num) {
        if (!joints_manager_send_joint_event(manager->joint_ctxs[num].queue, &event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    manager->is_running = true;

    return ATLAS_ERR_OK;
}

static atlas_err_t joints_manager_event_stop_handler(joints_manager_t* manager)
{
    printf("joints_manager_event_stop_handler\n\r");

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    static joint_event_t event = {.type = JOINT_EVENT_TYPE_STOP};
    for (joint_num_t num = 0; num < JOINT_NUM; ++num) {
        if (!joints_manager_send_joint_event(manager->joint_ctxs[num].queue, &event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    manager->is_running = false;

    return ATLAS_ERR_OK;
}

static atlas_err_t joints_manager_event_update_handler(joints_manager_t* manager,
                                                       joints_event_payload_t const* payload)
{
    printf("joints_manager_event_update_handler\n\r");

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    static joint_event_t event = {.type = JOINT_EVENT_TYPE_UPDATE};
    for (joint_num_t num = 0; num < JOINT_NUM; ++num) {
        event.payload.update.position = payload->update[num].position;
        event.payload.update.delta_time = payload->update[num].delta_time;

        printf("num: %d, position: %f, delta_time: %f\n\r",
               num,
               event.payload.update.position,
               event.payload.update.delta_time);

        if (!joints_manager_send_joint_event(manager->joint_ctxs[num].queue, &event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t joints_manager_notify_delta_timer_handler(joints_manager_t* manager)
{
    printf("joints_manager_notify_delta_timer_handler\n\r");

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    for (joint_num_t num = 0; num < JOINT_NUM; ++num) {
        joints_manager_send_joint_notify(manager->joint_ctxs[num].task, JOINTS_NOTIFY_DELTA_TIMER);
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t joints_manager_notify_handler(joints_manager_t* manager, joints_notify_t notify)
{
    if (notify & JOINTS_NOTIFY_DELTA_TIMER) {
        atlas_err_t err = joints_manager_notify_delta_timer_handler(manager);
        if (err != ATLAS_ERR_OK) {
            return err;
        }
    }

    return ATLAS_ERR_UNKNOWN_NOTIFY;
}

static atlas_err_t joints_manager_event_handler(joints_manager_t* manager,
                                                joints_event_t const* event)
{
    switch (event->type) {
        case JOINTS_EVENT_TYPE_START: {
            return joints_manager_event_start_handler(manager);
        }
        case JOINTS_EVENT_TYPE_STOP: {
            return joints_manager_event_stop_handler(manager);
        }
        case JOINTS_EVENT_TYPE_UPDATE: {
            return joints_manager_event_update_handler(manager, &event->payload);
        }
    }

    return ATLAS_ERR_UNKNOWN_EVENT;
}

atlas_err_t joints_manager_process(joints_manager_t* manager)
{
    assert(manager);

    joints_notify_t notify;
    if (joints_manager_receive_joints_notify(&notify)) {
        atlas_err_t err = joints_manager_notify_handler(manager, notify);
        if (err != ATLAS_ERR_OK) {
            return err;
        }
    }

    joints_event_t event;
    if (joints_manager_receive_joints_event(&event)) {
        return joints_manager_event_handler(manager, &event);
    }

    return ATLAS_ERR_OK;
}

static joint_task_ctx_t joint_task_ctxs[JOINT_NUM] = {
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
                                .step_change = 1.8F}}};

atlas_err_t joints_manager_initialize(joints_manager_t* manager)
{
    assert(manager);

    manager->is_running = false;

    portENTER_CRITICAL();
    for (joint_num_t num = 0; num < JOINT_NUM; ++num) {
        joint_task_initialize(&joint_task_ctxs[num]);
        joint_queue_initialize(&joint_task_ctxs[num]);

        manager->joint_ctxs[num].task = joint_task_ctxs[num].task;
        manager->joint_ctxs[num].queue = joint_task_ctxs[num].queue;
    }
    portEXIT_CRITICAL();

    return ATLAS_ERR_OK;
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef* htim)
{
    // BaseType_t task_woken = pdFALSE;

    // if (htim->Instance == TIM1) {
    //     xTaskNotifyFromISR(joint_task_ctxs[JOINT_NUM_1].task,
    //                        JOINT_NOTIFY_PWM_PULSE,
    //                        eSetBits,
    //                        &task_woken);
    // }

    // portYIELD_FROM_ISR(task_woken);
}