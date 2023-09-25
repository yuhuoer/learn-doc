#include<iostream>
#include<vector>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<queue>
#include<functional>
#include<chrono>

using namespace std;
//需要一个任务队列 ， 线程池， 锁， 条件变量 线程池停止标志位
template<typename Task>
class ThreadPool{
public:
    ThreadPool(size_t thread_nums){
        for (size_t i = 0; i < thread_nums; i++)
        {
            m_threads.emplace_back([this](){
                for (;;)
                {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lock(m_mutex);
                        m_cv.wait(lock,[this](){return m_stop || !m_tasks.empty();});
                        if (m_stop && m_tasks.empty())
                        {
                            return;
                        }
                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                        
                    }
                    {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    task();
                    }

                    
                }
                
            });
        }
    }

    ~ThreadPool(){
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_stop = true;
        }
        m_cv.notify_all();
        for (size_t i = 0; i < m_threads.size(); i++)
        {
            m_threads[i].join();
        }
    }

    void submit(Task task){
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_stop)
            {
                throw std::runtime_error("submit on a stopped threadpool");
            }
            m_tasks.emplace(task);
        }
        m_cv.notify_one();
    }

    void wait(){

    }

private:
    std::vector<std::thread> m_threads;
    std::queue<Task> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_stop=false;

};


int main(int argc, char const *argv[])
{
    ThreadPool<std::function<void(void)>> threadpool(4);
    cout << std::this_thread::get_id()<<endl;
    for (int i = 0; i < 1000; i++)
    {
        threadpool.submit([i](){cout << "任务" << i <<"运行在"<< std::this_thread::get_id() <<endl;});
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}
