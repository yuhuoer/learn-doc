#include<iostream>
#include<mutex>
#include<condition_variable>

std::condition_variable cv;

std::mutex mymytex;
std::lock_guard<std::mutex> lock(mymytex);
std::lock_guard<std::mutex> lock1(mymytex, std::adopt_lock);
std::unique_lock<std::mutex> ulock(mymytex);

bool pred()
{
    return true;
}

int main(int argc, char const *argv[])
{
    ulock.lock();
    //cv.wait(ulock, pred);
    

    return 0;
}

class Semaphore{

public:
    explicit Semaphore(int count = 0) : _count(count){};
    //v操作，唤醒
    void Signal(){
        std::unique_lock<std::mutex> ulock(_mutex);
        ++_count;
        _cv.notify_one();
    }
    //p操作，阻塞
    void Wait(){
        std::unique_lock<std::mutex> ulock(_mutex);
        _cv.wait(ulock,[=] {return _count > 0;});
        --_count;
    }




private:
    int _count;
    std::condition_variable _cv;
    std::mutex _mutex;
};