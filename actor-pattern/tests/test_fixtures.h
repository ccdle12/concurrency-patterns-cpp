#include "actor/actor.h"
#include <string>
#include <iostream>

struct IncrementMessage
{
    int m_inner;
};

class IncrementActor : public Actor<IncrementMessage, int>
{
private:
    void MsgRecv(IncrementMessage&& msg, Actor* sender = nullptr)
    {
        ++msg.m_inner;
    };

    int MsgRecvWithResult(IncrementMessage msg, Actor* sender = nullptr)
    {
        ++msg.m_inner;
        return msg.m_inner;
    };
};
