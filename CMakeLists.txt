cmake_minimum_required(VERSION 3.23)
project(goom C)

set(CMAKE_C_STANDARD 99)

add_executable(goom main.c structs.h)

target_link_libraries(goom -lOpenGL32 -lGLU32 -lfreeGLUT -lglew32)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/map
        ${CMAKE_CURRENT_BINARY_DIR}/map)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/map.bin
        ${CMAKE_CURRENT_BINARY_DIR}/demo.bin)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/libs
        ${CMAKE_CURRENT_BINARY_DIR})
