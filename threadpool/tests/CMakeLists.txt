include(GoogleTest)

# Add a test executable.
add_executable(
    bin_test
    test_thread_pool.cpp
)

# Link the bin with gtest.
# NOTE: we also need to link the binaries from /src for the modules we want to
# test.
target_link_libraries(
    bin_test
    ThreadPool::threadpool
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(bin_test)
