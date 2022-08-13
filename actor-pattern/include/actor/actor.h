#include <future>
#include <deque>

// Interface for an Actor, this should be used to inherit and implement all
// Actors.
template <typename M, typename R>
class Actor
{
    public:
        void Tell(M&& msg, A *sender = nullptr);
        std::future<R> Ask(M&& msg, A *sender = nullptr);

        // Returns the number of items in the queue.
        std::size_t Size();

    private:
        virtual void MsgRecv(M&& msg, A *sender = nullptr) = 0;
        virtual void MsgRecvWithResult(M&& msg, A *sender = nullptr) = 0;

        std::deque<std::unique_ptr<Callable>> m_queue;
}
