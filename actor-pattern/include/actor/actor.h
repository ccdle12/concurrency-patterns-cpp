#ifndef ACTOR_H_
#define ACTOR_H_

#include <deque>
#include <future>
#include <functional>
#include <vector>

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
        virtual void MsgRecv(M&& msg, Actor* sender = nullptr) = 0;
        virtual R MsgRecvWithResult(M msg, Actor* sender = nullptr) = 0;

        /**
         * Schedule a task without expecting to return a response to the caller.
         * This will add callable (MsgRecv) to the work queue.
         */
        void Tell(M&& msg, Actor* sender = nullptr)
        {
            // I think std::bind returns a bind based callable object.
            // When it gets captured in the lambda function, we wrap the callable
            // object in a function that returns void. When the void function is
            // copied or moved from the queue and executed, the function will
            // execute the callable_fn.
            auto callable_fn = std::bind(&Actor::MsgRecv, this, std::move(msg), sender);
            std::function<void()> fn = [callable_fn]() { callable_fn; };
            m_queue.push_back(fn);
        }

        /**
         * Schedule a task expecting a future to be returned to the caller.
         * This will add MsgRecvWithResult() to the work queue, and return
         * a future wrapping the return result.
         */
        std::future<R> Ask(M&& msg, Actor* sender = nullptr) {
            auto callable = std::bind(&Actor::MsgRecvWithResult, this, msg, sender);

            // NOTE: I think we need to create a shared pointer because packaged_task has
            // the copy constructor deleted?
            //
            // Create a shared pointer to a pacakged_task() (this creates an async function).
            // The async function (from pacakged_task()) is an async function that returns
            // R and has no parameters.
            auto ptr = std::make_shared<std::packaged_task<R()>>(std::packaged_task<R()>(callable));

            // Follow the shared pointer and get the future from the async task.
            // This will be returned to the caller so that we may wait/block, poll or manage
            // the job until completion.
            auto future = ptr->get_future();

            // Capture the the shared pointer and deference the pointer to get the async function. 
            // Add the command to execute the async function inside the lambda function, so that
            // when this lambda gets called, the async function is executed.
            std::function<void()> fn = [ptr]() {
              (*ptr)();
            };
            m_queue.push_back(std::move(fn));

            return future;
        }

        /**
         * Returns the number of items in the queue.
         */
        std::size_t Size() const
        {
            return m_queue.size();
        }

        /**
         * Contains all the calls to the actor in a queue, ready for execution
         * by some form of scheduler.
         */
        std::vector<std::function<void()>> m_queue;
};

#endif // ACTOR_H_
