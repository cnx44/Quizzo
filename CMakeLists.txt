cmake_minimum_required(VERSION 3.15)
project(quizzo)

set(CMAKE_C_STANDARD 11)

add_executable(quizzo src/main.c)
target_include_directories(quizzo  PRIVATE include)
