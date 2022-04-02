cmake_minimum_required(VERSION 3.18)
project(filter_finder)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -O3")
set(SOURCE_FILES main.cpp)

find_package (Python3 COMPONENTS Development NumPy)
find_package (OpenCL REQUIRED)

add_definitions(-w) # TODO Remove

add_executable(filter_finder main.cpp Model.cpp Arrays.cpp)

configure_file(data/train-images-idx3-ubyte trainingdata COPYONLY)

find_package(PythonLibs REQUIRED)
include_directories(${Python3_NumPy_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(${BoostCompute_INCLUDE_DIRS})


target_link_libraries(filter_finder OpenCL::OpenCL)
target_link_libraries(filter_finder ${PYTHON_LIBRARIES})
target_link_libraries(filter_finder ${Python3_NumPy_INCLUDE_DIRS})
