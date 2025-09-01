add_library(driver
    driver.cpp
)

target_include_directories(driver
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(driver
    PUBLIC
    pico_stdlib
    hardware_gpio
    hardware_spi
    hardware_i2c
    hardware_pwm
    hardware_adc
    hardware_timer
)
