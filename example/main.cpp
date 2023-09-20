#include "TaskQueue/TaskQueue.hpp"
#include "spdlog/spdlog.h"
#include <chrono>
#include <iostream>
#include <atomic>

using namespace std::chrono_literals;

std::atomic<bool> gStop{false};
std::atomic<unsigned> gCounter{0};

void worker(TaskQueue& queue)
{
    spdlog::info("Launching worker in thread {}", std::hash<std::thread::id>{}(std::this_thread::get_id()));
    while (not gStop)
    {
        queue.consume();
    }
}

void job1()
{
    unsigned count = ++gCounter;

    spdlog::info("  ['{}' '{}' '{}'] Begin", __PRETTY_FUNCTION__, std::hash<std::thread::id>{}(std::this_thread::get_id()), count);
    std::this_thread::sleep_for(1s);
    spdlog::info("  ['{}' '{}' '{}'] End", __PRETTY_FUNCTION__, std::hash<std::thread::id>{}(std::this_thread::get_id()), count);
}

void job2(int number)
{
  unsigned count = ++gCounter;

  spdlog::info("  ['{}' '{}' '{}'] Begin number={}", __PRETTY_FUNCTION__, std::hash<std::thread::id>{}(std::this_thread::get_id()), count, number);
  std::this_thread::sleep_for(2s);
  spdlog::info("  ['{}' '{}' '{}'] End", __PRETTY_FUNCTION__, std::hash<std::thread::id>{}(std::this_thread::get_id()), count);
}

int job3(int number)
{
  unsigned count = ++gCounter;

  spdlog::info("  ['{}' '{}' '{}'] Begin number={}", __PRETTY_FUNCTION__, std::hash<std::thread::id>{}(std::this_thread::get_id()), count, number);
  std::this_thread::sleep_for(1500ms);
  spdlog::info("  ['{}' '{}' '{}'] End number={}", __PRETTY_FUNCTION__, std::hash<std::thread::id>{}(std::this_thread::get_id()), count, ++number);
  return number;
}

int main()
{
    TaskQueue queue{};

    spdlog::info("About to start consumer from thread {}", std::hash<std::thread::id>{}(std::this_thread::get_id()));
    std::thread workerThread(worker, std::ref(queue));

    spdlog::info("Push job");
    auto future1 = queue.postTask(job1);
    //auto future2 = queue.postTask(job2);
    //auto future3 = queue.postTask(job3);

    std::future_status status;
    do {
      status = future1.wait_for(333ms);
      spdlog::info("Waiting for job");
    } while (status != std::future_status::ready);

    future1.get();
    spdlog::info("Job done");

    gStop = true;
    workerThread.join();

    return EXIT_SUCCESS;
}
