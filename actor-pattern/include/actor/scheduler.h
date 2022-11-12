#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "actor/actor.h"
#include "threadpool/thread_pool.h"

/**
 * Scheduler is responsible for managing registered Actors and polling their queues.
 * When there is work in the Actors' queues, then the Scheduler pops the work
 * off the queue and dispatches the tasks to an execution environment (ThreadPool).
 */

// TODO:
// 1. Register an Actor
// 2. Create a ThreadPool
// 3. Start the Scheduler by polling the Actors and consuming from their queue
// and executing on the threadpool.
class Scheduler
{
public:
    Scheduler(const int num_threads): m_thread_pool{num_threads} {};

    // TODO: Register the actor and add the reference to a queue?
    // TODO: Maybe a shared ptr to a queue?
    void Register(std::shared_ptr<fn_queue> actor_queue);

    // TODO: Comments
    int QueueSize() const;

    // TODO: Maybe an internal or protected method? maybe just make one block loop
    void Execute();


private:
    // TODO: Can't move or copy ThreadPool because it contains a mutex and
    // condition_variable.
    ThreadPool m_thread_pool;

    // TODO: The Actor requires a template, so maybe reference a queue.
    std::vector<std::shared_ptr<fn_queue>> m_actors;
};

#endif // SCHEDULER_H_
