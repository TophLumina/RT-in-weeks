#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

// TODO:: Refine the ThreadPool using shared_mutex

class ThreadPool
{
private:
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_stop;

public:
    ThreadPool(size_t threads = std::thread::hardware_concurrency())
        : m_stop(false)
    {
        for (size_t i = 0; i < threads; ++i)
        {
            m_workers.emplace_back([this]
            {
                while (true)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->m_mutex);
                        this->m_condition.wait(lock, [this] { return this->m_stop || !this->m_tasks.empty(); });
                        if (this->m_stop && this->m_tasks.empty())
                            return;
                        task = std::move(this->m_tasks.front());
                        this->m_tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    template <typename F, typename... Args>
    auto Submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
    {
        using return_type = decltype(f(args...));
        auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<return_type> result = task->get_future();
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_stop)
                throw std::runtime_error("Submit on stopped ThreadPool");
            m_tasks.emplace([task]() { (*task)(); });
        }
        m_condition.notify_one();
        return result;
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_stop = true;
        }
        m_condition.notify_all();
        for (std::thread& worker : m_workers)
            worker.join();
    }
};