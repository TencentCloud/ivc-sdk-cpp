#ifndef _IVCP_CACHEQUEUE_H__
#define _IVCP_CACHEQUEUE_H__

#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>

template<typename T>
class CacheQueue
{
public:
    CacheQueue(int size) { _maxSize = size; }
    ~CacheQueue() {}
public:
    // case 1: condition variable
    // product
    void add(T obj) {
        std::unique_lock<std::mutex> lk(_mtx);
        if (_pool.size() >= _maxSize)
        {
        }
        else
        {
            _pool.push_back(obj);
        }

        //std::cout << "product object, ID:" << obj->getID() << std::endl;
        lk.unlock();
        _condConsumer.notify_one();
    }
    // consume
    T pop()
    {
        std::unique_lock<std::mutex> lk(_mtx);
        while (_pool.empty()) {
            _condConsumer.wait(lk);
        }
        T obj = _pool.back();
        _pool.pop_back();
        return obj;
    }

    void cexit() {
        std::unique_lock<std::mutex> lk(_mtx);

        _pool.push_back(nullptr);

        //std::cout << "product object, ID:" << obj->getID() << std::endl;
        lk.unlock();
        _condConsumer.notify_one();
    }


private:
    int _maxSize;
    std::list<T> _pool;
    std::condition_variable _condConsumer;
    std::mutex _mtx;
};

#endif