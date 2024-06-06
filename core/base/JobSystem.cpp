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

JobSystem* JobSystem::create(int nthread)
{
    auto inst = new JobSystem();
    inst->start(nthread);
    return inst;
}

void JobSystem::destroy(JobSystem* inst)
{
    if (inst)
    {
        inst->stop();
        delete inst;
    }
}

JobSystem* JobSystem::create(std::span<std::shared_ptr<JobThreadData>> tdds)
{
    if (!tdds.empty())
    {
        auto inst = new JobSystem();
        inst->start(tdds);
        return inst;
    }
    return nullptr;
}

JobSystem::~JobSystem()
{
    stop();
}

// Call at task collect thread
void JobSystem::start(int nThreads)
{
    if (_executor)
        return;

    if (nThreads < 0)
    {
#if !defined(__EMSCRIPTEN_PTHREADS__)
        nThreads = (std::max)(static_cast<int>(std::thread::hardware_concurrency() * 3 / 2), 1);
#else
        nThreads = 4;
#endif
    }
    std::vector<std::shared_ptr<JobThreadData>> tdds;
    for (auto i = 0; i < nThreads; ++i)
        tdds.emplace_back(std::make_shared<JobThreadData>());

    _executor = new JobExecutor(tdds);
}

void JobSystem::start(std::span<std::shared_ptr<JobThreadData>> tdds)
{
    if (_executor)
        return;

    _executor = new JobExecutor(tdds);
}

// Call at task collect thread
void JobSystem::stop()
{
    if (_executor != nullptr)
    {
        delete _executor;
    }
}

void JobSystem::enqueue_v(std::function<void(JobThreadData*)> task)
{
    _executor->enqueue_v(std::move(task));
}

void JobSystem::enqueue(std::function<void()> task)
{
    _executor->enqueue_v([task_ = std::move(task)](JobThreadData*) { task_(); });
}

void JobSystem::enqueue(std::shared_ptr<JobThreadTask> task)
{
    _executor->enqueue_v([task](JobThreadData* thread_data) {
        if (!task->isRequestCancel())
        {
            task->setThreadData(thread_data);
            task->setState(JobThreadTask::State::Inprogress);
            task->execute();
            task->setState(JobThreadTask::State::Idle);
        }
    });
}

void JobSystem::enqueue(std::function<void()> task, std::function<void()> done)
{
    _executor->enqueue_v([task_ = std::move(task), done_ = std::move(done)](JobThreadData*) {
        task_();
        Director::getInstance()->getScheduler()->runOnAxmolThread(done_);
    });
}

#pragma endregion

NS_AX_END
