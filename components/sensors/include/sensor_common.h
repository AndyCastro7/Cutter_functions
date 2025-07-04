#ifndef SENSOR_COMMON_H
#define SENSOR_COMMON_H

#define PIN_TRIGGER             GPIO_NUM_12
#define PIN_ECHO                GPIO_NUM_13
#define PIN_AM2308              GPIO_NUM_14

// #define ULTRASONIC_SENSOR_ENABLED
// #define AM2308_SENSOR_ENABLED

#if !defined(ULTRASONIC_SENSOR_ENABLED) && !defined(AM2308_SENSOR_ENABLED)
    #warning "Sensors disabled"
#endif

#endif