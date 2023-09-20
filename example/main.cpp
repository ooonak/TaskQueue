#include "TaskQueue/TaskQueue.hpp"
#include "spdlog/spdlog.h"
#include <chrono>
#include <iostream>
#include <atomic>

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

void job()
{
    using namespace std::chrono_literals;
    unsigned count = gCounter++;

    spdlog::info("  [{}] Start job {}", std::hash<std::thread::id>{}(std::this_thread::get_id()), count);
    std::this_thread::sleep_for(1s);
    spdlog::info("  [{}] Job done {}", std::hash<std::thread::id>{}(std::this_thread::get_id()), count);
}

int main()
{
    TaskQueue queue{};

    spdlog::info("About to start consumer from thread {}", std::hash<std::thread::id>{}(std::this_thread::get_id()));
    std::thread workerThread(worker, std::ref(queue));

    const size_t jobs = 5;
    spdlog::info("About to push {} jobs", jobs);

    std::vector<std::future<void>> futures;
    for (size_t i=1; i<=jobs; ++i)
    {
      spdlog::info("Push job {}", i);
      futures.emplace_back(queue.postTask(job));
    }

    for (size_t i=1; i<=futures.size(); ++i)
    {
      spdlog::info("Waiting for job {}", i);
      futures.at(i-1).get();
      spdlog::info("Job {} done", i);
    }

    gStop = true;
    workerThread.join();

    return EXIT_SUCCESS;
}
