#include "IOCPServer.h"


extern CMFCServerDlg* g_pDlg;

class IOCPServer : public IOCPBase
{
public:
	// 新连接
	void OnConnectionEstablished(SocketContext *sockContext)
	{
		//可以保存此时的socket
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%4d-%02d-%02d %02d:%02d:%02d.%03d：Accept a connection，Current connects：%d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, GetConnectCnt());

		//g_pDlg->m_List 指向对话框的list列表
		//sockContext->clientAddr

			//Unicode版本
		char* strIp = inet_ntoa(sockContext->clientAddr.sin_addr);
		//pSession->strIp = strIp;
		wchar_t wIp[256] = { 0 };
		////将ip从asscil 转unicode
		mbstowcs(wIp, strIp, strlen(strIp));

		TCHAR szIpAddr[256] = { 0 };

		_stprintf(szIpAddr, _T("%s:%d"),
			wIp,//ip
			ntohs(sockContext->clientAddr.sin_port));// 端口


		//将数据插入到对话框中
		int nIndex = g_pDlg->m_List.InsertItem(0, (LPCTSTR)szIpAddr);
		if (nIndex != -1)
		{
			//将socket与列表行关联起来
			g_pDlg->m_List.SetItemData(nIndex, sockContext->connSocket);
		}

		int n = 0;


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
