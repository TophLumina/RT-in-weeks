#include "ThreadPool.h"

// 声明一个 thread_local 变量来保存当前线程在队列中的索引
// 初始化为 -1 表示不是线程池中的线程
thread_local int tls_queue_index = -1;

ThreadPool::ThreadPool(size_t numThreads) : terminate(false), submit_index(0)
{
    if (numThreads == 0)
        numThreads = 1;

    for (size_t i = 0; i < numThreads; ++i)
    {
        queues.emplace_back(std::make_unique<WorkQueue>());
    }

    for (size_t i = 0; i < numThreads; ++i)
    {
        workers.emplace_back(&ThreadPool::WorkerRoutine, this, i);
    }
}

ThreadPool::~ThreadPool()
{
    terminate = true;
    sleep_cv.notify_all();
    for (std::thread &worker : workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
}

void ThreadPool::WorkerRoutine(size_t index)
{
    tls_queue_index = static_cast<int>(index);
    const size_t numQueues = queues.size();

    while (!terminate)
    {
        std::function<void()> task;

        // 1. 尝试从本地队列 (LIFO 模式有助于缓存热度) 弹出
        {
            std::unique_lock<std::mutex> lock(queues[index]->mtx);
            if (!queues[index]->tasks.empty())
            {
                task = std::move(queues[index]->tasks.back());
                queues[index]->tasks.pop_back();
            }
        }

        // 2. 本地队列为空，尝试进行 Work-Stealing
        if (!task)
        {
            for (size_t i = 1; i < numQueues; ++i)
            {
                size_t steal_index = (index + i) % numQueues;
                std::unique_lock<std::mutex> lock(queues[steal_index]->mtx,
                                                  std::try_to_lock);
                if (lock.owns_lock() && !queues[steal_index]->tasks.empty())
                {
                    // 从别人队列的前端偷取 (FIFO，偷取那些久未执行的数据)
                    task = std::move(queues[steal_index]->tasks.front());
                    queues[steal_index]->tasks.pop_front();
                    break;
                }
            }
        }

        // 3. 如果依然没有任务，进入条件变量休眠
        if (!task)
        {
            std::unique_lock<std::mutex> sleep_lock(sleep_mtx);
            sleep_cv.wait_for(
                sleep_lock, std::chrono::milliseconds(10), [this, index]
                {
                    return terminate ||
                           !queues[index]
                                ->tasks.empty(); // 简单起见采用超时轮询配合信号
                });
            continue;
        }

        // 4. 执行任务
        task();
    }
}
