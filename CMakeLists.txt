cmake_minimum_required(VERSION 3.22.1)
project(CPU_usage_tracker LANGUAGES C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED On)
set(CMAKE_C_EXTENSIONS Off)

if("$ENV{CC}" MATCHES "clang")
  set(CMAKE_C_COMPILER clang)
  set(CMAKE_C_FLAGS "-Weverything")
elseif ("$ENV{CC}" MATCHES "gcc")
  set(CMAKE_C_COMPILER gcc)
  set(CMAKE_C_FLAGS "-ggdb3 -Wextra")
endif()

# set(CMAKE_THREAD_LIBS_INIT)
# set(MAKE_USE_PTHREADS_INIT)
add_library(
    src
    reader.c
    proc_stat_database.c
    circular_buffer.c
    printer.c
    analyzer.c
    logger.c
)
add_executable(cpuTracker main.c)
add_executable(unitTests test.c)
target_link_libraries(cpuTracker PRIVATE src)
target_link_libraries(unitTests PRIVATE src)
