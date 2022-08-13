#include <gtest/gtest.h>
#include <iostream>
#include "threadpool/thread_pool.h"


TEST(ThreadPool, Init) {
    constexpr int num_threads{5}; 

    ThreadPool thread_pool{num_threads};
    ASSERT_EQ(thread_pool.Size(), num_threads);

    auto add = [](int x, int y) { return x + y; };
    auto future1 = thread_pool.Execute(add, 1 , 2);
    future1.wait();

    ASSERT_EQ(future1.get(), 3);
}
