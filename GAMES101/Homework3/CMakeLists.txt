cmake_minimum_required(VERSION 3.27)
project(Homework3)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/local/include)

include_directories("/opt/homebrew/Cellar/eigen/3.4.0_1/include/")

add_executable(Homework3 main.cpp rasterizer.hpp rasterizer.cpp global.hpp Triangle.hpp Triangle.cpp Texture.hpp Texture.cpp Shader.hpp OBJ_Loader.h)

target_link_libraries(Homework3 ${OpenCV_LIBRARIES})

#target_compile_options(Homework2 PUBLIC -Wall -Wextra -pedantic)
