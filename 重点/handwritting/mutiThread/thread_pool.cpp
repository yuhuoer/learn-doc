#include<iostream>
#include<vector>
#include<queue>
#include<mutex>
#include<condition_variable>
#include<thread>
#include<functional>
#include<future>
/*
template<typename T>
class SafeQueue{
public:
    SafeQueue(){};
    ~SafeQueue(){};
    bool empty(){
        std::unique_lock<std::mutex> lock(m_mtx);
        return m_queue.empty()
    };
    int size(){
        std::unique_lock<std::mutex> lock(m_mtx);
        return m_queue.size();
    }
    void enqueue(T& t){
        std::unique_lock<std::mutex> lock(m_mtx);
        m_queue.push();
    }
    bool dequeue(T& t){
        std::unique_lock<std::mutex> lock(m_mtx);
        if (m_queue.empty())
        {
            return false;
        }
        t = std::move(m_queue.front());//将任务队列中头部任务通过右值引用传递
        m_queue.pop();
        return true; 
    }
private:
    std::queue<T> m_queue;
    std::mutex m_mtx;

};
*/


class ThreadPool{
public:
    explicit ThreadPool(int thread_nums) : m_stop(false){
        for (int i = 0; i < thread_nums; i++)
        {
            m_works.emplace_back([this]{
                for (;;)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> ul(m_mtx);
                        m_cv.wait(ul, [this]{return m_stop || !m_tasks.empty();}); //停止时或者任务队列为空时
                        if(m_stop && m_tasks.empty()) return;
                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                    }
                }
                
            });
        }
        
    }
    ~ThreadPool(){
        {
            std::unique_lock<std::mutex> ul(m_mtx);
            m_stop = true;
        }
        m_cv.notify_all();
        for(auto& woker : m_works)
        {
            woker.join();
        }
    }
    template<typename F, typename... Args>//可变参模板函数  F为传入的函数 Args可变长参数包
    auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>{  //函数声明 auto identifier ( argument-declarations... ) -> return_type 返回类型为future，future模板类型为传入函数f的类型
        // Create a function with bounded parameters ready to execute
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        // Encapsulate it into a shared ptr in order to be able to copy construct / assign 
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

        std::function<void()> wrapper_func = [task_ptr](){(*task_ptr)();};
        m_tasks.push(wrapper_func);
        m_cv.notify_one();
        return task_ptr->get_future();
    }


private:
    bool m_stop;
    std::vector<std::thread> m_works;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_mtx;
    std::condition_variable m_cv;

};


int main(int argc, char const *argv[])
{
    ThreadPool pool(4);
    std::vector<std::future<int>> results;
    for(int i = 0; i < 8; ++i) {
        results.emplace_back(pool.submit([]() {
            // computing task and return result
        }));
    }
    for(auto && result: results)
        std::cout << result.get() << ' ';
    return 0;
}
