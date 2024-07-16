/****************************************************************************

 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
 
 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "base/JobSystem.h"
#include "base/Director.h"
#include "yasio/thread_name.hpp"

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

NS_AX_BEGIN

#pragma region JobExecutor
class JobExecutor
{
public:
    explicit JobExecutor(std::span<std::shared_ptr<JobThreadData>> tdds) : stop(false)
    {
        for (auto thread_data : tdds)
            workers.emplace_back([this, thread_data] {
                thread_data->init();
                yasio::set_thread_name(thread_data->name());
                for (;;)
                {
                    std::function<void(JobThreadData*)> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                        if (this->stop && this->tasks.empty())
                            break;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    task(thread_data.get());
                }
                thread_data->finz();
            });
    }
    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F(Args...)>::type>
    {
        using return_type = typename std::invoke_result<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type(size_t)>>(
            std::bind(std::forward<F>(f), std::placeholders::_1, std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            // don't allow enqueueing after stopping the pool
            if (stop)
                throw std::runtime_error("enqueue on stopped executor");

            tasks.emplace([task](JobThreadData* thread_data) { (*task)(thread_data); });
        }
        condition.notify_one();
        return res;
    }

    template <class F, class... Args>
    void enqueue_v(F&& f, Args&&... args)
    {
        auto task = std::bind(std::forward<F>(f), std::placeholders::_1, std::forward<Args>(args)...);

        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            // don't allow enqueueing after stopping the pool
            if (stop)
                throw std::runtime_error("enqueue on stopped executor");

            tasks.emplace(std::move(task));
        }
        condition.notify_one();
    }
    ~JobExecutor()
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers)
            worker.join();
    }

private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> workers;

    // the task queue
    std::queue<std::function<void(JobThreadData*)>> tasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

#pragma endregion

#pragma region JobSystem

static int clampThreads(int nThreads)
{
    if (nThreads <= 0)
    {
#if !defined(__EMSCRIPTEN__) || defined(__EMSCRIPTEN_PTHREADS__)
#    if defined(AX_PLATFORM_PC)
        nThreads = (std::max)(static_cast<int>(std::thread::hardware_concurrency() * 3 / 2), 2);
#    else
        nThreads = (std::clamp)(static_cast<int>(std::thread::hardware_concurrency()) - 2, 2, 8);
#    endif
#else
        AXLOGW("The emscripten pthread not enabled, JobSystem not working");
        nThreads = 0;
#endif
    }

    return nThreads;
}

class MainThreadData : public JobThreadData
{
public:
    const char* name() override { return "axmol-main"; }
};

JobSystem::JobSystem(int nThreads)
{
    nThreads = clampThreads(nThreads);
    std::vector<std::shared_ptr<JobThreadData>> tdds;
    for (auto i = 0; i < nThreads; ++i)
        tdds.emplace_back(std::make_shared<JobThreadData>());

    init(tdds);
}

JobSystem::JobSystem(std::span<std::shared_ptr<JobThreadData>> tdds)
{
    init(tdds);
}

void JobSystem::init(const std::span<std::shared_ptr<JobThreadData>>& tdds)
{
    _mainThreadData = new MainThreadData();
    if (!tdds.empty())
        _executor = new JobExecutor(tdds);
}

JobSystem::~JobSystem()
{
    if (_executor)
        delete _executor;
    delete _mainThreadData;
}

void JobSystem::enqueue_v(std::function<void(JobThreadData*)> task)
{
    if (_executor)
        _executor->enqueue_v(std::move(task));
    else
        task(_mainThreadData);
}

void JobSystem::enqueue(std::function<void()> task)
{
    if (_executor)
        this->enqueue(task, nullptr);
    else
        task();
}

void JobSystem::enqueue(std::shared_ptr<JobThreadTask> task)
{
    auto taskw = [task](JobThreadData* thread_data) {
        if (!task->isRequestCancel())
        {
            task->setThreadData(thread_data);
            task->setState(JobThreadTask::State::Inprogress);
            task->execute();
            task->setState(JobThreadTask::State::Idle);
        }
    };
    if (_executor)
        _executor->enqueue_v(std::move(taskw));
    else
        taskw(_mainThreadData);
}

void JobSystem::enqueue(std::function<void()> task, std::function<void()> done)
{
    if (!task)
        return;
    auto taskw = [task_ = std::move(task), done_ = std::move(done)](JobThreadData*) {
        task_();
        if (done_)
            Director::getInstance()->getScheduler()->runOnAxmolThread(done_);
    };
    if (_executor)
        _executor->enqueue_v(taskw);
    else
        taskw(_mainThreadData);
}

#pragma endregion

NS_AX_END
