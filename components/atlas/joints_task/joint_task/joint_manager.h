#ifndef JOINT_TASK_JOINT_MANAGER_H
#define JOINT_TASK_JOINT_MANAGER_H

#include "FreeRTOS.h"
#include "a4988.h"
#include "joint_config.h"
#include "motor_driver.h"
#include "pid_regulator.h"
#include "queue.h"
#include "step_motor.h"
#include "task.h"
#include <stdint.h>
#include <stm32l4xx_hal.h>

typedef struct {
    float32_t position;
    float32_t delta_time;
    float32_t speed;
    bool is_running;

    a4988_t a4988;
    motor_driver_t driver;
    step_motor_t motor;
    pid_regulator_t regulator;

    GPIO_TypeDef* dir_port;
    uint32_t dir_pin;
    TIM_HandleTypeDef* delta_timer;
    TIM_HandleTypeDef* pwm_timer;
    uint32_t pwm_channel;

    QueueHandle_t joint_queue;
} joint_manager_t;

joint_err_t joint_manager_initialize(joint_manager_t* manager, joint_config_t const* config);
joint_err_t joint_manager_process(joint_manager_t* manager);

#endif // JOINT_TASK_JOINT_MANAGER_H