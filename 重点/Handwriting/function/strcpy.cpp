#include<stdio.h>
#include<iostream>

//复制字符串 不仅会复制字符串内容外，还会复制字符串的结束符 必须保证dst足够大，能够容纳下src,否则会导致溢出错误。
char *strcpy(char *dst, const char *src) //src const修饰避免函数内修改src
{
    if (dst==NULL || src==NULL)//避免对空指针进行操作
    {
        return NULL;
    }
    char* ret = dst;
    while((*dst++=*src++)!='\0');
    return ret;
    
}



int main(int argc, char const *argv[])
{
    char *a="coda";
    char b[100]="you are the best one.";
    char *p;
    p=strcpy(b+8,a);
    std::cout << p;
    return 0;
}
