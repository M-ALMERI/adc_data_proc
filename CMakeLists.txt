cmake_minimum_required(VERSION 3.20)
project(adc_binary_log_inspector C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(adc_binary_log_inspector
        main.c
        adc.c
        io.c
        stats.c)

if (UNIX)
    target_link_libraries(adc_binary_log_inspector m)
endif()