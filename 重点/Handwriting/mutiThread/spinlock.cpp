/*
使用原子类型atomic_flag实现自旋锁。
atomic_flag是一种简单的原子布尔类型，只支持两种操作：
1. test_and_set() 返回该std::atomic_flag对象当前状态，test表示先测试(读取当前atomic_flag的值)并返回个这结果值;set表示将atomic_flag状态设置为ture。
test_and_set函数的返回值与set的结果没有关系，返回值只表示调用test_and_set函数前的atomic_flag当前的状态。调用此函数后atomic_flag状态一定为true。
2. clear() 清除对flag的设置
*/

/*测试使用三个线程同时对一个全局变量进行写操作，在使用自旋锁的情况下，变量输出完全有序，不使用自旋锁情况下变量打印会出现无序*/

#include<iostream>
#include<atomic>
#include<thread>

using namespace std;

class spin_lock{
public:
    spin_lock()=default;
    spin_lock(const spin_lock&) = delete;
    spin_lock& operator=(const spin_lock) = delete;
    void lock()
    {
        while (flag.test_and_set()){}//flag为false才不会阻塞
    }
    void unlock()
    {
        flag.clear();
    }
private:
    atomic_flag flag;

};

int num=0;
spin_lock splock;
void addfun()
{
    for (int i = 0; i < 1000; i++)
    {
        splock.lock();
        ++num;
        cout << num << endl;
        splock.unlock();
    }  
}


int main(int argc, char const *argv[])
{
    thread t1(addfun);
    thread t2(addfun);
    thread t3(addfun);
    t1.join();
    t2.join();
    t3.join();
    return 0;
}
