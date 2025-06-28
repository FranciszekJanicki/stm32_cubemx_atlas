#ifndef JOINT_TASK_JOINT_TASK_H
#define JOINT_TASK_JOINT_TASK_H

#include "atlas_event.h"
#include "joint_manager.h"

#define JOINT_TASK_STACK_DEPTH (1024U / sizeof(StackType_t))
#define JOINT_TASK_PRIORITY (2U)

#define JOINT_QUEUE_ITEMS (100U)
#define JOINT_QUEUE_ITEM_SIZE (sizeof(joint_event_t))
#define JOINT_QUEUE_STORAGE_SIZE (JOINT_QUEUE_ITEMS * JOINT_QUEUE_ITEM_SIZE)

typedef struct {
    joint_manager_t manager;
    joint_config_t config;
    TaskHandle_t task;
    QueueHandle_t queue;
} joint_task_ctx_t;

typedef struct {
    StackType_t task_stack[JOINT_TASK_STACK_DEPTH];
    StaticTask_t task_buffer;
} joint_task_config_t;

typedef struct {
    uint8_t queue_storage[JOINT_QUEUE_STORAGE_SIZE];
    StaticQueue_t queue_buffer;
} joint_queue_config_t;

TaskHandle_t joint_task_initialize(joint_task_ctx_t* ctx, joint_task_config_t* config);
QueueHandle_t joint_queue_initialize(joint_task_ctx_t* ctx, joint_queue_config_t* config);

#endif // JOINT_TASK_JOINT_TASK_H