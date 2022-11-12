cmake_minimum_required(VERSION 3.10)

project(example-threadpool VERSION 1.0)

add_executable(${PROJECT_NAME} src/main.cpp)
set(CMAKE_CXX_FLAGS -pthread)

find_package(ThreadPool 1.0 REQUIRED)

# Link this library to the ThreadPool package as was declared using the ALIAS
# in the threadpool library.
target_link_libraries(${PROJECT_NAME} ThreadPool::threadpool)
