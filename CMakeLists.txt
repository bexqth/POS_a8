cmake_minimum_required(VERSION 3.18)
project(uloha8 C)

set(CMAKE_C_STANDARD 11)

add_executable(uloha8 main.c)
target_link_libraries(uloha8 pthread)
