#pragma once

//对IOCP的封装

//每次接收数据缓冲区最大大小
#define RECV_BUF_SIZE 32*1024

#include "Buffer.h"
#include <string>
#include <mutex>

class CScreenDlg;

/*
	Screen对话框销毁时候，内存泄露了 ？
	客户端突然挂掉，界面上显示移除了，session是否也释放了？

*/

//建立一个结构体来表示当前的连接数据 session会话
struct MySession
{
	SOCKET client;//表示客户机的socket
	sockaddr_in addr;
	DWORD dwLastTickout;//客户机通信最后活跃时间，后面用于心跳管理
	std::string strIp;//表示客户机ip地址

	CScreenDlg* pScreenDlg;//用于记录当前session的屏幕对话框

	//放一个接收缓冲区数组 32k  （收包的时候临时的缓存区域）
	char szRecvBuf[RECV_BUF_SIZE];


	//接收缓存区  当前这个会话收了多少数据，直到一个收完一个完整的数据包才将他取出处理
	CBuffer recvBuf;

	//发送缓存区
	CBuffer sendBuf;

	//发送缓存区的同步互斥体  为了支持多线程发包，我们需要同步
	std::mutex m_sendBufMtx;

	MySession() {
		pScreenDlg = NULL;

	}

};



class CIocpServer
{
public:
	CIocpServer();
	virtual ~CIocpServer();

	//创建
	BOOL Create();

	//绑定
	BOOL Bind(SOCKET sClient);

	//销毁
	BOOL Destory();

	//提供一个线程回调 原型没有this指针 这里用static取消this指针
	static DWORD ThreadFunc(LPVOID lpParam);

	//投递一个接受请求
	BOOL PostRecv(MySession* pSession);

	//将同步的发包改成异步的发包

	BOOL PostSend(MySession* pSession);

	//向目标发送nType类型的包
	BOOL SendData(MySession* pSession , unsigned int nType ,char* pData = NULL , int nLength = 0);



private:
	HANDLE m_hIocp;
	//线程池数组  可以动态获取
	HANDLE m_hThread[16];

};

