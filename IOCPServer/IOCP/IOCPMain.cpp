// IOCP.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "IOCPBase.h"
#include <stdio.h>
#include <time.h>


#define _WINSOCK_DEPRECATED_NO_WARNINGS

class CMyServer : public IOCPBase
{
public:
	// 新连接
	void OnConnectionEstablished(SocketContext *sockContext)
	{
		//可以保存此时的socket
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%4d-%02d-%02d %02d:%02d:%02d.%03d：Accept a connection，Current connects：%d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, GetConnectCnt());
	}

	// 连接关闭
	void OnConnectionClosed(SocketContext *sockContext)
	{
		//SYSTEMTIME sys;
		//GetLocalTime(&sys);
		//printf("%4d-%02d-%02d %02d:%02d:%02d.%03d：A connection had closed，Current connects：%d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, GetConnectCnt());
	}

	// 连接上发生错误
	void OnConnectionError(SocketContext *sockContext, int error)
	{
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%4d-%02d-%02d %02d:%02d:%02d.%03d：A connection erro： %d，Current connects：%d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, error, GetConnectCnt());
	}

	// 读操作完成
	void OnRecvCompleted(SocketContext *sockContext, IOContext *ioContext)
	{
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%4d-%02d-%02d %02d:%02d:%02d.%03d：Recv data： %s \n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, ioContext->wsaBuf.buf);
		//数据存放在  ioContext->wsaBuf.buf 中
		//添加对包的解析
	
	}
	// 写操作完成
	void OnSendCompleted(SocketContext *sockContext, IOContext *ioContext)
	{
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%4d-%02d-%02d %02d:%02d:%02d.%03d：Send data successd！\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		/*
			1.从MFC列表选中要操作的socket
			2.从界面获取其对应的sockContext->connSocket
			3.
		*/
		
		// 向  sockContext->connSocket  
	
	}

};
	

int main()
{
	CMyServer *pServer = new CMyServer;

	// 开启服务
	if (pServer->Start())
	{
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%4d-%02d-%02d %02d:%02d:%02d.%03d：Server start succussed on port:%d... \n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, pServer->GetPort());
	}
	else
	{
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%4d-%02d-%02d %02d:%02d:%02d.%03d：Server start failed！\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		return 0;
	}



	/*
	1.从MFC列表选中要操作的socket
	2.从界面获取其对应的sockContext->connSocket
	3.调用	pServer->SendData() 向这个socket发送数据

	目前问题：
			1.需要将每次

	*/

	/*
		IOCP类使用的话，在对话框类的初始化  开启服务  保存this指针


	*/


	/*
		通过将对话框的this指针传递给 IOCP类
		使得IOCP类中  有新的连接进来的时候可以将连接的IP:PORT 输出到list控件上
		并且将相关的socket信息绑定到 单行list中，以便后续Menu菜单向选中的socket发送相关命令

	*/

	// 创建事件对象，让ServerShutdown程序能够关闭自己
	HANDLE hEvent = ::CreateEvent(NULL, FALSE, FALSE, L"ShutdownEvent");
	::WaitForSingleObject(hEvent, INFINITE);
	::CloseHandle(hEvent);

	// 关闭服务
	pServer->Stop();
	delete pServer;

	SYSTEMTIME sys;
	GetLocalTime(&sys);
	printf("%4d-%02d-%02d %02d:%02d:%02d.%03d：Server Closed \n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
    return 0;
}

