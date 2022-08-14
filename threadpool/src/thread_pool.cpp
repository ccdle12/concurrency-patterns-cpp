#include "threadpool/thread_pool.h"
#include <chrono>

using namespace std::chrono_literals;

ThreadPool::ThreadPool(const int num_threads) : m_shutdown{false}
{
    for (int i = 0; i < num_threads; i++) {
        auto worker = std::thread(&ThreadPool::ExecuteTasks, this);
        m_threads.push_back(std::move(worker));
    }
}

ThreadPool::~ThreadPool()
{
    m_shutdown = true;

    for (std::thread &t : m_threads) {
      if (t.joinable()) {
        t.join();
      }
    }
}

std::size_t ThreadPool::Size() const
{
    return m_threads.size();
}

void ThreadPool::ExecuteTasks()
{
    while (!m_shutdown)
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_queue_cv.wait_for(lock, 10ms, [this]{return m_queue_ready;});

        if (!m_queue.empty()) {
            auto task = m_queue.back();
            m_queue.pop_back();

            task();
        }

        if (m_queue.empty()) {
            m_queue_ready = false;
        }

        // Explicitly free the lock before notifying another thread.
        lock.unlock();
        m_queue_cv.notify_one();
    }
}
