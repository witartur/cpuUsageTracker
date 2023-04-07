cmake_minimum_required(VERSION 3.22.1)
project("CPU_usage_tracker")

add_library(
    src
    reader.c
    proc_stat_database.c
)
add_executable(cpuTracker.elf main.c)