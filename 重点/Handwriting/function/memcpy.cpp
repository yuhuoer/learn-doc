#include<stdio.h>

void* memcpy(void* dst, const void* src, int len)//目标地址 源地址 拷贝长度
{
    if (dst==NULL || src==NULL || len<=0)
    {
        return NULL;
    }
    char* pdst = (char*)dst;
    char* psrc = (char*)src;

    if (pdst > psrc && pdst < psrc+len)
    {
        
    } 
}