cmake_minimum_required(VERSION 3.21)
project(filter_finder)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -I/usr/include/python3.10 -lpython3.10")
set(SOURCE_FILES main.cpp)

add_executable(filter_finder main.cpp Model.cpp Arrays.cpp)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
add_library(CPy SHARED ${SOURCE_FILES})
target_link_libraries(CPy ${PYTHON_LIBRARIES})
