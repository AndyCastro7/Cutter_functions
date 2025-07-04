idf_component_register(SRCS "state_machine.c"
                    INCLUDE_DIRS "include"
                    REQUIRES    "sensors"
                    REQUIRES    "uart_communication" 
                    REQUIRES    "control"
                    REQUIRES    "seed_dispenser")