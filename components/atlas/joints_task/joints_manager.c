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

static char const* const TAG = "joints_manager";

static inline bool joints_manager_send_joint_event(QueueHandle_t queue, joint_event_t const* event)
{
    return xQueueSend(queue, event, pdMS_TO_TICKS(1)) == pdTRUE;
}

static inline bool joints_manager_send_joint_notify(TaskHandle_t task, joint_notify_t notify)
{
    return xTaskNotify(task, notify, eSetBits) == pdTRUE;
}

static inline bool joints_manager_send_kinematics_notify(kinematics_notify_t notify)
{
    return xTaskNotify(task_manager_get(TASK_TYPE_KINEMATICS), notify, eSetBits) == pdTRUE;
}

static inline bool joints_manager_receive_joints_event(joints_event_t* event)
{
    return xQueueReceive(queue_manager_get(QUEUE_TYPE_JOINTS), event, pdMS_TO_TICKS(1)) == pdTRUE;
}

static inline bool joints_manager_receive_joints_notify(joints_notify_t* notify)
{
    return xTaskNotifyWait(0, JOINTS_NOTIFY_ALL, (uint32_t*)notify, pdMS_TO_TICKS(1)) == pdTRUE;
}

static inline bool joints_manager_all_joints_ready(joints_manager_t const* manager)
{
    for (uint8_t num = 0; num < JOINT_NUM; ++num) {
        if (!manager->joint_ctxs[num].is_ready) {
            return false;
        }
    }

    return true;
}

static atlas_err_t joints_manager_event_start_handler(joints_manager_t* manager,
                                                      joints_event_payload_start_t const* payload)
{
    ATLAS_LOG(TAG, "joints_manager_event_start_handler");

    if (manager->is_running) {
        return ATLAS_ERR_ALREADY_RUNNING;
    }

    joint_event_t event = {.type = JOINT_EVENT_TYPE_START};
    for (uint8_t num = 0; num < JOINT_NUM; ++num) {
        if (!joints_manager_send_joint_event(manager->joint_ctxs[num].queue, &event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    HAL_TIM_Base_Start_IT(manager->delta_timer);

    manager->is_running = true;

    return ATLAS_ERR_OK;
}

static atlas_err_t joints_manager_event_stop_handler(joints_manager_t* manager,
                                                     joints_event_payload_stop_t const* payload)
{
    ATLAS_LOG(TAG, "joints_manager_event_stop_handler");

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    joint_event_t event = {.type = JOINT_EVENT_TYPE_STOP};
    for (uint8_t num = 0; num < JOINT_NUM; ++num) {
        if (!joints_manager_send_joint_event(manager->joint_ctxs[num].queue, &event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    HAL_TIM_Base_Stop_IT(manager->delta_timer);

    manager->is_running = false;

    return ATLAS_ERR_OK;
}

static atlas_err_t joints_manager_event_update_handler(joints_manager_t* manager,
                                                       joints_event_payload_update_t const* payload)
{
    ATLAS_LOG(TAG, "joints_manager_event_update_handler");

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    joint_event_t event = {.type = JOINT_EVENT_TYPE_UPDATE};
    for (uint8_t num = 0; num < JOINT_NUM; ++num) {
        event.payload.update.position = payload->positions[num];
        ATLAS_LOG(TAG, "num: %d, position: %f", num, event.payload.update.position);

        if (!joints_manager_send_joint_event(manager->joint_ctxs[num].queue, &event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t joints_manager_notify_joint_ready_handler(joints_manager_t* manager,
                                                             joint_num_t num)
{
    ATLAS_LOG(TAG, "joints_manager_notify_joint_ready_handler");

    manager->joint_ctxs[num].is_ready = true;

    if (joints_manager_all_joints_ready(manager)) {
        if (!joints_manager_send_kinematics_notify(KINEMATICS_NOTIFY_JOINTS_READY)) {
            return ATLAS_ERR_FAIL;
        }
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t joints_manager_notify_delta_timer_handler(joints_manager_t* manager)
{
    ATLAS_LOG(TAG, "joints_manager_notify_delta_timer_handler");

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    for (uint8_t num = 0; num < JOINT_NUM; ++num) {
        joints_manager_send_joint_notify(manager->joint_ctxs[num].task, JOINTS_NOTIFY_DELTA_TIMER);
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t joints_manager_notify_handler(joints_manager_t* manager, joints_notify_t notify)
{
    if (notify & JOINTS_NOTIFY_DELTA_TIMER) {
        ATLAS_RET_ON_ERR(joints_manager_notify_delta_timer_handler(manager));
    }
    if (notify & JOINTS_NOTIFY_JOINT_READY) {
        for (uint8_t num = 0; num < JOINT_NUM; ++num) {
            if (notify & (1 << num)) {
                ATLAS_RET_ON_ERR(joints_manager_notify_joint_ready_handler(manager, num));
            }
        }
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t joints_manager_event_handler(joints_manager_t* manager,
                                                joints_event_t const* event)
{
    switch (event->type) {
        case JOINTS_EVENT_TYPE_START:
            return joints_manager_event_start_handler(manager, &event->payload.start);
        case JOINTS_EVENT_TYPE_STOP:
            return joints_manager_event_stop_handler(manager, &event->payload.stop);
        case JOINTS_EVENT_TYPE_UPDATE:
            return joints_manager_event_update_handler(manager, &event->payload.update);
        default:
            return ATLAS_ERR_UNKNOWN_EVENT;
    }
}

atlas_err_t joints_manager_process(joints_manager_t* manager)
{
    assert(manager);

    joints_notify_t notify;
    if (joints_manager_receive_joints_notify(&notify)) {
        ATLAS_RET_ON_ERR(joints_manager_notify_handler(manager, notify));
    }

    joints_event_t event;
    if (joints_manager_receive_joints_event(&event)) {
        ATLAS_RET_ON_ERR(joints_manager_event_handler(manager, &event));
    }

    return ATLAS_ERR_OK;
}

atlas_err_t joints_manager_initialize(joints_manager_t* manager)
{
    assert(manager);

    manager->is_running = false;

    for (uint8_t num = 0; num < JOINT_NUM; ++num) {
        manager->joint_ctxs[num].is_ready = false;
    }

    return ATLAS_ERR_OK;
}

// void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef* htim)
// {
//     BaseType_t task_woken = pdFALSE;

//     if (htim->Instance == TIM1) {
//         xTaskNotifyFromISR(joint_task_ctxs[JOINT_NUM_1].task,
//                            JOINT_NOTIFY_PWM_PULSE,
//                            eSetBits,
//                            &task_woken);
//     }

//     portYIELD_FROM_ISR(task_woken);
// }