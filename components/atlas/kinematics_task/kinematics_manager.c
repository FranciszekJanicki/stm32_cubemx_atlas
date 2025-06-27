#include "kinematics_manager.h"
#include "FreeRTOS.h"
#include "joints_manager.h"
#include "kinematics_config.h"
#include "queue_manager.h"
#include "task.h"
#include "task_manager.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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

static kinematics_err_t kinematics_manager_event_start_handler(kinematics_manager_t* manager)
{
    if (manager->is_running) {
        return KINEMATICS_ERR_ALREADY_RUNNING;
    }

    static joints_event_t event = {.type = JOINTS_EVENT_TYPE_START};
    if (kinematics_manager_send_joints_event(&event)) {
        return KINEMATICS_ERR_FAIL;
    }

    manager->is_running = true;

    return KINEMATICS_ERR_OK;
}

static kinematics_err_t kinematics_manager_event_stop_handler(kinematics_manager_t* manager)
{
    if (!manager->is_running) {
        return KINEMATICS_ERR_NOT_RUNNING;
    }

    static joints_event_t event = {.type = JOINTS_EVENT_TYPE_STOP};
    if (kinematics_manager_send_joints_event(&event)) {
        return KINEMATICS_ERR_FAIL;
    }

    manager->is_running = true;

    return KINEMATICS_ERR_OK;
}

static kinematics_err_t kinematics_manager_event_update_handler(
    kinematics_manager_t* manager,
    kinematics_event_payload_t const* payload)
{
    if (!manager->is_running) {
        return KINEMATICS_ERR_NOT_RUNNING;
    }

    return KINEMATICS_ERR_OK;
}

static kinematics_err_t kinematics_manager_notify_handler(kinematics_manager_t* manager,
                                                          kinematics_notify_t notify)
{
    return KINEMATICS_ERR_UNKNOWN_NOTIFY;
}

static kinematics_err_t kinematics_manager_event_handler(kinematics_manager_t* manager,
                                                         kinematics_event_t const* event)
{
    return KINEMATICS_ERR_UNKNOWN_EVENT;
}

kinematics_err_t kinematics_manager_process(kinematics_manager_t* manager)
{
    assert(manager);

    kinematics_notify_t notify;
    if (kinematics_manager_receive_kinematics_notify(&notify)) {
        kinematics_err_t err = kinematics_manager_notify_handler(manager, notify);
        if (err != KINEMATICS_ERR_OK) {
            return err;
        }
    }

    kinematics_event_t event;
    if (kinematics_manager_receive_kinematics_event(&event)) {
        return kinematics_manager_event_handler(manager, &event);
    }

    return KINEMATICS_ERR_OK;
}

kinematics_err_t kinematics_manager_initialize(kinematics_manager_t* manager)
{
    assert(manager);

    manager->is_running = false;

    return KINEMATICS_ERR_OK;
}
