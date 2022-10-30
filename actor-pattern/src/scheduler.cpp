#include "actor/scheduler.h"
#include<iostream>

// TODO: No thread safety guarantees.
/* void Scheduler::Register(Actor& actor) */
void Scheduler::Register(std::vector<std::function<void()>>& actor_queue)
{
    m_actors.push_back(actor_queue);
}
    
int Scheduler::QueueSize() const
{
    return m_actors.size();
}

void Scheduler::Execute()
{
    // TODO:
    // 1. Iterate on each actor.
    for (auto& queue : m_actors) {
        auto fn = queue.get().back();
        m_thread_pool.Execute(fn);
        queue.get().pop_back();
    }
    // 2. Pop off each queue and exectue.
}
