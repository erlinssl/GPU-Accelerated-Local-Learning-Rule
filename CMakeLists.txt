cmake_minimum_required(VERSION 3.18)
project(filter_finder)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpython3.10 -I/usr/include/python3.10")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/home/erlinssl/.local/lib/python3.9/site-packages/numpy/core/include/")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/home/ingebrigt/.local/lib/python3.9/site-packages/numpy/core/include")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lpython3.9 -I/usr/bin/python3.9")
set(SOURCE_FILES main.cpp)

add_definitions(-w) # TODO Remove

add_executable(filter_finder main.cpp Model.cpp Arrays.cpp)

configure_file(data/train-images-idx3-ubyte trainingdata COPYONLY)
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
add_library(CPy SHARED ${SOURCE_FILES})
target_link_libraries(CPy ${PYTHON_LIBRARIES})
