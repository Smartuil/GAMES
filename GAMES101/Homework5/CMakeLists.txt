cmake_minimum_required(VERSION 3.27)
project(Homework5)

set(CMAKE_CXX_STANDARD 17)

add_executable(Homework5 main.cpp Object.hpp Vector.hpp Sphere.hpp global.hpp Triangle.hpp Scene.cpp Scene.hpp Light.hpp Renderer.cpp)
target_compile_options(Homework5 PUBLIC -Wall -Wextra -pedantic -Wshadow -Wreturn-type -fsanitize=undefined)
target_compile_features(Homework5 PUBLIC cxx_std_17)
target_link_libraries(Homework5 PUBLIC -fsanitize=undefined)