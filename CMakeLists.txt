cmake_minimum_required(VERSION 3.22.1)
project(CPU_usage_tracker LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED On)
set(CMAKE_C_EXTENSIONS Off)

add_library(
    src
    reader.c
    proc_stat_database.c
    circular_buffer.c
    printer.c
)
add_executable(cpuTracker main.c)
target_link_libraries(cpuTracker PRIVATE src)
