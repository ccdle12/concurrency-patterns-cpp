#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <thread>
#include <deque>
#include <vector>
#include <functional>
#include <future>
#include <condition_variable>

/**
 * ThreadPool contains a collection of threads that will be used to execute
 * tasks.
 */
class ThreadPool
{
public:
    ThreadPool(const int num_threads);
    ~ThreadPool();

    // Returns the number of threads in the ThreadPool.
    std::size_t Size() const;

    // Accepts a function and it's args to be queued and executed on the thread
    // pool. A std::future is returned to the caller for handling.
    template<typename F, typename... Args>
    auto Execute(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
    {
        // Bind the function and args into a callable function and forward the
        // references to their respective l or rvales.
        std::function<decltype(f(args...))()> fn = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

        // Create a pointer to copy/move the callable function and wrap the 
        // function as a packaged_task so that it can be invoked async and the 
        // result/exception stored in a future.
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(fn);

        //  Create a lambda to create a generic void function, so that the all
        //  the functions can be stored in a queue.
        std::function<void()> wrapper_fn = [task_ptr]() {
          (*task_ptr)();
        };

        m_queue.push_front(wrapper_fn);

        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            m_queue_ready = true;
        }

        return task_ptr->get_future();
    }

    // Accepts a void function without returning a future. The intention is to
    // allow the caller to pass a void function that ALREADY has a future allocated
    // by the caller.
    void Execute(std::function<void()> task);

   
private:
    // Contains all the threads to receive a task for execution.
    std::vector<std::thread> m_threads;

    // Mutex for m_queuem it contains all the tasks to be read and executed.
    std::mutex m_queue_mutex;
    std::deque<std::function<void()>> m_queue;

    // Conditional variable and flag to notify waiting threads that the queue
    // is ready for work.
    std::condition_variable m_queue_cv;
    bool m_queue_ready;

    // Flag to identify whether the threads should stop working and tear down
    // safely.
    bool m_shutdown;

    // A method called from the threads in the thread pool. It will wait to
    // acquire the queue mutex, read from the queue and execute a task.
    void ExecuteTasks();
};
    
#endif
