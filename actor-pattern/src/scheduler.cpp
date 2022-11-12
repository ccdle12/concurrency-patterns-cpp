#include "actor/scheduler.h"
#include<iostream>

// TODO: No thread safety guarantees.
void Scheduler::Register(std::shared_ptr<fn_queue> actor_queue)
{
    m_actors.push_back(actor_queue);
}
    
int Scheduler::QueueSize() const
{
    return m_actors.size();
}

void Scheduler::Execute()
{
    for (auto& queue : m_actors) {
        m_thread_pool.Execute(queue->back());
        queue->pop_back();
    }
}
