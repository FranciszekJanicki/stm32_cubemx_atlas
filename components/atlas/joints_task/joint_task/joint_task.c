#include "joint_task.h"
#include "FreeRTOS.h"
#include "joint_manager.h"
#include "task.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static void joint_task_func(void* param)
{
    joint_task_ctx_t* ctx = (joint_task_ctx_t*)param;

    ctx->manager.joint_queue = ctx->queue;
    joint_manager_initialize(&ctx->manager, &ctx->config);

    while (1) {
        joint_manager_process(&ctx->manager);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void joint_task_initialize(joint_task_ctx_t* ctx)
{
    ctx->task = xTaskCreateStatic(joint_task_func,
                                  "joint_task",
                                  JOINT_TASK_STACK_DEPTH,
                                  (void*)ctx,
                                  JOINT_TASK_PRIORITY,
                                  ctx->task_stack,
                                  &ctx->task_buffer);

    assert(ctx->task);
}

void joint_queue_initialize(joint_task_ctx_t* ctx)
{
    ctx->queue = xQueueCreateStatic(JOINT_QUEUE_ITEMS,
                                    JOINT_QUEUE_ITEM_SIZE,
                                    ctx->queue_storage,
                                    &ctx->queue_buffer);

    assert(ctx->queue);
}

#undef JOINT_TASK_STACK_DEPTH
#undef JOINT_TASK_PRIORITY

#undef JOINT_QUEUE_ITEMS
#undef JOINT_QUEUE_ITEM_SIZE
#undef JOINT_QUEUE_STORAGE_SIZE