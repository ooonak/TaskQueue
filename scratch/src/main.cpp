#include <chrono>
#include <deque>
#include <future>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <variant>
#include <vector>

// TOOD: One can propably just template ResultT.
using ResultT = std::string;
using ErrorT = std::pair<int, std::string>;
using ReturnT = std::variant<ResultT, ErrorT>;
using FutureReturnT = std::future<ReturnT>;

template <class> inline constexpr bool always_false_v = false;

class WrappedTask
{
public:
  explicit WrappedTask()
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    future_ = promise_.get_future();
  }

  ~WrappedTask() { std::cout << __PRETTY_FUNCTION__ << std::endl; }

  // Called once by worker thread
  virtual void operator()() = 0;

  // Polled from waiting thread
  bool done() { return (future_.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready); }

  // Called once by waiting thread after done has returned true.
  void result()
  {
    // TODO: For now, as PoC just print, we should split templated result type and error handling up more elegantly.
    std::visit(
        [](auto &&arg) {
          using T = std::decay_t<decltype(arg)>;

          if constexpr (std::is_same_v<T, ResultT>)
            std::cout << "main [" << std::this_thread::get_id() << "] Result: " << arg << std::endl;
          else if constexpr (std::is_same_v<T, ErrorT>)
            std::cout << "main [" << std::this_thread::get_id() << "] Error: " << arg.first << " " << arg.second
                      << std::endl;
          else
            static_assert(always_false_v<T>, "non-exhaustive visitor!");
        },
        future_.get());
  }

protected:
  std::promise<ReturnT> promise_;

private:
  FutureReturnT future_;
};

class WrappedTaskImpl : public WrappedTask
{
public:
  explicit WrappedTaskImpl(std::string input) : input_{std::move(input)}
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }

  ~WrappedTaskImpl() { std::cout << __PRETTY_FUNCTION__ << std::endl; }

  void operator()() override { action(); }

protected:
  std::string input_;

  void action()
  {
    std::cout << "worker [" << std::this_thread::get_id() << "] About to execute" << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(2));

    if (input_ == "break")
      error_cb();
    else
      result_cb();
  }

  void error_cb()
  {
    std::cout << "worker [" << std::this_thread::get_id() << "] Error ready" << std::endl;
    promise_.set_value(ErrorT{42, "Shit happens"});
  }

  void result_cb()
  {
    std::cout << "worker [" << std::this_thread::get_id() << "] Result ready" << std::endl;
    promise_.set_value(ResultT{input_ + std::string(" World!")});
  }
};

void run_ok()
{
  WrappedTaskImpl task("World!");
  std::cout << "main [" << std::this_thread::get_id() << "] Waiting" << std::endl;
  std::thread worker([&]() { task(); });
  while (!task.done()) {
  }
  task.result();
  worker.join();
}

void run_error()
{
  WrappedTaskImpl task("break");
  std::cout << "main [" << std::this_thread::get_id() << "] Waiting" << std::endl;
  std::thread worker([&]() { task(); });
  while (!task.done()) {
  }
  task.result();
  worker.join();
}

int main()
{
  std::cout << "\n==================== About to run example that succeeds ====================\n" << std::endl;
  run_ok();
  std::cout << "\n====================== About to run example that fails =====================\n" << std::endl;
  run_error();
  std::cout << "\n=================================== Done ===================================\n" << std::endl;

  return 0;
}
