#pragma once

#include <future>
#include "base/Director.h"
#include "base/Scheduler.h"
#include "doctest_fwd.h"


/// A helper class for testing asynchronous functions. It waits while the function
/// is executing on another thread and in the meantime updates the scheduler so
/// async result queued by `runOnAxmolThread` will be executed.
///
/// @code
///    auto run = AsyncRunner<bool>();
///    fu->isFileExist("text/123.txt", [&](bool exists) {
///        run.finish(exists); // Set result
///    });
///    auto result = run(); // Get result
///    CHECK(result); // Test result
/// @endcode
template <class T>
class AsyncRunner {
    std::promise<T> p;

public:
    void finish(const T& value) { p.set_value(value); }
    void finish(T&& value) { p.set_value(value); }

    T operator()() {
        auto scheduler = ax::Director::getInstance()->getScheduler();
        auto f = p.get_future();
        while (true) {
            auto result = f.wait_for(std::chrono::seconds(0));
            if (result == std::future_status::ready)
                break;
            std::this_thread::yield();
            scheduler->update(0);
        }
        return f.get();
    }
};


namespace ax {
    doctest::String toString(const Color4B& value);
    doctest::String toString(const Vec2& value);
    doctest::String toString(const Vec3& value);
}
