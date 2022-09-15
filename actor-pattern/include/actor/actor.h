#include <future>
#include <deque>

/**
 * Base class for all Actors.
 *   - M is a generic for a MessageType
 *   - R is a generic for a ResultType
 *
 * An Actor has it's own private state and queue for scheduled tasks.
 * Each Actor is isolated and should not have minimal shared memory or knowledge
 * of other Actors.
 *
 *
 * TODO: Figure out sender
 */
template <typename M, typename R>
class Actor
{
    public:
        /**
         * Schedule a task without expecting a response to the caller. This will
         * add MsgRecv to the work queue.
         */
        void Tell(M&& msg, Actor* sender = nullptr);

        /**
         * Schedule a task expecting a future to be returned to the caller.
         * This will add MsgRecvWithResult() to the work queue, and return
         * a future wrapping the return result.
         */
        std::future<R> Ask(M&& msg, Actor* sender = nullptr);

        /**
         * Returns the number of items in the queue.
         */
        std::size_t Size();

    private:
        virtual void MsgRecv(M&& msg, A* sender = nullptr) = 0;
        virtual R MsgRecvWithResult(M&& msg, A* sender = nullptr) = 0;

        std::deque<std::unique_ptr<Callable>> m_queue;
}
