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

    const auto start = std::chrono::system_clock::now();
    spdlog::info("Waiting for job");

    while (future1.wait_for(1ms) != std::future_status::ready) {}

    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
    spdlog::info("Waited for {}ms, reault ready", elapsed);

    future1.get();
    spdlog::info("Job done");

    gStop = true;
    workerThread.join();

    return EXIT_SUCCESS;
}
