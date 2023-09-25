/*
    创建守护线程每隔5s打印内容
*/


#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<sstream>
#include<ctime>
#include<stdio.h>
#include<cstdlib>
#include<sys/stat.h>
#include<time.h>
#include<assert.h>
#include<unistd.h>
#include<signal.h>



void sigint_handler(int signum) {
	fprintf(stdout, "Receive SIGINT signal, qiut!\n");
	_exit(0);
}

int main()
{
    //1. 父进程执行fork后退出
    pid_t m_pid;
    m_pid = fork();
    if (m_pid<0)
    {
        exit(1);//创建失败，进程退出
    }
    else if (m_pid>0)
    {
        exit(0);//父进程退出-->子进程变为孤儿进程
    }

    //2.子进程调用setid创建新的会话
    setsid();//摆脱原会话，自己成为新会话的组长

    //3.调用getcwd()获取当前目录路经，并在子进程中调用chdir函数，让根目录‘/’成为子进程的工作目录
    char m_path[1024];
    if (getcwd(m_path,sizeof(m_path)==NULL))
    {
        perror("get path error!");
        exit(1);
    }
    printf("get path success!");
    chdir("/");//切换根目录为工作目录

    //4.子进程中调用umask函数重设文件掩玛
    umask(0);//0表示可以设置任何权限

    //5.在子进程中关闭不需要的文件描述符
    for (int i = 0; i < getdtablesize(); i++)
    {
        close(i);
        printf("文件描述符关闭成功");
    }

    printf("守护进程构建成功！");


    char buf[]="this is a test demo for deamon...\n";

    while (1)
    {
        //signal(SIGQUIT, sigint_handler);
        FILE* fd = fopen("/home/yuhuoer/test/deamon.log","a");
        assert(fd!=NULL);
        fprintf(fd,buf);
        fclose(fd);
        sleep(5);

    }

}