idf_component_register(SRCS "motors.c" "control.c"
                    INCLUDE_DIRS "include"
                    REQUIRES     "driver"
                    REQUIRES     "esp_timer"
                    REQUIRES     "bdc_motor")
