#include<iostream>
#include<assert.h>
//#include<memory>
using namespace std;

template<class T>
class smartPointer{

private:
    T* _ptr;
    size_t* _count;

public:
    //smartPointer()=default;
    //构造函数
    smartPointer(T* p=nullptr)//无参数传递时创建空指针
    {
        _ptr = p;
        if (p==nullptr){
            _count = new size_t(0);//空指针引用计数为0。引用计数需要在堆上申请空间原因：?
        }
        else{
            _count = new size_t(1);
        }
    }
    //拷贝构造函数
    smartPointer(smartPointer& sp){
        _ptr = sp._ptr;
        _count = sp._count;
        (*_count)++;
    }
    //析构函数
    ~smartPointer(){
        if (_ptr==nullptr)
        {
            cout << "指针为空" << endl;
            delete _count;
            return;
        }
        (*_count)--;
        if ((*_count)==0)
        {
            delete _ptr;
            delete _count;
            cout << "智能指针释放" << endl;
        }
        
    }
    size_t use_count(){
        return *_count;
    }
    T* operator ->() {
        assert(_ptr != nullptr);
        return _ptr;
    }
    T& operator *() {
        assert(_ptr != nullptr);
        return *_ptr;
    }
    smartPointer& operator=(smartPointer& sp){
        if (sp._ptr == _ptr)
        {
            return *this;
        }
        //1.解决旧的
        (*_count)--;
        if (*_count==0)
        {
            delete _ptr;
            delete _count;
            cout << "智能指针释放" << endl;
        }
        //2.指向新的
        _ptr = sp._ptr;
        _count = sp._count;
        *(_count)++;
        return *this;
    }

};

int main(int argc, char const *argv[])
{
    smartPointer<string> p1(new string("123"));
    cout << p1.use_count() << endl;
    smartPointer<string> p2;
    //p2 = p1;
    // cout << p2.use_count();


    return 0;
}
