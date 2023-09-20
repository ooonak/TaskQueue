#include "TaskQueue/TaskQueue.hpp"
#include <gtest/gtest.h>
#include <thread>

std::atomic<bool> gStop{false};
std::atomic<size_t> gCounter{0};

void worker(TaskQueue& queue)
{
  while (not gStop)
  {
    queue.consume();
  }
}

void job1()
{
  gCounter++;
}

TEST(TaskQueue, PushAndPull) {
  gStop = false;
  gCounter = 0;

  TaskQueue queue{};
  std::thread workerThread(worker, std::ref(queue));

  const size_t jobs = 3;

  for (size_t i=1; i<=jobs; ++i)
  {
    auto future1 = queue.postTask(job1);
    future1.get();
  }

  EXPECT_EQ(gCounter, jobs);

  gStop = true;
  workerThread.join();
}

TEST(TaskQueue, PushThenPull) {
  gStop = false;
  gCounter = 0;

  TaskQueue queue{};
  std::thread workerThread(worker, std::ref(queue));

  const size_t jobs = 3;

  std::vector<std::future<void>> futures;
  for (size_t i=1; i<=jobs; ++i)
  {
    futures.emplace_back(queue.postTask(job1));
  }

  for (size_t i=1; i<=futures.size(); ++i)
  {
    futures.at(i-1).get();
  }

  EXPECT_EQ(gCounter, jobs);

  gStop = true;
  workerThread.join();
}
