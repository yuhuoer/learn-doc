//创建n个线程依次输出1、2、3、4、5...,共计输出m遍
#include<iostream>
#include<vector>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<algorithm>
#include<functional>

#define thread_nums 10
#define circle_nums 2


std::mutex mtx;
std::condition_variable cv;


volatile int count=1;
volatile int cur_circle=0;



void fun(int num)
{
    while (cur_circle <= circle_nums)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [num]{return count==num;});
        std::cout << num << std::endl;
        count++;
        if (count==(thread_nums+1))
        {
            count=1;
            cur_circle++;
        }
        //std::this_thread::sleep_for(std::chrono::seconds(1));
        cv.notify_all();
    }
    cv.notify_all();
    //std::cout << "进程" << num <<"finish" <<std::endl;
}

int main(int argc, char const *argv[])
{
    std::vector<std::thread> th;
    for (int i = 0; i < thread_nums; i++)
    {
        th.emplace_back(std::thread(fun, i+1));
    }
    std::for_each(th.begin(), th.end(), std::mem_fn(&std::thread::join));
    // for(auto& t : th)
    // {
    //     t.join();
    // }
    std::cout << "finish" <<std::endl;
    
    return 0;
}
