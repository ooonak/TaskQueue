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


class WrappedTask
{
public:
    explicit WrappedTask(std::string input) : input_{std::move(input)} {}

    FutureReturnT load() {
        return promise_.get_future();
    }

    void operator()() {
        launch();
    }

    bool done() {

    }
    
protected:
    const std::string input_;
    std::promise<ReturnT> promise_;

private:
    void launch() {
        action();
    }

    void action() {
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
        promise_.set_value(ErrorT{ 42, "Shit happens" });
    }

    void result_cb()
    {
        std::cout << "worker [" << std::this_thread::get_id() << "] Result ready" << std::endl; 
        promise_.set_value(ResultT{input_ + std::string(" World!")});
    }

};

template<class>
inline constexpr bool always_false_v = false;

int main() {
    WrappedTask task("break");
    auto future = task.load();

    std::cout << "main [" << std::this_thread::get_id() << "] Waiting" << std::endl;

    std::thread worker([&](){ task(); });

    while (future.wait_for(std::chrono::milliseconds(1)) != std::future_status::ready) {}

    std::visit([](auto&& arg)
    {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, ResultT>)
            std::cout << "main [" << std::this_thread::get_id() << "] Result: " << arg << std::endl;
        else if constexpr (std::is_same_v<T, ErrorT>)
            std::cout << "main [" << std::this_thread::get_id() << "] Error: " << arg.first << " " << arg.second << std::endl;
        else
            static_assert(always_false_v<T>, "non-exhaustive visitor!");
    }, future.get());

    worker.join();

    return 0;
}
