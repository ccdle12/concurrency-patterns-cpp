#include <gtest/gtest.h>
#include "test_fixtures.h"

TEST(Actor, RegisterMessage)
{
    // Test that we can add send a message to the Actor, and it will queue the
    // message for execution.
    IncrementActor mock_actor;
    ASSERT_EQ(mock_actor.Size(), 0);

    int x = 1;
    IncrementMessage m{ .m_inner = x };
    mock_actor.Tell(std::move(m));

    ASSERT_EQ(mock_actor.Size(), 1);
}
