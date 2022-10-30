include(GoogleTest)

# Add a test executable.
add_executable(
    bin_test
    test_actor.cpp
    test_scheduler.cpp
    test_fixtures.h
)

# Link the bin with gtest.
# NOTE: we also need to link the binaries from /src for the modules we want to
# test.
target_link_libraries(
    bin_test
    Actor::actor
    GTest::gtest_main
)

gtest_discover_tests(bin_test)
