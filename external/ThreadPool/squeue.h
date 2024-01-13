#include <queue>
#include <mutex>

template<typename T>
class squeue
{
private:
    std::queue<T> _queue;
    std::mutex mtx;

public:
    squeue() {}
    squeue(squeue &&other) {}
    ~squeue() {}

    bool empty()
    {
        std::unique_lock<std::mutex> lock(mtx);
        return _queue.empty();
    }

    int size()
    {
        std::unique_lock<std::mutex> lock(mtx);
        return _queue.size();
    }

    void enqueue(T &t)
    {
        std::unique_lock<std::mutex> lock(mtx);
        _queue.emplace(t);
    }

    bool dequeue(T &t)
    {
        std::unique_lock<std::mutex> lock(mtx);

        if (_queue.empty())
            return false;
        t = std::move(_queue.front());
        _queue.pop();
        return true;
    }
};