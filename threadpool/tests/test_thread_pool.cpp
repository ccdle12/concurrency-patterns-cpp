#include <gtest/gtest.h>
#include <iostream>
#include "threadpool/thread_pool.h"


// Adding multiple tests here since not sure if creating threads per test might
// lead to irregularities in results.
TEST(ThreadPool, Init) {
    constexpr int num_threads{5}; 

    ThreadPool thread_pool{num_threads};
    ASSERT_EQ(thread_pool.Size(), num_threads);

    auto add = [](int x, int y) { return x + y; };
    auto future1 = thread_pool.Execute(add, 1 , 2);
    future1.wait();

    ASSERT_EQ(future1.get(), 3);

    // This is a test that allows the caller to provide their own future
    // instead of receiving a future from the threadpool.
    auto task_ptr = std::make_shared<std::packaged_task<int()>>(std::bind(add, 5, 5));
    std::function<void()> wrapper_fn = [task_ptr]() {
      (*task_ptr)();
    };
    auto future2 = task_ptr->get_future();

    thread_pool.Execute(wrapper_fn);

    future2.wait();
    ASSERT_EQ(future2.get(), 10);
}
