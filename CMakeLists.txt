idf_component_register(SRCS "src/setup.c" "src/io.c" "src/compensation.c" "src/read.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "esp_driver_i2c")
