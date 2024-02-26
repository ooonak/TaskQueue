#include <iostream>
#include <future>
#include <thread>
#include <string>
#include <optional>
#include <deque>
#include <memory>
#include <chrono>
#include <string>
#include <vector>
#include <variant>


using ResultT = std::string;
using ErrorT = std::pair<int, std::string>;
using ReturnT = std::variant<ResultT, ErrorT>;
using FutureReturnT = std::future<ReturnT>;

template<class>
inline constexpr bool always_false_v = false;

class WrappedTask
{
public:
    explicit WrappedTask(std::string input) : input_{std::move(input)} {
        load();
    }

    void operator()() {
        launch();
    }

    bool done() {
        return (future_.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready);
    }
    
    void result()
    {
        std::visit([](auto&& arg)
        {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, ResultT>)
                std::cout << "main [" << std::this_thread::get_id() << "] Result: " << arg << std::endl;
            else if constexpr (std::is_same_v<T, ErrorT>)
                std::cout << "main [" << std::this_thread::get_id() << "] Error: " << arg.first << " " << arg.second << std::endl;
            else
                static_assert(always_false_v<T>, "non-exhaustive visitor!");
        }, future_.get());
    }

protected:
    const std::string input_;
    std::promise<ReturnT> promise_;
    FutureReturnT future_;

    virtual void action()
    {
        std::cout << "worker [" << std::this_thread::get_id() << "] About to execute" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(2));

        if (input_ == "break")
          error_cb();
        else
          result_cb();
    }

    virtual void error_cb()
    {
        std::cout << "worker [" << std::this_thread::get_id() << "] Error ready" << std::endl; 
        promise_.set_value(ErrorT{ 42, "Shit happens" });
    }

    virtual void result_cb()
    {
        std::cout << "worker [" << std::this_thread::get_id() << "] Result ready" << std::endl; 
        promise_.set_value(ResultT{input_ + std::string(" World!")});
    }

private:
    void load() {
        future_ = promise_.get_future();
    }

    void launch() {
        action();
    }
};

void run_ok()
{
    WrappedTask task("Hello");
    std::cout << "main [" << std::this_thread::get_id() << "] Waiting" << std::endl;
    std::thread worker([&](){ task(); });
    while (!task.done()) {}
    task.result();
    worker.join();
}

void run_error()
{
    WrappedTask task("break");
    std::cout << "main [" << std::this_thread::get_id() << "] Waiting" << std::endl;
    std::thread worker([&](){ task(); });
    while (!task.done()) {}
    task.result();
    worker.join();
}

int main() {
    std::cout << "\n==================== About to run example that succeeds ====================\n" << std::endl;
    run_ok();
    std::cout << "\n====================== About to run example that fails =====================\n" << std::endl;
    run_error();    
    std::cout << "\n=================================== Done ===================================\n" << std::endl;

    return 0;
}
