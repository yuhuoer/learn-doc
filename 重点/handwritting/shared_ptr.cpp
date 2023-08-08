#include<iostream>
#include<mutex>
#include<assert.h>


using namespace std;

struct cblock
{
    int shared_count = 1;
    int weak_count = 0;
};

template <class T>
class shared_ptr{
public:
    shared_ptr() noexcept = default;
    explicit shared_ptr(T* data) : _data(data){
        //_shared_ptr_mutex = new mutex(); //线程安全 初始化锁
        if (_data)
        {
            _cblock = new cblock();
        }
    }

    shared_ptr(const shared_ptr& sp) noexcept
        : _data(sp._data)
        , _cblock(sp._cblock){
        //_shared_ptr_mutex = sp._shared_ptr_mutex; //拷贝锁
        if (_cblock)
        {
            //_shared_ptr_mutex->lock();//加锁
            ++_cblock->shared_count;
            //_shared_ptr_mutex->unlock();//解锁
        }
    }
    ~shared_ptr(){
        dec_shared_count();
    }
    shared_ptr& operator=(const shared_ptr& sp) noexcept{
        shared_ptr tmp(sp);
        swap(tmp);
        return *this;
    }
    //右值引用
    shared_ptr& operator=(shared_ptr&& sp) noexcept{
        swap(sp);
        sp.reset();
        return *this;
    }
    T get() const noexcept{
        return _data;
    }
    T& operator->(){
        return _data;
    }
    T* operator*(){
        assert(*this);
        return *_data;
    }
    void reset(T* data){
        dec_shared_count();
        _data = data;
        _cblock = new cblock();
        //_shared_ptr_mutex = new mutex(); //建一把新锁

    }

    



private:
    void dec_shared_count(){
        if (!_cblock)
        {
            return;
        }
        //bool deleteMutex = false; //锁删除标志位
        //_shared_ptr_mutex->lock(); //加锁
        --_cblock->shared_count;
        if (_cblock->shared_count==0)
        {
            delete _data;
            _data = nullptr;
            if (_cblock->weak_count <= 0)
            {
                delete _cblock;
                _cblock = nullptr;
                //deleteMutex = true; //删除锁
            }
        }
        //_shared_ptr_mutex->unlock(); //解锁
        // if (deleteMutex) //删除锁
        // {
        //     delete _shared_ptr_mutex;
        // }
        
    }
    void swap(shared_ptr& sp) noexcept{
        std::swap(_data, sp._data);
        std::swap(_cblock, sp._cblock);
        //std::swap(_shared_ptr_mutex,sp._shared_ptr_mutex); //swap锁
    }

private:
    T* _data;
    cblock* _cblock = nullptr;
    //mutex* _shared_ptr_mutex;

};
