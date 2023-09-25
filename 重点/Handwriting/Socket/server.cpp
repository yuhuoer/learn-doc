#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib") //链接ws2_32.lib这个库


using namespace std;


int main(int argc, char const *argv[])
{

	//初始化WSA
	WORD socketVersion=MAKEWORD(2,2);// makeword是将两个byte型合并成一个word型，一个在高8位，一个在低8位。这句话的意思是调用版本2.2
	WSADATA wsaData; //WSADATA结构体变量的地址值
	if (WSAStartup(socketVersion, &wsaData)!=0)//打开网络库/启动网络库，启动了这个库，这个库里的函数/功能才能使用。使用socket之前调用WSAStartup，后面程序就可以调用socket库中的其他socket函数了。
	{
		cout << "WSAStartup() error!" << endl;
		return 0;
	}
	SOCKET tcp_server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//创建套接字 AF_INET：ipv4 SOCK_STREAM:流式套接字 IPPROTO_TCP:TCP PPTOTO_UDP:UDP
	if (tcp_server_socket==INVALID_SOCKET)
	{
		cout << "socket error!" << endl;
	}
	//绑定IP和端口
	sockaddr_in sin;//ipv4的指定方法是使用struct sockaddr_in类型的变量
	sin.sin_family = AF_INET;//协议族选择ipv4
	sin.sin_port = htons(8888);//设置端口。htons将主机的unsigned short int转换为网络字节顺序
	sin.sin_addr.S_un.S_addr = INADDR_ANY;//IP地址设置成INADDR_ANY，让系统自动获取本机的IP地址
	//bind函数把一个地址族中的特定地址赋给scket
	if (bind(tcp_server_socket,(LPSOCKADDR)&sin,sizeof(sin))==SOCKET_ERROR)
	{
		cout << "bind error!" << endl;;
	}
	//开始监听
	if (listen(tcp_server_socket,5)==SOCKET_ERROR) //backlog: socket全连接队列长度
	{
		cout << "listen error!" << endl;
		return -1;
	}

	//循环接收数据
	SOCKET sclient;
	sockaddr_in remoteAddr;
	int nAddrlen=sizeof(remoteAddr);
	char revData[255];
	while (true)
	{
		cout << "等待连接..." << endl;
		sclient = accept(tcp_server_socket,(sockaddr *)& remoteAddr,&nAddrlen);
		if (sclient==INVALID_SOCKET)
		{
			cout << "accpet error!" << endl;
			continue;
		}
		cout << "接收到一个连接：" << inet_ntoa(remoteAddr.sin_addr) << endl;
		//接收数据
		int ret = recv(sclient,revData,255,0);
		if (ret>0)
		{
			revData[ret]=0x00;
			cout << revData << endl;
		}
		//发送数据
		const char* sendData = "你好,TCP客户端!\n";
		send(sclient,sendData,strlen(sendData),0);
		closesocket(sclient);		
		
	}
	closesocket(sclient);
	WSACleanup();
	return 0;
}

