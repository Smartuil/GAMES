cmake_minimum_required(VERSION 3.27)
project(Homework1)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/local/include)

include_directories("/opt/homebrew/Cellar/eigen/3.4.0_1/include/")

add_executable(Homework1 main.cpp rasterizer.hpp rasterizer.cpp Triangle.hpp Triangle.cpp)
target_link_libraries(Homework1 ${OpenCV_LIBRARIES})
