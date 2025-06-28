#include "kinematics_manager.h"
#include "FreeRTOS.h"
#include "atlas_event.h"
#include "atlas_notify.h"
#include "queue_manager.h"
#include "task.h"
#include "task_manager.h"
#include "utility.h"
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

static inline cartesian_space_t kinematics_manager_joint_space_to_cartesian_space(
    joint_space_t const* joint_space)
{
    return (cartesian_space_t){.tool_position = {.x = 0.0F, .y = 0.0F, .z = 0.0F},
                               .tool_orientation = {.x = 0.0F, .y = 0.0F, .z = 0.0F}};
}

static inline joint_space_t kinematics_manager_cartesian_space_to_joint_space(
    cartesian_space_t const* cartesian_space)
{
    return (joint_space_t){.joint_position = {10.0F, 10.0F, 10.0F, 10.0F, 10.0F, 10.0F}};
}

static atlas_err_t kinematics_manager_event_start_handler(
    kinematics_manager_t* manager,
    kinematics_event_payload_start_t const* payload)
{
    LOG(TAG, "kinematics_manager_event_start_handler");

    if (manager->is_running) {
        return ATLAS_ERR_ALREADY_RUNNING;
    }

    joints_event_t event = {.type = JOINTS_EVENT_TYPE_START};
    if (manager->is_joints_ready) {
        if (!kinematics_manager_send_joints_event(&event)) {
            return ATLAS_ERR_FAIL;
        }
    }

    manager->is_running = true;

    return ATLAS_ERR_OK;
}

static atlas_err_t kinematics_manager_event_stop_handler(
    kinematics_manager_t* manager,
    kinematics_event_payload_stop_t const* payload)
{
    LOG(TAG, "kinematics_manager_event_stop_handler");

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    joints_event_t event = {.type = JOINTS_EVENT_TYPE_STOP};
    if (!kinematics_manager_send_joints_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    manager->is_running = false;

    return ATLAS_ERR_OK;
}

static atlas_err_t kinematics_manager_event_direct_handler(
    kinematics_manager_t* manager,
    kinematics_event_payload_direct_t const* payload)
{
    LOG(TAG, "kinematics_manager_event_direct_handler");

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    cartesian_space_t cartesian_space =
        kinematics_manager_joint_space_to_cartesian_space(&payload->joint_space);

    LOG(TAG,
        "position x: %f, y: %f, z: %f",
        cartesian_space.tool_position.x,
        cartesian_space.tool_position.y,
        cartesian_space.tool_position.z);

    LOG(TAG,
        "orientation x: %f, y: %f, z: %f",
        cartesian_space.tool_orientation.x,
        cartesian_space.tool_orientation.y,
        cartesian_space.tool_orientation.z);

    return ATLAS_ERR_OK;
}

static atlas_err_t kinematics_manager_event_inverse_handler(
    kinematics_manager_t* manager,
    kinematics_event_payload_inverse_t const* payload)
{
    LOG(TAG, "kinematics_manager_event_inverse_handler");

    if (!manager->is_running) {
        return ATLAS_ERR_NOT_RUNNING;
    }

    joints_event_t event = {.type = JOINTS_EVENT_TYPE_UPDATE};
    joint_space_t joint_space =
        kinematics_manager_cartesian_space_to_joint_space(&payload->cartesian_space);
    event.payload.update.joint_space = joint_space;

    LOG(TAG,
        "joint position 1: %f, 2: %f, 3: %f, 4: %f, 5: %f, 6: %f",
        joint_space.joint_position[JOINT_NUM_1],
        joint_space.joint_position[JOINT_NUM_2],
        joint_space.joint_position[JOINT_NUM_3],
        joint_space.joint_position[JOINT_NUM_4],
        joint_space.joint_position[JOINT_NUM_5],
        joint_space.joint_position[JOINT_NUM_6]);

    if (!kinematics_manager_send_joints_event(&event)) {
        return ATLAS_ERR_FAIL;
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t kinematics_manager_notify_joints_ready_handler(kinematics_manager_t* manager)
{
    LOG(TAG, "kinematics_manager_notify_joints_ready_handler");

    manager->is_joints_ready = true;

    kinematics_manager_event_start_handler(manager, NULL);

    return ATLAS_ERR_OK;
}

static atlas_err_t kinematics_manager_notify_handler(kinematics_manager_t* manager,
                                                     kinematics_notify_t notify)
{
    if (notify & KINEMATICS_NOTIFY_JOINTS_READY) {
        RET_ON_ERR(kinematics_manager_notify_joints_ready_handler(manager));
    }

    return ATLAS_ERR_OK;
}

static atlas_err_t kinematics_manager_event_handler(kinematics_manager_t* manager,
                                                    kinematics_event_t const* event)
{
    switch (event->type) {
        case KINEMATICS_EVENT_TYPE_START:
            return kinematics_manager_event_start_handler(manager, &event->payload.start);
        case KINEMATICS_EVENT_TYPE_STOP:
            return kinematics_manager_event_stop_handler(manager, &event->payload.stop);
        case KINEMATICS_EVENT_TYPE_DIRECT:
            return kinematics_manager_event_direct_handler(manager, &event->payload.direct);
        case KINEMATICS_EVENT_TYPE_INVERSE:
            return kinematics_manager_event_inverse_handler(manager, &event->payload.inverse);
        default:
            return ATLAS_ERR_UNKNOWN_EVENT;
    }
}

atlas_err_t kinematics_manager_process(kinematics_manager_t* manager)
{
    assert(manager);

    kinematics_notify_t notify;
    if (kinematics_manager_receive_kinematics_notify(&notify)) {
        RET_ON_ERR(kinematics_manager_notify_handler(manager, notify));
    }

    kinematics_event_t event;
    if (kinematics_manager_receive_kinematics_event(&event)) {
        RET_ON_ERR(kinematics_manager_event_handler(manager, &event));
    }

    // // test purposes only
    // vTaskDelay(100);
    // kinematics_manager_event_inverse_handler(manager, NULL);

    return ATLAS_ERR_OK;
}

atlas_err_t kinematics_manager_initialize(kinematics_manager_t* manager)
{
    assert(manager);

    manager->is_running = false;
    manager->is_joints_ready = false;

    return ATLAS_ERR_OK;
}
