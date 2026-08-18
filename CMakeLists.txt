cmake_minimum_required(VERSION 3.10)
project(ipc-lite-cpp LANGUAGES CXX)
add_executable(demo main.cpp src/shm.cpp)
target_include_directories(demo PRIVATE ${CMAKE_SOURCE_DIR}/include)
