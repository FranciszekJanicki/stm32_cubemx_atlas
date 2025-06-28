#include "kinematics_manager.h"
#include "FreeRTOS.h"
#include "event.h"
#include "notify.h"
#include "queue_manager.h"
#include "task.h"
#include "task_manager.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static char const* const TAG = "kinematics_manager";

static inline bool kinematics_manager_send_joints_event(joints_event_t const* event)
{
    return xQueueSend(queue_manager_get(QUEUE_TYPE_JOINTS), event, pdMS_TO_TICKS(1)) == pdTRUE;
}

static inline bool kinematics_manager_send_joints_notify(joints_notify_t notify)
{
    return xTaskNotify(task_manager_get(TASK_TYPE_JOINTS), notify, eSetBits) == pdTRUE;
}

static inline bool kinematics_manager_receive_kinematics_event(kinematics_event_t* event)
{
    return xQueueReceive(queue_manager_get(QUEUE_TYPE_KINEMATICS), event, pdMS_TO_TICKS(1)) ==
           pdTRUE;
}

static inline bool kinematics_manager_receive_kinematics_notify(kinematics_notify_t* notify)
{
    return xTaskNotifyWait(0, KINEMATICS_NOTIFY_ALL, (uint32_t*)notify, pdMS_TO_TICKS(1)) == pdTRUE;
}

static inline void kinematics_manager_get_joints_event_payload_update(
    joints_event_payload_update_t* payload)
{
    payload->positions[JOINT_NUM_1] = 0.0F;
    payload->positions[JOINT_NUM_2] = 0.0F;
    payload->positions[JOINT_NUM_3] = 0.0F;
    payload->positions[JOINT_NUM_4] = 0.0F;
    payload->positions[JOINT_NUM_5] = 0.0F;
    payload->positions[JOINT_NUM_6] = 0.0F;
}

static error_t kinematics_manager_event_start_handler(
    kinematics_manager_t* manager,
    kinematics_event_payload_start_t const* payload)
{
    LOG(TAG, "kinematics_manager_event_start_handler");

    if (manager->is_running) {
        return ERROR_ALREADY_RUNNING;
    }

    joints_event_t event = {.type = JOINTS_EVENT_TYPE_START};
    // if (manager->is_joints_ready) {
    if (!kinematics_manager_send_joints_event(&event)) {
        return ERROR_FAIL;
    }
    // }

    manager->is_running = true;

    return ERROR_OK;
}

static error_t kinematics_manager_event_stop_handler(kinematics_manager_t* manager,
                                                     kinematics_event_payload_stop_t const* payload)
{
    LOG(TAG, "kinematics_manager_event_stop_handler");

    if (!manager->is_running) {
        return ERROR_NOT_RUNNING;
    }

    joints_event_t event = {.type = JOINTS_EVENT_TYPE_STOP};
    if (!kinematics_manager_send_joints_event(&event)) {
        return ERROR_FAIL;
    }

    manager->is_running = false;

    return ERROR_OK;
}

static error_t kinematics_manager_event_update_handler(
    kinematics_manager_t* manager,
    kinematics_event_payload_update_t const* payload)
{
    LOG(TAG, "kinematics_manager_event_update_handler");

    if (!manager->is_running) {
        return ERROR_NOT_RUNNING;
    }

    joints_event_t event = {.type = JOINTS_EVENT_TYPE_UPDATE};
    kinematics_manager_get_joints_event_payload_update(&event.payload.update);
    if (!kinematics_manager_send_joints_event(&event)) {
        return ERROR_FAIL;
    }

    return ERROR_OK;
}

static error_t kinematics_manager_notify_joints_ready_handler(kinematics_manager_t* manager)
{
    LOG(TAG, "kinematics_manager_notify_joints_ready_handler");

    manager->is_joints_ready = true;

    joints_event_t event = {.type = JOINTS_EVENT_TYPE_START};
    if (!kinematics_manager_send_joints_event(&event)) {
        return ERROR_FAIL;
    }

    return ERROR_OK;
}

static error_t kinematics_manager_notify_handler(kinematics_manager_t* manager,
                                                 kinematics_notify_t notify)
{
    if (notify & KINEMATICS_NOTIFY_JOINTS_READY) {
        error_t err = kinematics_manager_notify_joints_ready_handler(manager);
        if (err != ERROR_OK) {
            return err;
        }
    } else {
        return ERROR_UNKNOWN_NOTIFY;
    }

    return ERROR_OK;
}

static error_t kinematics_manager_event_handler(kinematics_manager_t* manager,
                                                kinematics_event_t const* event)
{
    switch (event->type) {
        case KINEMATICS_EVENT_TYPE_START: {
            return kinematics_manager_event_start_handler(manager, &event->payload.start);
        }
        case KINEMATICS_EVENT_TYPE_STOP: {
            return kinematics_manager_event_stop_handler(manager, &event->payload.stop);
        }
        case KINEMATICS_EVENT_TYPE_UPDATE: {
            return kinematics_manager_event_update_handler(manager, &event->payload.update);
        }
        default: {
            return ERROR_UNKNOWN_NOTIFY;
        }
    }
}

error_t kinematics_manager_process(kinematics_manager_t* manager)
{
    assert(manager);

    kinematics_notify_t notify;
    if (kinematics_manager_receive_kinematics_notify(&notify)) {
        RET_ON_ERROR(kinematics_manager_notify_handler(manager, notify));
    }

    kinematics_event_t event;
    if (kinematics_manager_receive_kinematics_event(&event)) {
        RET_ON_ERROR(kinematics_manager_event_handler(manager, &event));
    }

    return ERROR_OK;
}

error_t kinematics_manager_initialize(kinematics_manager_t* manager)
{
    assert(manager);

    manager->is_running = false;
    manager->is_joints_ready = false;

    kinematics_manager_event_start_handler(manager, NULL);

    return ERROR_OK;
}
