#include "kinematics_manager.h"
#include "FreeRTOS.h"
#include "atlas_event.h"
#include "atlas_notify.h"
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

static inline void kinematics_manager_get_joints_event_update_payload(
    joints_event_payload_t* payload)
{
    payload->update[JOINT_NUM_1].position = 0.0F;
    payload->update[JOINT_NUM_1].delta_time = 0.01F;

    payload->update[JOINT_NUM_2].position = 0.0F;
    payload->update[JOINT_NUM_2].delta_time = 0.01F;

    payload->update[JOINT_NUM_3].position = 0.0F;
    payload->update[JOINT_NUM_3].delta_time = 0.01F;

    payload->update[JOINT_NUM_4].position = 0.0F;
    payload->update[JOINT_NUM_4].delta_time = 0.01F;

    payload->update[JOINT_NUM_5].position = 0.0F;
    payload->update[JOINT_NUM_5].delta_time = 0.01F;

    payload->update[JOINT_NUM_6].position = 0.0F;
    payload->update[JOINT_NUM_6].delta_time = 0.01F;
}

static atlas_err_t kinematics_manager_event_start_handler(kinematics_manager_t* manager)
{
    if (manager->is_running) {
        return ATLAS_ERR_ALREADY_RUNNING;
    }

    static joints_event_t event = {.type = JOINTS_EVENT_TYPE_START};
    if (kinematics_manager_send_joints_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    manager->is_running = true;

    return ATLAS_ERR_OK;
}

static atlas_err_t kinematics_manager_event_stop_handler(kinematics_manager_t* manager)
{
    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    static joints_event_t event = {.type = JOINTS_EVENT_TYPE_STOP};
    if (kinematics_manager_send_joints_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    manager->is_running = true;

    return ATLAS_ERR_OK;
}

static atlas_err_t kinematics_manager_event_update_handler(
    kinematics_manager_t* manager,
    kinematics_event_payload_t const* payload)
{
    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    static joints_event_t event = {.type = JOINTS_EVENT_TYPE_UPDATE};
    kinematics_manager_get_joints_event_update_payload(&event.payload);
    if (kinematics_manager_send_joints_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t kinematics_manager_notify_handler(kinematics_manager_t* manager,
                                                     kinematics_notify_t notify)
{
    return ATLAS_ERR_UNKNOWN_NOTIFY;
}

static atlas_err_t kinematics_manager_event_handler(kinematics_manager_t* manager,
                                                    kinematics_event_t const* event)
{
    return ATLAS_ERR_UNKNOWN_EVENT;
}

atlas_err_t kinematics_manager_process(kinematics_manager_t* manager)
{
    assert(manager);

    kinematics_notify_t notify;
    if (kinematics_manager_receive_kinematics_notify(&notify)) {
        atlas_err_t err = kinematics_manager_notify_handler(manager, notify);
        if (err != ATLAS_ERR_OK) {
            return err;
        }
    }

    kinematics_event_t event;
    if (kinematics_manager_receive_kinematics_event(&event)) {
        return kinematics_manager_event_handler(manager, &event);
    }

    return ATLAS_ERR_OK;
}

atlas_err_t kinematics_manager_initialize(kinematics_manager_t* manager)
{
    assert(manager);

    manager->is_running = false;

    kinematics_manager_event_start_handler(manager);

    return ATLAS_ERR_OK;
}
