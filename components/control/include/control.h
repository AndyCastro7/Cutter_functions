
#ifndef CONTROL_H
#define CONTROL_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_err.h"

#include "motors.h"


#define PWM_OFF                     0
#define PWM_LINEAR_MOTOR            50
#define PWM_CUTTER_MOTOR            100


/**
 * @brief Init cutter tasks
 * 
 * This function initialize cutter motors and endstop sensors.
 * 
 * @retval ESP_OK: motors initialization completed.
 * @retval ESP_FAIL: motors initialization failed.
 */
esp_err_t init_cutter_motors();

/**
 * @brief Stop cutter motors
 * 
 * This function disable the cutter motors.
 * 
 * Linear motors
 * Cutter motor
 * 
 * @retval ESP_OK: Cutter motors stopped.
 * @retval ESP_FAIL: Cutter motors failed to stop.
 */
esp_err_t stop_cutter();

/**
 * @brief Start cutter motors
 * 
 * This function enable the cutter motors.
 * 
 * Linear motors.
 * Cutter motor.
 * 
 * @retval ESP_OK: Cutter motors started.
 * @retval ESP_FAIL: Cutter motors failed to start.
 */
esp_err_t start_cutter();

/**
 * @brief Raise the linear motor effector.
 * 
 * This function set the linear motor in up position.
 * 
 * @retval ESP_OK: raising state.
 * @retval ESP_FAIL: failed to reach raising state.
 */
esp_err_t raise_effector();

/**
 * @brief Lower the linear motor effector.
 * 
 * This function set the linear motor in down position.
 * 
 * @retval ESP_OK: Lower state. 
 * @retval ESP_FAIL Failed to reach lower state.
 */
esp_err_t lower_effector();

/**
 * @brief Lower the linear motor effector
 * 
 * This function disable the linear motor.
 * 
 * @retval ESP_OK: Stop state.
 * @retval ESP_FAIL: Failed to reach stop state
 */
esp_err_t stop_effector();


/**
 * @brief Test rpms
 * 
 * This function test the motor at PWM level. 
 * 
 * @retval ESP_OK: Test initialized.
 * @retval ESP_FAIL: Test failed to initialize.
 */
esp_err_t test_rpms();

/**
 * @brief Stop test rpms
 * 
 * This function stop the motor (put in PWM to zero).
 * 
 * @retval ESP_OK: Test stopped.
 * @retval ESP_FAIL: Test failed to stop
 */
esp_err_t stop_test_rpms();

#endif