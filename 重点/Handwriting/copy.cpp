#include<iostream>

using namespace std;



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