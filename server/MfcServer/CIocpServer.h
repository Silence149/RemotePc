#pragma once

//��IOCP�ķ�װ

//ÿ�ν������ݻ���������С
#define RECV_BUF_SIZE 32*1024

#include "Buffer.h"
#include <string>
#include <mutex>

class CScreenDlg;

/*
	Screen�Ի�������ʱ���ڴ�й¶�� ��
	�ͻ���ͻȻ�ҵ�����������ʾ�Ƴ��ˣ�session�Ƿ�Ҳ�ͷ��ˣ�

*/

//����һ���ṹ������ʾ��ǰ���������� session�Ự
struct MySession
{
	SOCKET client;//��ʾ�ͻ�����socket
	sockaddr_in addr;
	DWORD dwLastTickout;//�ͻ���ͨ������Ծʱ�䣬����������������
	std::string strIp;//��ʾ�ͻ���ip��ַ

	CScreenDlg* pScreenDlg;//���ڼ�¼��ǰsession����Ļ�Ի���

	//��һ�����ջ��������� 32k  ���հ���ʱ����ʱ�Ļ�������
	char szRecvBuf[RECV_BUF_SIZE];


	//���ջ�����  ��ǰ����Ự���˶������ݣ�ֱ��һ������һ�����������ݰ��Ž���ȡ������
	CBuffer recvBuf;

	//���ͻ�����
	CBuffer sendBuf;

	//���ͻ�������ͬ��������  Ϊ��֧�ֶ��̷߳�����������Ҫͬ��
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

	//����
	BOOL Create();

	//��
	BOOL Bind(SOCKET sClient);

	//����
	BOOL Destory();

	//�ṩһ���̻߳ص� ԭ��û��thisָ�� ������staticȡ��thisָ��
	static DWORD ThreadFunc(LPVOID lpParam);

	//Ͷ��һ����������
	BOOL PostRecv(MySession* pSession);

	//��ͬ���ķ����ĳ��첽�ķ���

	BOOL PostSend(MySession* pSession);

	//��Ŀ�귢��nType���͵İ�
	BOOL SendData(MySession* pSession , unsigned int nType ,char* pData = NULL , int nLength = 0);



private:
	HANDLE m_hIocp;
	//�̳߳�����  ���Զ�̬��ȡ
	HANDLE m_hThread[16];

};

