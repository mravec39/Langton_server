cmake_minimum_required(VERSION 3.18)
project(Langton_server C)

set(CMAKE_C_STANDARD 11)

add_executable(Langton_server main.c)
target_link_libraries(Langton_server)
