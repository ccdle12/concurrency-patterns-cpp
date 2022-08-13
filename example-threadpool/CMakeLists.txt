cmake_minimum_required(VERSION 3.10)

project(example-threadpool VERSION 1.0)

add_executable(example-threadpool src/main.cpp)
target_compile_features(example-threadpool PRIVATE cxx_auto_type)
set(CMAKE_CXX_FLAGS -pthread)

find_package(ThreadPool 1.0 REQUIRED)
target_link_libraries(example-threadpool ThreadPool::threadpool)
