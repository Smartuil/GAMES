cmake_minimum_required(VERSION 3.27)
project(Homework4)

find_package(OpenCV REQUIRED)

include_directories(/usr/local/include)

set(CMAKE_CXX_STANDARD 17)

add_executable(Homework4 main.cpp)

target_link_libraries(Homework4 ${OpenCV_LIBRARIES})