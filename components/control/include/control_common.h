/* Settting global configuration for control, motors and mypid  */
#ifndef CONTROL_COMMON_H
#define CONTROL_COMMON_H


// PIN behaviour for motor control

#define CUTTER_PWM_A                GPIO_NUM_4
#define CUTTER_PWM_B                GPIO_NUM_5
#define CUTTER_ENA_A                GPIO_NUM_6
#define CUTTER_ENA_B                GPIO_NUM_7

#define LINEAR_MOTOR_PWM_A          GPIO_NUM_18
#define LINEAR_MOTOR_PWM_B          GPIO_NUM_17
#define LINEAR_MOTOR_ENA_A          GPIO_NUM_8
#define LINEAR_MOTOR_ENA_B          GPIO_NUM_9
#define LINEAR_SENSOR_UP            GPIO_NUM_10
#define LINEAR_SENSOR_DW            GPIO_NUM_11


#define PWM_CONTROL 


#endif