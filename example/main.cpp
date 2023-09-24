#include "TaskQueue/TaskQueue.hpp"
#include "spdlog/spdlog.h"
#include <atomic>
#include <chrono>
#include <iostream>
#include <sstream>

using namespace std::chrono_literals;

std::atomic<bool> gStop{false};
std::atomic<unsigned> gCounter{0};

void worker(TaskQueue &queue)
{
  spdlog::info("Launching worker in thread {}", std::hash<std::thread::id>{}(std::this_thread::get_id()));
  while (not gStop) {
    queue.consume();
  }
}

void job1()
{
  unsigned count = ++gCounter;

  spdlog::info("  ['{}' '{}' '{}'] Begin", __PRETTY_FUNCTION__,
               std::hash<std::thread::id>{}(std::this_thread::get_id()), count);
  std::this_thread::sleep_for(100ms);
  spdlog::info("  ['{}' '{}' '{}'] End", __PRETTY_FUNCTION__, std::hash<std::thread::id>{}(std::this_thread::get_id()),
               count);
}

void job2(int)
{
  unsigned count = ++gCounter;

  spdlog::info("  ['{}' '{}' '{}'] Begin", __PRETTY_FUNCTION__,
               std::hash<std::thread::id>{}(std::this_thread::get_id()), count);
  std::this_thread::sleep_for(100ms);
  spdlog::info("  ['{}' '{}' '{}'] End", __PRETTY_FUNCTION__, std::hash<std::thread::id>{}(std::this_thread::get_id()),
               count);
}

int job3(int number)
{
  unsigned count = ++gCounter;

  spdlog::info("  ['{}' '{}' '{}'] Begin", __PRETTY_FUNCTION__,
               std::hash<std::thread::id>{}(std::this_thread::get_id()), count);
  std::this_thread::sleep_for(100ms);
  spdlog::info("  ['{}' '{}' '{}'] End", __PRETTY_FUNCTION__, std::hash<std::thread::id>{}(std::this_thread::get_id()),
               count);

  return number;
}

std::string job4(const std::string &first, const std::string last, size_t age)
{
  unsigned count = ++gCounter;

  spdlog::info("  ['{}' '{}' '{}'] Begin", __PRETTY_FUNCTION__,
               std::hash<std::thread::id>{}(std::this_thread::get_id()), count);
  std::this_thread::sleep_for(100ms);
  spdlog::info("  ['{}' '{}' '{}'] End", __PRETTY_FUNCTION__, std::hash<std::thread::id>{}(std::this_thread::get_id()),
               count);

  std::ostringstream oss;
  oss << first << " " << last << ", age " << age;
  return oss.str();
}

class IHaveMembers
{
public:
  struct Data {
    int x;
  };

  bool iAmAMemberFunction(const Data &m)
  {
    unsigned count = ++gCounter;

    spdlog::info("  ['{}' '{}' '{}'] Begin", __PRETTY_FUNCTION__,
                 std::hash<std::thread::id>{}(std::this_thread::get_id()), count);
    std::this_thread::sleep_for(100ms);
    spdlog::info("  ['{}' '{}' '{}'] End", __PRETTY_FUNCTION__,
                 std::hash<std::thread::id>{}(std::this_thread::get_id()), count);

    return (m.x > aNumber_);
  }

private:
  const int aNumber_{2};
};

int main()
{
  TaskQueue queue{10};

  spdlog::info("About to start consumer from thread {}", std::hash<std::thread::id>{}(std::this_thread::get_id()));
  std::thread workerThread(worker, std::ref(queue));

  spdlog::info("Push job");
  auto optFuture1 = queue.post(job1);
  auto optFuture2 = queue.post(job2, 2);
  auto optFuture3 = queue.post(job3, 3);

  const std::string first = "John";
  std::string last = "Doe";
  auto optFuture4 = queue.post(job4, first, last, 42);

  IHaveMembers iHaveMembers;
  IHaveMembers::Data data{1};
  auto optFuture5 = queue.post(&IHaveMembers::iAmAMemberFunction, &iHaveMembers, data);

  auto optFuture6 = queue.post(
      [](int a, int b) {
        unsigned count = ++gCounter;
        spdlog::info("  ['{}' '{}' '{}'] Begin", __PRETTY_FUNCTION__,
                     std::hash<std::thread::id>{}(std::this_thread::get_id()), count);
        std::this_thread::sleep_for(100ms);
        spdlog::info("  ['{}' '{}' '{}'] End", __PRETTY_FUNCTION__,
                     std::hash<std::thread::id>{}(std::this_thread::get_id()), count);
        return a * b;
      },
      6, 7);

  if (optFuture1.has_value()) {
    const auto start = std::chrono::system_clock::now();
    spdlog::info("Waiting for job");
    while (optFuture1->wait_for(1ms) != std::future_status::ready) {
    }
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
    spdlog::info("Waited for {}ms, reault ready", elapsed);
    optFuture1->get();
    spdlog::info("Job done");
  }

  if (optFuture2.has_value()) {
    const auto start = std::chrono::system_clock::now();
    spdlog::info("Waiting for job");
    while (optFuture2->wait_for(1ms) != std::future_status::ready) {
    }
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
    spdlog::info("Waited for {}ms, reault ready", elapsed);
    optFuture2->get();
    spdlog::info("Job done");
  }

  if (optFuture3.has_value()) {
    const auto start = std::chrono::system_clock::now();
    spdlog::info("Waiting for job");
    while (optFuture3->wait_for(1ms) != std::future_status::ready) {
    }
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
    spdlog::info("Waited for {}ms, reault ready", elapsed);
    const auto result = optFuture3->get();
    spdlog::info("Job done, {}", result);
  }

  if (optFuture4.has_value()) {
    const auto start = std::chrono::system_clock::now();
    spdlog::info("Waiting for job");
    while (optFuture4->wait_for(1ms) != std::future_status::ready) {
    }
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
    spdlog::info("Waited for {}ms, reault ready", elapsed);
    const auto result = optFuture4->get();
    spdlog::info("Job done, {}", result);
  }

  if (optFuture5.has_value()) {
    const auto start = std::chrono::system_clock::now();
    spdlog::info("Waiting for job");
    while (optFuture5->wait_for(1ms) != std::future_status::ready) {
    }
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
    spdlog::info("Waited for {}ms, reault ready", elapsed);
    const auto result = optFuture5->get();
    spdlog::info("Job done, {}", result);
  }

  if (optFuture6.has_value()) {
    const auto start = std::chrono::system_clock::now();
    spdlog::info("Waiting for job");
    while (optFuture6->wait_for(1ms) != std::future_status::ready) {
    }
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
    spdlog::info("Waited for {}ms, reault ready", elapsed);
    const auto result = optFuture6->get();
    spdlog::info("Job done, {}", result);
  }

  queue.stop();

  gStop = true;
  workerThread.join();

  return EXIT_SUCCESS;
}
