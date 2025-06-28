#include "joint_task.h"
#include "FreeRTOS.h"
#include "joint_manager.h"
#include "task.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

char const* const TAG = "joint_task";

static void joint_task_func(void* task_param)
{
    joint_task_ctx_t* task_ctx = (joint_task_ctx_t*)task_param;

    joint_manager_initialize(&task_ctx->manager, &task_ctx->config);

    while (1) {
        //   ATLAS_LOG_ON_ERR(TAG, joint_manager_process(&task_ctx->manager));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

TaskHandle_t joint_task_initialize(joint_task_ctx_t* task_ctx,
                                   StaticTask_t* task_buffer,
                                   StackType_t (*task_stack)[JOINT_TASK_STACK_DEPTH])
{
    assert(task_ctx && task_buffer && task_stack);

    task_ctx->manager.joint_task = xTaskCreateStatic(joint_task_func,
                                                     "joint_task",
                                                     JOINT_TASK_STACK_DEPTH,
                                                     (void*)task_ctx,
                                                     JOINT_TASK_PRIORITY,
                                                     *task_stack,
                                                     task_buffer);

    assert(task_ctx->manager.joint_task);

    return task_ctx->manager.joint_task;
}

QueueHandle_t joint_queue_initialize(joint_task_ctx_t* task_ctx,
                                     StaticQueue_t* queue_buffer,
                                     uint8_t (*queue_storage)[JOINT_QUEUE_STORAGE_SIZE])
{
    assert(task_ctx && queue_buffer && queue_storage);

    task_ctx->manager.joint_queue =
        xQueueCreateStatic(JOINT_QUEUE_ITEMS, JOINT_QUEUE_ITEM_SIZE, *queue_storage, queue_buffer);

    assert(task_ctx->manager.joint_queue);

    return task_ctx->manager.joint_queue;
}

#undef JOINT_TASK_STACK_DEPTH
#undef JOINT_TASK_PRIORITY

#undef JOINT_QUEUE_ITEMS
#undef JOINT_QUEUE_ITEM_SIZE
#undef JOINT_QUEUE_STORAGE_SIZE