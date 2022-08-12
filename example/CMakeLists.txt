cmake_minimum_required(VERSION 3.10)

project(example VERSION 1.0)

add_executable(example src/main.cpp)
target_compile_features(example PRIVATE cxx_auto_type)
set(CMAKE_CXX_FLAGS -pthread)

find_package(ThreadPool 1.0 REQUIRED)
target_link_libraries(example ThreadPool::threadpool)
