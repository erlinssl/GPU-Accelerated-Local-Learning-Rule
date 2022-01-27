cmake_minimum_required(VERSION 3.21)
project(filter_finder)

set(CMAKE_CXX_STANDARD 20)

add_executable(filter_finder main.cpp Model.cpp Model.h Arrays.h Arrays.cpp)
