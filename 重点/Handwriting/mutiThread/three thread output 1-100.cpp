#include<iostream>
#include<thread>
#include<mutex>
#include<vector>
#include<condition_variable>

using namespace std;


volatile int num=1;

int thread_nums=3;
int sum=100;

std::mutex mtx;
std::condition_variable cv;

void fn(int idx)
{
    while (num<=sum)
    {
        unique_lock<std::mutex> lock(mtx);
        cv.wait(lock,[idx]{return num%3==idx;});
        if(num<=sum) cout << num << endl;
        num++;
        cv.notify_all();
    }
    cv.notify_all();
}


int main(int argc, char const *argv[])
{
    vector<thread> vec;
    for (int i = 0; i < thread_nums; i++)
    {
        vec.push_back(thread(fn, i));
    }
    for(auto& iter : vec)
    {
        iter.join();
    }
    
    return 0;
}




