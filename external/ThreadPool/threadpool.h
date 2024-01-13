#include "squeue.h"
#include <utility>
#include <vector>
#include <future>
#include <functional>
#include <condition_variable>

class ThreadPool
{
private:
    class Worker
    {
    private:
        int _id;
        ThreadPool *_pool;

    public:
        Worker(ThreadPool *pool, const int id) : _pool(pool), _id(id){};

        void operator()()
        {
            std::function<void()> func;

            bool dequeued;

            while (!_pool->_shutdown)
            {
                std::unique_lock<std::mutex> lock(_pool->_conditional_mtx);

                if (_pool->_queue.empty())
                    _pool->_conditional_lock.wait(lock);

                dequeued = _pool->_queue.dequeue(func);
                if (dequeued)
                    func();
            }
        }
    };

    bool _shutdown;
    squeue<std::function<void()>> _queue;
    std::vector<std::thread> _worker_threads;
    std::mutex _conditional_mtx;
    std::condition_variable _conditional_lock;

public:
    ThreadPool(const int n_worker_threads = 4) : _worker_threads(std::vector<std::thread>(n_worker_threads)), _shutdown(false) {}
    ~ThreadPool() { shutdown(); }
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool &operator=(ThreadPool &&) = delete;

    void Init()
    {
        for (int i = 0; i < _worker_threads.size(); ++i)
            _worker_threads[i] = std::thread(Worker(this, i));
    }

    void shutdown()
    {
        _shutdown = true;
        _conditional_lock.notify_all();

        for (int i = 0; i < _worker_threads.size(); ++i)
        {
            if (_worker_threads[i].joinable())
                _worker_threads[i].join();
        }
    }

    // Submit a function
    template <typename Func, typename... Args>
    auto submit(Func &&f, Args &&...args) -> std::future<decltype(f(args...))>
    {
        // A function with bounded parameter ready to execute
        std::function<decltype(f(args...))()> func = std::bind(std::forward<Func>(f), std::forward<Args>(args)...);

        // Encapsulate it in a shared pointer to make it able to copy construct
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

        // Warp the package into a void function
        std::function<void()> warpper_func = [task_ptr]()
        { (*task_ptr)(); };

        _queue.enqueue(warpper_func);
        _conditional_lock.notify_one();
        return task_ptr->get_future();
    }
};
