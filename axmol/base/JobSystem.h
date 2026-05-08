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

#pragma once

#include <atomic>
#include <chrono>
#include <exception>
#include <functional>
#include <vector>
#include <memory>
#include <string>
#include <span>
#include "axmol/base/Config.h"
#include "axmol/platform/PlatformDefine.h"

namespace ax
{
/**
 * @addtogroup base
 * @{
 */

class JobExecutor;
class JobSystem;
struct JobState;

class JobThreadData
{
public:
    virtual ~JobThreadData() {}
    virtual void init() {}
    virtual void finz() {}
    virtual const char* name() { return "axmol"; }
};

class JobThreadTask
{
    friend class JobSystem;

public:
    enum class State
    {
        Idle,
        Inprogress,
        RequestCancel,
    };
    virtual ~JobThreadTask() {}
    bool isInprogress() const { return _taskState.load(std::memory_order_acquire) == State::Inprogress; }
    bool isIdle() const { return _taskState.load(std::memory_order_acquire) == State::Idle; }
    bool isRequestCancel() const { return _taskState.load(std::memory_order_acquire) == State::RequestCancel; }
    void cancel()
    {
        if (isInprogress())
            _taskState.store(State::RequestCancel, std::memory_order_release);
    }

    JobThreadData* getThreadData() const { return _threadData.load(std::memory_order_acquire); }

protected:
    void setState(State state) { _taskState.store(state, std::memory_order_release); }
    void setThreadData(JobThreadData* threadData) { _threadData.store(threadData, std::memory_order_release); }

    virtual void execute() {}

    std::atomic<State> _taskState{State::Idle};
    std::atomic<JobThreadData*> _threadData{nullptr};
};

enum class JobStatus
{
    /// The job has been accepted by the queue but has not started.
    Queued,
    /// The job is currently executing on a worker thread or the synchronous fallback thread.
    Running,
    /// The job finished normally.
    Completed,
    /// Cancellation was requested before or during execution.
    Canceled,
    /// The job function threw an exception. Use JobHandle::exception() to inspect it.
    Failed,
};

/**
 * @brief Lightweight handle returned when a job is submitted to JobSystem.
 *
 * JobHandle is a shared, copyable view of a job's completion state. It can be ignored for fire-and-forget work, or kept
 * by native code that needs to wait for completion, poll status, or request cooperative cancellation.
 *
 * @note Cancellation is cooperative. A queued job may be skipped, but a running job must check the cancellation state
 *       and return by itself.
 * @warning Avoid calling wait() from a JobSystem worker thread when the waited job may be queued behind the current
 *          worker. The current executor does not steal queued work while waiting.
 * @warning This is a native C++ synchronization object. It is not intended to be exposed directly to scripting bindings.
 */
class AX_API JobHandle
{
public:
    JobHandle() = default;

    /// Returns true if this handle refers to a submitted job.
    bool valid() const;
    /// Returns true once the job reaches Completed, Canceled, or Failed.
    bool isDone() const;
    /// Returns true while the job function is currently executing.
    bool isRunning() const;
    /// Returns true if cooperative cancellation has been requested.
    bool isCancelRequested() const;
    /// Returns the current job status, or JobStatus::Canceled for an invalid handle.
    JobStatus status() const;
    /// Returns the exception captured from the job function, if status() is JobStatus::Failed.
    std::exception_ptr exception() const;

    /**
     * @brief Request cooperative cancellation.
     *
     * If the job is still queued, it is marked Canceled and will not execute. If it is already running, the job function
     * must observe cancellation through its own captured state or through this handle and return voluntarily.
     *
     * @return false if the handle is invalid, otherwise true.
     */
    bool requestCancel() const;
    /// Alias for requestCancel().
    bool cancel() const { return requestCancel(); }
    /// Block until the job reaches Completed, Canceled, or Failed.
    void wait() const;
    /// Block until the job reaches a terminal state or until the timeout expires.
    bool waitFor(std::chrono::milliseconds timeout) const;

private:
    friend class JobExecutor;
    friend class JobSystem;

    explicit JobHandle(std::shared_ptr<JobState> state);

    std::shared_ptr<JobState> _state;
};

/**
 * @brief Thread-pool based background job system used by Axmol.
 *
 * JobSystem owns a small worker pool and executes submitted jobs off the Axmol thread when worker threads are available.
 * If the platform or configuration creates the JobSystem with no workers, jobs run synchronously on the caller thread
 * with a main-thread JobThreadData fallback.
 *
 * The preferred entry point is enqueue(std::function<void()>). Use enqueue(std::function<void(JobThreadData*)>) when a
 * job needs per-worker data, and enqueue(std::shared_ptr<JobThreadTask>) for legacy task-object code.
 *
 * @note JobSystem is intentionally a low-level background execution API. For the common "run work in the background,
 *       then continue on the Axmol thread" pattern, prefer Director::runAsync().
 * @warning Jobs must not call rendering, scene graph, or other Axmol-thread-only APIs directly unless the specific API
 *          is documented as thread-safe.
 * @warning Waiting for another JobSystem job from inside a worker can deadlock when all workers are occupied.
 */
class AX_API JobSystem
{
public:
    JobSystem(int nThreads = -1);
    JobSystem(std::span<std::shared_ptr<JobThreadData>> tdds);
    ~JobSystem();

    /**
     * @brief Enqueue a job that receives the worker thread's JobThreadData.
     *
     * Use this overload for jobs that need per-worker context initialized by JobThreadData::init(), such as renderer,
     * decoder, or platform resources that should be reused by a worker thread.
     *
     * @param task Function executed by the JobSystem. In the synchronous fallback path, it receives the main-thread
     *             JobThreadData instance.
     * @return A handle that can be ignored, polled, waited, or used for cooperative cancellation.
     */
    JobHandle enqueue(std::function<void(JobThreadData*)> task);

    /**
     * @brief Enqueue a regular background job.
     *
     * This is the default overload for fire-and-forget work. Keep the returned handle only when native code needs to
     * observe completion, wait for the job, or request cooperative cancellation.
     *
     * @param task Function executed by the JobSystem.
     * @return A handle that can be ignored, polled, waited, or used for cooperative cancellation.
     */
    JobHandle enqueue(std::function<void()> task);

    /**
     * @brief Enqueue a JobThreadTask object.
     *
     * This overload preserves the legacy JobThreadTask model for code that stores task state in a polymorphic task
     * object. New code should generally prefer the std::function overloads and keep the returned JobHandle when
     * observation is needed.
     *
     * @param task Task object whose execute() method is invoked by the JobSystem.
     * @return A handle that observes the submitted task.
     */
    JobHandle enqueue(std::shared_ptr<JobThreadTask> task);

protected:
    void init(const std::span<std::shared_ptr<JobThreadData>>& tdds);

private:
    JobExecutor* _executor{nullptr};
    JobThreadData* _mainThreadData{nullptr};
};

// end of base group
/** @} */

}  // namespace ax
