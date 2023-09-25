#include<iostream>

using namespace std;

template<class T>
class unique_ptr{

public:
    unique_ptr(T* data=nullptr):_ptr(data){}
    ~unique_ptr(){del();}
    void reset(T* data){//释放原资源，指向新资源
        del();
        _ptr=data;
    }
    unique_ptr& operator=(unique_ptr&& p):_ptr(p._ptr){p.ptr=nullptr; return this;}
    T* release(){//返回资源，放弃对资源的管理
        T* data=_ptr;
        _ptr=nullptr;
        retunr data;
    }
    T* get(){//获取资源
        return _ptr;
    }
    T& operator* (){
        return *_ptr;
    }

private:
    unique_ptr(const unique_ptr&)=delete;
    unique_ptr& operator= (const unique_ptr&)=delete;
    void del()
    {
        if (_ptr==nullptr) return;
        delete _ptr;
        _ptr=nullptr;
    }

private:
    T* _ptr;

};