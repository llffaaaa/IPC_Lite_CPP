cmake_minimum_required(VERSION 3.10)
project(ipc-lite-cpp LANGUAGES CXX)

add_library(ipc STATIC src/shm.cpp)
target_include_directories(ipc PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(day1_writer src/writer.cpp)
target_link_libraries(day1_writer PRIVATE ipc)

add_executable(day1_reader src/reader.cpp)
target_link_libraries(day1_reader PRIVATE ipc)
