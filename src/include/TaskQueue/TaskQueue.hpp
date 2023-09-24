#pragma once

#include <deque>
#include <functional>
#include <future>
#include <mutex>
#include <optional>
#include <thread>
#include <utility>

#include <iostream>

class TaskQueue
{
public:
  explicit TaskQueue(size_t maxQueueLength) : maxQueueLength_{maxQueueLength} {}

  // Should be polled from consumer thread.
  void consume()
  {
    std::function<void()> task;
    {
      const std::lock_guard<std::mutex> lock(mutex_);
      if (tasks_.empty()) {
        return;
      }
      task = std::move(tasks_.front());
      tasks_.pop_front();
    }
    task();
  }

  // Push task to queue for execution in consumer thread.
  template <class Function, class... Args>
  auto post(Function &&func, Args &&...args)
      -> std::optional<std::future<typename std::invoke_result<Function, Args...>::type>>
  {
    using invokeResultT = typename std::invoke_result<Function, Args...>::type;

    auto task = std::make_shared<std::packaged_task<invokeResultT()>>(
        std::bind(std::forward<Function>(func), std::forward<Args>(args)...));
    std::future<invokeResultT> result = task->get_future();

    const std::lock_guard<std::mutex> lock(mutex_);
    if (stop_ == true || tasks_.size() >= maxQueueLength_) {
      return std::nullopt;
    }

    tasks_.emplace_back([task](){ (*task)(); }); // A lambda (std::function<void>) that calls the function stored in shared_ptr.
    return result;
  }

  void stop()
  {
    const std::lock_guard<std::mutex> lock(mutex_);
    stop_ = true;
  }

private:
  const size_t maxQueueLength_;
  bool stop_{false};
  std::mutex mutex_;
  std::deque<std::function<void()>> tasks_;
};
