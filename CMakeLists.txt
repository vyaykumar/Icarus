cmake_minimum_required(VERSION 4.2)
project(Icarus C CXX)

# Set C++23 Standard
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set C23 Standard
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-O2 -Wall -Wextra -Wpedantic -g)

add_executable(Icarus main.cpp)
