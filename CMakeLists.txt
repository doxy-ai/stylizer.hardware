cmake_minimum_required(VERSION 3.21)
project(stylizer.hardware C CXX)

set(CMAKE_CXX_STANDARD 23)

set(SDL_RENDER OFF)
set(SDL_VULKAN OFF)
add_subdirectory(thirdparty/SDL)

add_library(stylizer_hardware INTERFACE)
target_link_libraries(stylizer_hardware INTERFACE SDL3::SDL3)
add_library(stylizer::hardware ALIAS stylizer_hardware)

# add_executable(tst hello.cpp)
# target_link_libraries(tst PUBLIC stylizer_hardware)
