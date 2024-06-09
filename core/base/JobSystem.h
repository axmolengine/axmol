/****************************************************************************

 Copyright (c) 2019-present Axmol Engine
 * contributors (see AUTHORS.md).

 https://axmolengine.github.io/

 Permission is hereby granted, free of charge, to
 * any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the
 * Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished
 * to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be
 * included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY
 * OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

 * ****************************************************************************/

#pragma once

#include <vector>
#include <memory>
#include <string>
#include <span>
#include "base/Config.h"
#include "platform/PlatformDefine.h"

NS_AX_BEGIN

class JobExecutor;
class JobSystem;
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
    bool isInprogress() const { return _taskState == State::Inprogress; }
    bool isIdle() const { return _taskState == State::Idle; }
    bool isRequestCancel() const { return _taskState == State::RequestCancel; }
    void cancel()
    {
        if (isInprogress())
            _taskState = State::RequestCancel;
    }

    JobThreadData* getThreadData() const { return _threadData; }

protected:
    void setState(State state) { _taskState = state; }
    void setThreadData(JobThreadData* threadData) { _threadData = threadData; }

    virtual void execute() {}

    State _taskState{State::Idle};
    JobThreadData* _threadData{nullptr};
};

class AX_API JobSystem
{
public:
    static JobSystem* create(int nthread = -1);
    static JobSystem* create(std::span<std::shared_ptr<JobThreadData>> tdds);
    static void destroy(JobSystem* system);

    JobSystem(int nThreads = -1);
    JobSystem(std::span<std::shared_ptr<JobThreadData>> tdds);
    ~JobSystem();

    void enqueue_v(std::function<void(JobThreadData*)> task);

    void enqueue(std::function<void()> task);
    void enqueue(std::function<void()> task, std::function<void()> done);
    void enqueue(std::shared_ptr<JobThreadTask> task);

 protected:
    void init(const std::span<std::shared_ptr<JobThreadData>>& tdds);

private:
    JobExecutor* _executor{nullptr};
    JobThreadData* _mainThreadData{nullptr};
};

NS_AX_END
