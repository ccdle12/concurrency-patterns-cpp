add_library(threadpool thread_pool.cpp)

target_include_directories(threadpool
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)
