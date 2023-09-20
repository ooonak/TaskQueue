#pragma once

/**
 * References
 * C++ Concurrency in action, second edition, p. 86.
 */

#include <deque>
#include <future>
#include <mutex>
#include <thread>
#include <utility>

class TaskQueue {
    public:
        // Should be polled from consumer thread.
        void consume()
        {
            std::packaged_task<void()> task;
            {
                const std::lock_guard<std::mutex> lock(mtx_);
                if (tasks_.empty())
                {
                    return;
                }
                task = std::move(tasks_.front());
                tasks_.pop_front();
            }
            task();
        }

        // Push task to queue for execution in consumer thread.
        template<typename Func>
        std::future<void> postTask(Func func)
        {
            std::packaged_task<void()> task(func);
            std::future<void> result = task.get_future();
            const std::lock_guard<std::mutex> lock(mtx_);
            tasks_.push_back(std::move(task));
            return result;
        }

    private:
        std::mutex mtx_;
        std::deque<std::packaged_task<void()>> tasks_;
};
