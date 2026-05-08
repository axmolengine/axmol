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

#include "axmol/base/JobSystem.h"
#include "axmol/base/Logging.h"
#include "yasio/thread_name.hpp"

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <stdexcept>

#if defined(__EMSCRIPTEN__)
#    include <emscripten/emscripten.h>
#endif

namespace ax
{

struct JobState
{
    std::atomic<JobStatus> status{JobStatus::Queued};
    std::atomic_bool cancelRequested{false};
    std::exception_ptr exception;
    mutable std::mutex mutex;
    mutable std::condition_variable condition;

    void setStatus(JobStatus value)
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            status.store(value, std::memory_order_release);
        }
        condition.notify_all();
    }

    void setException(std::exception_ptr value)
    {
        std::lock_guard<std::mutex> lock(mutex);
        exception = value;
    }
};

static bool isDoneStatus(JobStatus status)
{
    return status == JobStatus::Completed || status == JobStatus::Canceled || status == JobStatus::Failed;
}

JobHandle::JobHandle(std::shared_ptr<JobState> state) : _state(std::move(state)) {}

bool JobHandle::valid() const
{
    return static_cast<bool>(_state);
}

bool JobHandle::isDone() const
{
    return _state && isDoneStatus(_state->status.load(std::memory_order_acquire));
}

bool JobHandle::isRunning() const
{
    return _state && _state->status.load(std::memory_order_acquire) == JobStatus::Running;
}

bool JobHandle::isCancelRequested() const
{
    return _state && _state->cancelRequested.load(std::memory_order_acquire);
}

JobStatus JobHandle::status() const
{
    return _state ? _state->status.load(std::memory_order_acquire) : JobStatus::Canceled;
}

std::exception_ptr JobHandle::exception() const
{
    if (!_state)
        return nullptr;

    std::lock_guard<std::mutex> lock(_state->mutex);
    return _state->exception;
}

bool JobHandle::requestCancel() const
{
    if (!_state)
        return false;

    _state->cancelRequested.store(true, std::memory_order_release);
    auto expected = JobStatus::Queued;
    if (_state->status.compare_exchange_strong(expected, JobStatus::Canceled, std::memory_order_acq_rel))
        _state->condition.notify_all();

    return true;
}

void JobHandle::wait() const
{
    if (!_state)
        return;

    std::unique_lock<std::mutex> lock(_state->mutex);
    _state->condition.wait(lock,
                           [state = _state] { return isDoneStatus(state->status.load(std::memory_order_acquire)); });
}

bool JobHandle::waitFor(std::chrono::milliseconds timeout) const
{
    if (!_state)
        return true;

    std::unique_lock<std::mutex> lock(_state->mutex);
    return _state->condition.wait_for(
        lock, timeout, [state = _state] { return isDoneStatus(state->status.load(std::memory_order_acquire)); });
}

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

    JobHandle enqueue(std::function<void(JobThreadData*)> task, std::shared_ptr<JobState> state = nullptr)
    {
        if (!state)
            state = std::make_shared<JobState>();

        auto taskw = [state, task = std::move(task)](JobThreadData* thread_data) mutable {
            auto expected = JobStatus::Queued;
            if (!state->status.compare_exchange_strong(expected, JobStatus::Running, std::memory_order_acq_rel))
                return;
            state->condition.notify_all();

            try
            {
                task(thread_data);
                state->setStatus(state->cancelRequested.load(std::memory_order_acquire) ? JobStatus::Canceled
                                                                                        : JobStatus::Completed);
            }
            catch (...)
            {
                state->setException(std::current_exception());
                state->setStatus(JobStatus::Failed);
            }
        };
        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            // don't allow enqueueing after stopping the pool
            if (stop)
                throw std::runtime_error("enqueue on stopped executor");

            tasks.emplace(std::move(taskw));
        }
        condition.notify_one();
        return JobHandle(state);
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
#if !defined(__EMSCRIPTEN__)
#    if defined(AX_PLATFORM_PC)
        nThreads = (std::max)(static_cast<int>(std::thread::hardware_concurrency() * 3 / 2), 2);
#    else
        nThreads = (std::clamp)(static_cast<int>(std::thread::hardware_concurrency()) - 2, 2, 8);
#    endif
#else
#    if defined(__EMSCRIPTEN_PTHREADS__)
        nThreads = EM_ASM_INT(return PThread.unusedWorkers.length);
        AXLOGI("The emscripten pthread enabled, unused workers count:{}", nThreads);
#    else
        nThreads = 0;
        AXLOGW("The emscripten pthread not enabled, JobSystem not working");
#    endif
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
    _workerCount    = static_cast<int>(tdds.size());
    if (!tdds.empty())
        _executor = new JobExecutor(tdds);
}

JobSystem::~JobSystem()
{
    if (_executor)
        delete _executor;
    delete _mainThreadData;
}

JobHandle JobSystem::enqueue(std::function<void()> task)
{
    if (!task)
        return {};

    return enqueue([task = std::move(task)](JobThreadData*) { task(); });
}

JobHandle JobSystem::enqueue(std::function<void(JobThreadData*)> task)
{
    if (_executor)
        return _executor->enqueue(std::move(task));

    auto state  = std::make_shared<JobState>();
    auto handle = JobHandle(state);
    if (handle.isCancelRequested())
    {
        state->setStatus(JobStatus::Canceled);
        return handle;
    }

    state->setStatus(JobStatus::Running);
    try
    {
        task(_mainThreadData);
        state->setStatus(JobStatus::Completed);
    }
    catch (...)
    {
        state->setException(std::current_exception());
        state->setStatus(JobStatus::Failed);
    }
    return handle;
}

#pragma endregion

}  // namespace ax
