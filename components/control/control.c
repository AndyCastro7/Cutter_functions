#include "control.h"
#include "control_common.h"
#include "esp_log.h"
#include "esp_check.h"

static const char TAG[] = "Motor control";

motors_vars_t cutter_motor = {
    .ena_A = CUTTER_ENA_A,
    .ena_B = CUTTER_ENA_B,
    .group_id = 0,
    .pwm_A = CUTTER_PWM_A,
    .pwm_B = CUTTER_PWM_B,
};

motors_vars_t linear_motor = {
    .ena_A = LINEAR_MOTOR_ENA_A,
    .ena_B = LINEAR_MOTOR_ENA_B,
    .group_id = 1,
    .pwm_A = LINEAR_MOTOR_PWM_A,
    .pwm_B = LINEAR_MOTOR_PWM_B,
};

static QueueHandle_t endstop_up_queue = NULL;
static QueueHandle_t endstop_dw_queue = NULL;


/**
 * @brief Wait for rising edge in endstop.
 * 
 * @param[in] endstop_queue: Queue from endstop interruption handler.
 * 
 * @retval ESP_OK: Endstop in true state.
 * @retval ESP_FAIL: Timeout. 
 * 
 */
static esp_err_t verify_endstop(QueueHandle_t endstop_queue);

/**
 * @brief Initializes endstop sensor.
 * 
 * @param[in] endstop: GPIO pin for endstop.
 * @param[in] isr_handler: handler for gpio interruption.
 */
static void init_endstop(gpio_num_t endstop, gpio_isr_t isr_handler);

/**
 * @brief Endstop interrumption handler
 * 
 * @param[in] args: GPIO_PIN that cause interruption
 */
static void endstop_intrr_handler(void *args);


esp_err_t init_cutter_motors()
{
    ESP_LOGI(TAG, "Cutter is initializing...");
    // -- motor cutter --
    init_motor(&cutter_motor);
    ESP_ERROR_CHECK(bdc_motor_enable(cutter_motor.motor_handler));
    ESP_ERROR_CHECK(bdc_motor_forward(cutter_motor.motor_handler));
    ESP_ERROR_CHECK(bdc_motor_set_speed(cutter_motor.motor_handler, (uint32_t)BDC_DUTY_CYCLE(PWM_OFF)));

    ESP_LOGI(TAG, "Linear motor is initializing...");
    // -- linear motor --
    init_motor(&linear_motor);
    ESP_ERROR_CHECK(bdc_motor_enable(linear_motor.motor_handler));
    ESP_ERROR_CHECK(bdc_motor_forward(linear_motor.motor_handler));
    ESP_ERROR_CHECK(bdc_motor_set_speed(linear_motor.motor_handler, (uint32_t)BDC_DUTY_CYCLE(PWM_OFF)));

    // -- linear sensors
    ESP_LOGI(TAG, "Linear sensors are initializing...");
    endstop_up_queue = xQueueCreate(5, sizeof(int));    
    endstop_dw_queue = xQueueCreate(5, sizeof(int));
    
    gpio_install_isr_service(0);
    init_endstop(LINEAR_SENSOR_UP, endstop_intrr_handler);
    init_endstop(LINEAR_SENSOR_DW, endstop_intrr_handler);

    return ESP_OK;
}

esp_err_t start_cutter()
{
#ifdef PWM_CONTROL
    enable_pins_bts7960(cutter_motor.ena_A, cutter_motor.ena_B);
    enable_pins_bts7960(linear_motor.ena_A, linear_motor.ena_B);
    ESP_ERROR_CHECK(bdc_motor_set_speed(cutter_motor.motor_handler, BDC_DUTY_CYCLE(PWM_CUTTER_MOTOR)));
    return lower_effector();
#endif
    
}

esp_err_t stop_cutter()
{
#ifdef PWM_CONTROL
    if(raise_effector() == ESP_OK)
    {
        stop_effector();
        disable_pins_bts7960(cutter_motor.ena_A, cutter_motor.ena_B);
        ESP_ERROR_CHECK(bdc_motor_set_speed(cutter_motor.motor_handler, BDC_DUTY_CYCLE(PWM_OFF)));
        
        return ESP_OK;
    }

    return ESP_FAIL;
    
#endif

}

esp_err_t raise_effector()
{
    // verify the state before to turn on the motor
    if (gpio_get_level(LINEAR_SENSOR_UP)){
        ESP_LOGI(TAG, "Linear motor is already in up state...");
        return ESP_OK;
    }

    // reset the endstop queue
    xQueueReset(endstop_up_queue);

    // turn on the linear motors
    ESP_LOGI(TAG, "Linear motor up state...");
    enable_pins_bts7960(linear_motor.ena_A, linear_motor.ena_B);
    ESP_ERROR_CHECK(bdc_motor_forward(linear_motor.motor_handler));
    ESP_ERROR_CHECK(bdc_motor_set_speed(linear_motor.motor_handler, BDC_DUTY_CYCLE(PWM_LINEAR_MOTOR)));

    // return response 
    return verify_endstop(endstop_up_queue);
}

esp_err_t lower_effector()
{
    // verify the state before to turn on the motor 
    if (gpio_get_level(LINEAR_SENSOR_DW)){
        ESP_LOGI(TAG, "Linear motor is already in down state...");
        return ESP_OK;
    }

    // reset the endstop queue 
    xQueueReset(endstop_dw_queue);

    ESP_LOGI(TAG, "Linear motor down state...");
    enable_pins_bts7960(linear_motor.ena_A, linear_motor.ena_B);
    ESP_ERROR_CHECK(bdc_motor_reverse(linear_motor.motor_handler));
    ESP_ERROR_CHECK(bdc_motor_set_speed(linear_motor.motor_handler, BDC_DUTY_CYCLE(PWM_LINEAR_MOTOR)));

    // return response 
    return verify_endstop(endstop_dw_queue);
}

esp_err_t stop_effector()
{
    ESP_LOGI(TAG, "Linear motor stop state...");
    disable_pins_bts7960(linear_motor.ena_A, linear_motor.ena_B);
    ESP_ERROR_CHECK(bdc_motor_set_speed(linear_motor.motor_handler, BDC_DUTY_CYCLE(PWM_OFF)));

    return ESP_OK;
}

esp_err_t test_rpms()
{
    ESP_LOGI(TAG, "Starting RPM test..");
    enable_pins_bts7960(cutter_motor.ena_A, cutter_motor.ena_B);
    ESP_ERROR_CHECK(bdc_motor_set_speed(cutter_motor.motor_handler, BDC_DUTY_CYCLE(PWM_CUTTER_MOTOR)));

    return ESP_OK;
}

esp_err_t stop_test_rpms()
{
    ESP_LOGI(TAG, "Stoping RPM test...");
    disable_pins_bts7960(cutter_motor.ena_A, cutter_motor.ena_B);
    ESP_ERROR_CHECK(bdc_motor_set_speed(cutter_motor.motor_handler, BDC_DUTY_CYCLE(PWM_OFF)));
    
    return ESP_OK;
}


//-----------------------------------------------------------------------------------------//
//------------------------------    ENDSTOP FUNCTIONS   -----------------------------------//
//-----------------------------------------------------------------------------------------//

static void init_endstop(gpio_num_t endstop, gpio_isr_t isr_handler)
{
    // set pin input intrr
    gpio_set_direction(endstop, GPIO_MODE_INPUT);
    gpio_pulldown_en(endstop);
    gpio_set_intr_type(endstop, GPIO_INTR_POSEDGE);

    // set isr handler 
    gpio_isr_handler_add(endstop, isr_handler, (void *)endstop);

}

static esp_err_t verify_endstop(QueueHandle_t endstop_queue)
{
    int info = 0;
    if(xQueueReceive(endstop_queue, &info, portMAX_DELAY))
    {
        return ESP_OK;
    }

    return ESP_FAIL;
}

static void IRAM_ATTR endstop_intrr_handler(void *args)
{
    int which_endstop = (int)args;

    if(which_endstop == LINEAR_SENSOR_UP){
       xQueueSendFromISR(endstop_up_queue, &which_endstop, NULL);
    }else if(which_endstop == LINEAR_SENSOR_DW)
    {
        xQueueSendFromISR(endstop_dw_queue, &which_endstop, NULL);
    }

}
