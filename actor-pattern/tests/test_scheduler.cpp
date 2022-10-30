#include <gtest/gtest.h>
#include <iostream>
#include "actor/scheduler.h"
#include "test_fixtures.h"

TEST(Scheudler, Init) {
    Scheduler scheduler{5 /* number of threads */};
    IncrementActor mock_actor;

    // No thread saftey guarantees, test that an actors queue can be registered.
    ASSERT_EQ(scheduler.QueueSize(), 0);
    scheduler.Register(mock_actor.m_queue);
    ASSERT_EQ(scheduler.QueueSize(), 1);

    // Ask the actor to execute the IncrementMessage.
    int x = 1;
    IncrementMessage m{ .m_inner = x };
    auto future = mock_actor.Ask(std::move(m));
    ASSERT_EQ(mock_actor.Size(), 1);

    // Advance the scheduler to read all queues and execute the functions.
    // TODO: Maybe separate the Execute implementation and the runtime (while loop)?
    scheduler.Execute();

    // The caller is able to call the future after the scheduler has executed
    // to find the result of the message.
    ASSERT_EQ(future.get(), 2);
}
