#include "joints_manager.h"
#include "FreeRTOS.h"
#include "gpio.h"
#include "joint_manager.h"
#include "joint_task.h"
#include "queue.h"
#include "queue_manager.h"
#include "task.h"
#include "task_manager.h"
#include "tim.h"
#include "usart.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static joints_err_t joints_manager_event_start_handler(joints_manager_t* manager)
{
    if (manager->is_running) {
        return JOINTS_ERR_ALREADY_RUNNING;
    }

    for (joint_num_t num = 0; num < JOINT_NUM; ++num) {
        manager->joints[num].event.type = JOINT_EVENT_TYPE_START;
        xQueueSend(manager->joints[num].queue, &manager->joints[num].event, pdMS_TO_TICKS(1));
    }

    manager->is_running = true;

    return JOINTS_ERR_OK;
}

static joints_err_t joints_manager_event_stop_handler(joints_manager_t* manager)
{
    if (!manager->is_running) {
        return JOINTS_ERR_NOT_RUNNING;
    }

    for (joint_num_t num = 0; num < JOINT_NUM; ++num) {
        manager->joints[num].event.type = JOINT_EVENT_TYPE_STOP;
        xQueueSend(manager->joints[num].queue, &manager->joints[num].event, pdMS_TO_TICKS(1));
    }

    manager->is_running = false;

    return JOINTS_ERR_OK;
}

static joints_err_t joints_manager_event_update_handler(joints_manager_t* manager,
                                                        joints_event_payload_t const* payload)
{
    if (!manager->is_running) {
        return JOINTS_ERR_NOT_RUNNING;
    }

    for (joint_num_t num = 0; num < JOINT_NUM; ++num) {
        if (manager->joints[num].event.payload.update.position != payload->update[num].position) {
            manager->joints[num].event.type = JOINT_EVENT_TYPE_UPDATE;
            manager->joints[num].event.payload.update.position = payload->update[num].position;
            manager->joints[num].event.payload.update.delta_time = 0.01F;
            xQueueSend(manager->joints[num].queue, &manager->joints[num].event, pdMS_TO_TICKS(1));
        }
    }

    return JOINTS_ERR_OK;
}

static joints_err_t joints_manager_notify_delta_timer_handler(joints_manager_t* manager)
{
    if (!manager->is_running) {
        return JOINTS_ERR_NOT_RUNNING;
    }

    for (joint_num_t num = 0; num < JOINT_NUM; ++num) {
        xTaskNotify(manager->joints[num].task, JOINTS_NOTIFY_DELTA_TIMER, eSetBits);
    }

    return JOINTS_ERR_OK;
}

static joints_err_t joints_manager_notify_handler(joints_manager_t* manager, joints_notify_t notify)
{
    if (notify & JOINTS_NOTIFY_DELTA_TIMER) {
        joints_err_t err = joints_manager_notify_delta_timer_handler(manager);
        if (err != JOINTS_ERR_OK) {
            return err;
        }
    }

    return JOINTS_ERR_UNKNOWN_NOTIFY;
}

static joints_err_t joints_manager_event_handler(joints_manager_t* manager,
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

    return JOINTS_ERR_UNKNOWN_EVENT;
}

joints_err_t joints_manager_process(joints_manager_t* manager)
{
    assert(manager);

    joints_err_t err;

    uint32_t notify;
    if (xTaskNotifyWait(0, JOINTS_NOTIFY_ALL, &notify, pdMS_TO_TICKS(1))) {
        err = joints_manager_notify_handler(manager, notify);
        if (err != JOINT_ERR_OK) {
            return err;
        }
    }

    joints_event_t event;
    if (xQueueReceive(queue_manager_get(QUEUE_TYPE_JOINTS), &event, pdMS_TO_TICKS(1))) {
        err = joints_manager_event_handler(manager, &event);
        if (err != JOINT_ERR_OK) {
            return err;
        }
    }

    return err;
}

joints_err_t joints_manager_initialize(joints_manager_t* manager, joints_config_t* config)
{
    assert(manager && config);

    manager->is_running = false;

    for (joint_num_t num = 0; num < JOINT_NUM; ++num) {
        joint_task_initialize(&config->joints_ctxs[num]);
        manager->joints[num].task = config->joints_ctxs[num].task;

        joint_queue_initialize(&config->joints_ctxs[num]);
        manager->joints[num].queue = config->joints_ctxs[num].queue;
    }

    return JOINTS_ERR_OK;
}
