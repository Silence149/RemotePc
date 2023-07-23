#include "IOCPServer.h"


extern CMFCServerDlg* g_pDlg;

class IOCPServer : public IOCPBase
{
public:
	// ������
	void OnConnectionEstablished(SocketContext *sockContext)
	{
		//���Ա����ʱ��socket
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%4d-%02d-%02d %02d:%02d:%02d.%03d��Accept a connection��Current connects��%d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, GetConnectCnt());

		//g_pDlg->m_List ָ��Ի����list�б�
		//sockContext->clientAddr

			//Unicode�汾
		char* strIp = inet_ntoa(sockContext->clientAddr.sin_addr);
		//pSession->strIp = strIp;
		wchar_t wIp[256] = { 0 };
		////��ip��asscil תunicode
		mbstowcs(wIp, strIp, strlen(strIp));

		TCHAR szIpAddr[256] = { 0 };

		_stprintf(szIpAddr, _T("%s:%d"),
			wIp,//ip
			ntohs(sockContext->clientAddr.sin_port));// �˿�


		//�����ݲ��뵽�Ի�����
		int nIndex = g_pDlg->m_List.InsertItem(0, (LPCTSTR)szIpAddr);
		if (nIndex != -1)
		{
			//��socket���б��й�������
			g_pDlg->m_List.SetItemData(nIndex, sockContext->connSocket);
		}

		int n = 0;


	}

	// ���ӹر�
	void OnConnectionClosed(SocketContext *sockContext)
	{
		//SYSTEMTIME sys;
		//GetLocalTime(&sys);
		//printf("%4d-%02d-%02d %02d:%02d:%02d.%03d��A connection had closed��Current connects��%d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, GetConnectCnt());
	}

	// �����Ϸ�������
	void OnConnectionError(SocketContext *sockContext, int error)
	{
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%4d-%02d-%02d %02d:%02d:%02d.%03d��A connection erro�� %d��Current connects��%d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, error, GetConnectCnt());
	}

	// ���������
	void OnRecvCompleted(SocketContext *sockContext, IOContext *ioContext)
	{
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%4d-%02d-%02d %02d:%02d:%02d.%03d��Recv data�� %s \n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, ioContext->wsaBuf.buf);
		//���ݴ����  ioContext->wsaBuf.buf ��
		//��Ӷ԰��Ľ���

	}
	// д�������
	void OnSendCompleted(SocketContext *sockContext, IOContext *ioContext)
	{
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%4d-%02d-%02d %02d:%02d:%02d.%03d��Send data successd��\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		/*
			1.��MFC�б�ѡ��Ҫ������socket
			2.�ӽ����ȡ���Ӧ��sockContext->connSocket
			3.
		*/

		// ��  sockContext->connSocket  







	}

};
