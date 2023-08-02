深拷贝、浅拷贝、移动构造函数实现

```cpp
class A{
public:
    A(const A& a)
    {
        tmp = a.tmp;
        ptr = (int*)malloc(sizeof(int));
        *ptr = *a.ptr;//深拷贝
    }
    A(A&& a):ptr(a.ptr){
        a.ptr = nullptr;//移动构造函数
    }
private:
    int tmp;
    int* ptr;
};
```

