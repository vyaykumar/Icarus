cmake_minimum_required(VERSION 4.2)
project(Icarus C CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-O2 -Wall -Wextra -Wpedantic -g)

add_subdirectory(transport-layer)

add_executable(Icarus main.cpp)
target_link_libraries(Icarus event_bus)