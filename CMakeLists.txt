cmake_minimum_required(VERSION 3.23)
project(binary_space_partitioning C)

set(CMAKE_C_STANDARD 99)

add_executable(binary_space_partitioning main.c)

target_link_libraries(binary_space_partitioning -lOpenGL32 -lfreeGLUT -lGLU32)
