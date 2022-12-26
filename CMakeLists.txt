cmake_minimum_required(VERSION 3.16)
project(Graphics)

add_compile_options(-g -Ofast -flto -Wall -Wextra -pedantic -ansi -std=c++17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")


# Source code directories
add_subdirectory(Source/Foundation)
add_subdirectory(Source/World_Creation)
add_subdirectory(Source/Parse)

# Testing directory
add_subdirectory(Test)
