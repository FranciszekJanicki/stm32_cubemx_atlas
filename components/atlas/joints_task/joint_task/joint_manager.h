#ifndef JOINT_TASK_JOINT_MANAGER_H
#define JOINT_TASK_JOINT_MANAGER_H

#include "FreeRTOS.h"
#include "a4988.h"
#include "atlas_config.h"
#include "atlas_err.h"
#include "motor_driver.h"
#include "pid_regulator.h"
#include "queue.h"
#include "step_motor.h"
#include "task.h"
#include <stdbool.h>
#include <stdint.h>
#include <stm32l4xx_hal.h>

typedef struct {
    float32_t position;
    float32_t delta_time;
    bool is_running;

    a4988_t a4988;
    motor_driver_t driver;
    step_motor_t motor;
    pid_regulator_t regulator;

    GPIO_TypeDef* dir_port;
    uint32_t dir_pin;
    TIM_HandleTypeDef* pwm_timer;
    uint32_t pwm_channel;

    QueueHandle_t joint_queue;
    TaskHandle_t joint_task;
} joint_manager_t;

typedef struct {
    float32_t kp;
    float32_t ki;
    float32_t kd;
    float32_t kc;
    float32_t min_angle;
    float32_t max_angle;
    float32_t min_speed;
    float32_t max_speed;
    joint_num_t num;
} joint_config_t;

atlas_err_t joint_manager_initialize(joint_manager_t* manager, joint_config_t const* config);
atlas_err_t joint_manager_process(joint_manager_t* manager);

#endif // JOINT_TASK_JOINT_MANAGER_H