#include "TaskQueue/TaskQueue.hpp"
#include <gtest/gtest.h>
#include <thread>

std::atomic<bool> gStop{false};
std::atomic<size_t> gCounter{0};

void worker(TaskQueue &queue)
{
  while (not gStop) {
    queue.consume();
  }
}

void job1() { gCounter++; }

TEST(TaskQueue, PushAndPull)
{
  gStop = false;
  gCounter = 0;

  TaskQueue queue{10};
  std::thread workerThread(worker, std::ref(queue));

  const size_t jobs = 3;

  for (size_t i = 1; i <= jobs; ++i) {
    auto optFuture = queue.post(job1);
    EXPECT_NE(optFuture, std::nullopt);
    optFuture->get();
  }

  EXPECT_EQ(gCounter, jobs);

  gStop = true;
  workerThread.join();
}

TEST(TaskQueue, PushThenPull)
{
  gStop = false;
  gCounter = 0;

  TaskQueue queue{10};
  std::thread workerThread(worker, std::ref(queue));

  const size_t jobs = 3;

  auto optFuture1 = queue.post(job1);
  auto optFuture2 = queue.post(job1);
  auto optFuture3 = queue.post(job1);

  optFuture1->get();
  optFuture2->get();
  optFuture3->get();

  EXPECT_EQ(gCounter, jobs);

  gStop = true;
  workerThread.join();
}

TEST(TaskQueue, TestStop)
{
  gStop = false;
  gCounter = 0;

  TaskQueue queue{10};
  std::thread workerThread(worker, std::ref(queue));

  const size_t jobs = 1;

  auto optFuture1 = queue.post(job1);
  queue.stop();
  EXPECT_FALSE(queue.post(job1).has_value());

  EXPECT_TRUE(optFuture1.has_value());
  optFuture1->get();

  EXPECT_EQ(gCounter, jobs);

  gStop = true;
  workerThread.join();
}

TEST(TaskQueue, TestMaxLength)
{
  gStop = false;
  gCounter = 0;

  TaskQueue queue{1};
  std::thread workerThread(worker, std::ref(queue));

  const size_t jobs = 1;

  auto optFuture1 = queue.post(job1);
  EXPECT_FALSE(queue.post(job1).has_value());
  EXPECT_TRUE(optFuture1.has_value());
  optFuture1->get();

  EXPECT_EQ(gCounter, jobs);

  gStop = true;
  workerThread.join();
}
