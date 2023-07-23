#include "pch.h"
#include "CIocpServer.h"
#include "../../client/ReClient_KBH_CMD/MyPacket.h"
#include "MfcServerDlg.h"
#include "CScreenDlg.h"

#include <iostream>

extern CMfcServerDlg* g_pDlg;


CIocpServer::CIocpServer()
{

}


CIocpServer::~CIocpServer()
{

}

//�̳߳�
//1.����߳�Ҫ�������пͻ���
//2.����ط����հ�һ��Ҫע�⣬����һ��������
DWORD CIocpServer::ThreadFunc(LPVOID lpParam)
{
	CIocpServer* pThis = (CIocpServer*)lpParam;
	if (pThis == NULL)
	{
		return 0;
	}

	while (TRUE)
	{
		//�Ӷ�������ȡ����
		DWORD dwNumberOfBytes = 0;//����I/Oͨ�ŵ��ֽ���
		DWORD dwCompletionKey = 0;//��ɼ�
		MYOVERLAPPED* pOl;

		//�ú���һ�����أ��ͱ�ʾĳһ�����͵�ͨ�ţ��հ��򷢰����ɹ���
		//���������Ƕ��socketͬʱ�������������������������
		BOOL bRet = GetQueuedCompletionStatus(
			pThis->m_hIocp,       // handle to completion port
			&dwNumberOfBytes,     // bytes transferred  ͨ���ֽ���
			&dwCompletionKey,  // file completion key  �󶨵�ʱ����д��socket
			(LPOVERLAPPED*)&pOl,  // buffer ���䵽�ǽ����ڴ���
			INFINITE         // ��ʾ����ȴ���Ҫ��ʱ�� INFINITE���޵ȴ�
		);
		if (bRet)
		{
			//��ʾ�ɹ���
			if (pOl->nType == TYPE_RECV)
			{
				//��ʾ�հ��ɹ���  �����õ�����
				DWORD dwRecvBytes = 0;

				//�����Ǻ��Ĵ������
				//������Ҫ���ݲ�ͬ�Ŀͻ���socke��������δ���

				//socket ����һ�����������ݣ��޷�ȷ����ʱ���˶೤����
				//����ʽ �� 4�ֽ����� + 4�ֽڳ��� + ��������
				//Ҫ��֤�յ������İ���ʽ�����ܴ���

				//���ݶ�Ӧ��socket �ҵ����Ƕ�Ӧ��pSession  ������hash map   ��ֵ�Բ��ң�
				//������Ҫ��ÿһ���յ������ݻ���������ֱ���յ�һ�������İ���ʽ������ȥ����
				auto item = g_pDlg->m_map.find(dwCompletionKey);//�������ɼ�������˰�ʱ���socket
				if (item != g_pDlg->m_map.cend())
				{
					//��ʾ�ܹ��ҵ���Ӧ��Session
					MySession* pSession = item->second;
					//pSession->recvBuf.GetBufferLen() > sizeof(unsigned int)*2) ��ʾ���ǰ���ͷ������
					if (pSession != NULL && dwNumberOfBytes > 0)
					{
						//1.����ǰ�յ������ݷ��뻺����
						/*���ﲻ��ͬ��������Ϊ���ǰ��ŵ���ֻ����һ��recv����*/
						pSession->recvBuf.Write((PBYTE)pSession->szRecvBuf, dwNumberOfBytes);

						//2.�жϵ�ǰ�������Ƿ��Ѿ��յ�
						//2.a �����ж���û������ǰ8���ֽ�  �����ֻ�а�ͷ����û�о�������
						while (pSession->recvBuf.GetBufferLen() >= sizeof(unsigned int)*2)
						{
							//2.b һ��������ǰ��8���ֽڣ��Ϳ��������ж��Ƿ����룬�������������
							MyPacket* pkt =(MyPacket*) pSession->recvBuf.GetBuffer();
							if (pSession->recvBuf.GetBufferLen() - sizeof(unsigned int) * 2 >= pkt->length)
							{
								//�������ʾ�Ѿ��յ����������ݰ������Կ�ʼ������

								//���ȴӻ������л�ȡǰ���8���ֽ�
								MyPacket head;
								char* pTmpRecvBuf = NULL;

								pSession->recvBuf.Read((PBYTE)&head, sizeof(unsigned int) * 2);

								//pkt->length Ϊ0ʱ˵������û������
								//��ʾβ������������Ҫ������ȡ
								if (head.length !=0)
								{

									//�ٽ���ʵ�ʵ�����

									/*
									pTmpRecvBuf = new char[head.length];

									if (pTmpRecvBuf == NULL)
									{
										//��ʾ�Ѿ�ʧ����
										break;
									}

									pSession->recvBuf.Read((PBYTE)pTmpRecvBuf, head.length);
									*/

									//����д�����ڴ濽����Ч�ʸ���
									pTmpRecvBuf = (char*) pSession->recvBuf.GetBuffer();

									




									switch (head.type)
									{
									case PACKET_RLY_SCREEN:
									{
										//��ʾ�Ѿ��ӿͻ�����ȡ�˾�������ݿ��Կ�ʼ������	
										if (pSession->pScreenDlg != NULL)
										{


											//�û�ȡ���ĶԻ�����ʾԶ�̷��͹����ľ�����Ļ����
											if (!pSession->pScreenDlg->IsWindowVisible())
											{
												pSession->pScreenDlg->ShowWindow(SW_SHOWNORMAL);
											}
											//�����ݻ������ǵĶԻ���
											pSession->pScreenDlg->ShowScreen(pTmpRecvBuf, head.length);


											//֪ͨ�ͻ��˷���һ��
											//SendCommand(pSession->client, PACKET_REQ_SCREEN);

											//����֧�ֶ���߳����ⷢ��   ����һ�ͻ���һ�η�������ֻ��һ����
											pThis->SendData(pSession, PACKET_REQ_SCREEN);
										}



									}
									break;

									case PACKET_REQ_BEAT:
									{
										//��ʾ�յ�����������ʼ�ظ�����
										//SendCommand(pSession->client, PACKET_RLY_BEAT);

										pThis->SendData(pSession, PACKET_RLY_BEAT);

									}
									break;


									default:
										break;
									}

									//����д�����ڴ濽����Ч�ʸ���
									if (head.length !=0)
									{
										pSession->recvBuf.Delete(head.length);
									}
									/*
									if (pTmpRecvBuf != NULL)
									{
										delete []pTmpRecvBuf;
										pTmpRecvBuf = NULL;
									}
									*/
								


								}
							}
							else//����ֻ�����˰�ͷ������������ݻ�û������
							{
								break;
							}



						}
				







						//�������û������ǰ���ͷ������Ӧ�����߳�ִ������ֱ����һ������Ϊֹ
						//���ﻹ��Ҫ�ٴ�Ͷ������,���������һ�䣬���ʾû�취�����հ�
						pThis->PostRecv(pSession);
					}
					
				}


			}
			else if (pOl->nType == TYPE_SEND)
			{
				//��ʾ����,�ɹ�����������  ���ȣ�dwNumberOfBytes 
				//��ʾ�����̣߳��̳߳أ��Ѿ������ǻ����������dwNumberOfBytes�����Ѿ�����ȥ��

				auto item = g_pDlg->m_map.find(dwCompletionKey);
				if (item != g_pDlg->m_map.cend()) 
				{
					MySession* pSession = item->second;
					if (pSession != NULL && dwNumberOfBytes > 0)
					{
						std::lock_guard<std::mutex> lg(pSession->m_sendBufMtx);

						//���Ѿ����͵����ݴӷ��ͻ�������ɾ��
						pSession->sendBuf.Delete(dwNumberOfBytes);

						//�ٴ��жϷ��ͻ��������Ƿ���û�з��͵����ݣ�����������Ͷ�ݷ�������
						if (pSession->sendBuf.GetBufferLen() > 0)
						{
							//����Ͷ�ݷ�������
							pThis->PostSend(pSession);
						}


					}
				
				}
				

			}

			//�����Ҫ����  ���ָ���Ƿ����ڷǽ���ҳ���棬������Ҫ��ʱ����
			if (pOl !=NULL)
			{
				delete pOl;
				pOl = NULL;
			}


		}
		else
		{
			break;
		}





	}








	return 0;
}


//����
BOOL CIocpServer::Create()
{
	//����IOCP����
	m_hIocp = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE,              // handle to file
		NULL,  // handle to I/O completion port
		0,        // completion key
		8 * 2 // number of threads to execute concurrently
	);
	if (m_hIocp == NULL)
	{
		return FALSE;
	}

	//����Ҫ�����̳߳أ��ṩ�ص�����
	for (int i = 0; i < 8*2; i++)
	{
		m_hThread[i]=CreateThread(NULL,
			0,
			(LPTHREAD_START_ROUTINE)ThreadFunc,
			this,
			0,
			NULL);
	}






	return TRUE;
}

//��
BOOL CIocpServer::Bind(SOCKET sClient)
{

//��������IOCP������й���

//ÿ�����ӵ��ͻ�����Ҫ��IOCP��

	HANDLE hIOCP = CreateIoCompletionPort(
		(HANDLE)sClient,              // ��Ҫ�󶨵�socket
		m_hIocp,  // handle to I/O completion port
		sClient,        // completion key ��socket����ɼ� ʡ�ĺ�����socket��˭
		8 * 2 // number of threads to execute concurrently
	);
	if (hIOCP == NULL)
	{
		return FALSE;
	}


	return TRUE;
}

//Ͷ��һ����������
BOOL CIocpServer::PostRecv(MySession* pSession)
{

	if (pSession == NULL)
	{
		return FALSE;
	}

	//�������������WSARecv

	WSABUF wsaRecvBuf;
	wsaRecvBuf.buf = pSession->szRecvBuf;
	wsaRecvBuf.len = RECV_BUF_SIZE;
	DWORD dwRecvBytes = 0;
	DWORD dwFlags = 0;


	/*
		//MyOVERLAPPED  myol;
		��������д������д��ջ��������Ҫ����������ȥ��
		������������������ˣ�����Ҫʹ�ö��������ڴ�
	*/

	MYOVERLAPPED*  myol = new MYOVERLAPPED;
	myol->nType = TYPE_RECV;
	myol->pBuf = pSession->szRecvBuf; //���������Ҳ��Ҫnew����



	//�첽recv�ĵ���  ������ɣ��൱�ڿͻ�����������з�����һ������ ��û���յ���
	int nRet = WSARecv(pSession->client,
		&wsaRecvBuf,
		1,
		&dwRecvBytes,
		&dwFlags,
		&(myol->ol),//���봫�����΢��涨���ص��ṹ�����ǵ�����Ҳ��������
		NULL);
	if (nRet != -1 && GetLastError() != ERROR_IO_PENDING)
	{
		//��ʾWSARecv������
		return FALSE;
	}

	return TRUE;
}


//Ͷ��һ����������
BOOL CIocpServer::PostSend(MySession* pSession)
{


	if (pSession == NULL)
	{
		return FALSE;
	}

	//�������������WSASend

	WSABUF wsaSendBuf;
	wsaSendBuf.buf = (char*) pSession->sendBuf.GetBuffer();
	wsaSendBuf.len = pSession->sendBuf.GetBufferLen();
	DWORD dwRecvBytes = 0;
	DWORD dwFlags = 0;



	/*
		//MyOVERLAPPED  myol;
		��������д������д��ջ��������Ҫ����������ȥ��
		������������������ˣ�����Ҫʹ�ö��������ڴ�
	*/

	MYOVERLAPPED*  myol = new MYOVERLAPPED;
	myol->nType = TYPE_SEND;
	//myol->pBuf = pSession->szRecvBuf; //���������Ҳ��Ҫnew����



	//�첽send�ĵ���  ������ɣ��൱�ڿͻ�����������з�����һ������ ��û���յ���
	int nRet = WSASend(pSession->client,
		&wsaSendBuf,
		1,
		&dwRecvBytes,
		dwFlags,
		&(myol->ol),//���봫�����΢��涨���ص��ṹ�����ǵ�����Ҳ��������
		NULL);
	if (nRet != -1 && GetLastError() != ERROR_IO_PENDING)
	{
		//��ʾWSARecv������
		return FALSE;
	}

	return TRUE;
}


//����ֻ֧�����ǵ����ͣ����������������
BOOL CIocpServer::SendData(MySession* pSession, unsigned int nType, char* pData, int nLength)
{
	//���ﲻ�ǵ���send���������ǽ�Ҫ���͵����ݼ��뵽����������
	if (pSession == NULL)
	{
		return FALSE;
	}

	MyPacket pkt;
	pkt.length = nLength;
	pkt.type = nType;

	{
		std::lock_guard<std::mutex> lg(pSession->m_sendBufMtx);
		pSession->sendBuf.Write((PBYTE)&pkt,sizeof(unsigned int) * 2);

		if (nLength >0 && pData != NULL)
		{
			pSession->sendBuf.Write((PBYTE)pData, nLength);
		}

		//���Ĵ���
		//�жϵ�ǰ�Ƿ�ֻ��һ�������  ��֤ÿ�������������ֻ��һ����

		//��������ʾ�Ƿ��ǵ�һ��ȥ����������
		if (pSession->sendBuf.GetBufferLen() == nLength + sizeof(unsigned int) * 2)
		{
			//��ʾ����֮ǰ��û��Ͷ�ݷ�������
			//��ʱ��ҪȥͶ��һ����������
			PostSend(pSession);
		}

	}





	return TRUE;
}




//����
BOOL CIocpServer::Destory()
{
	//��Ҫ�����߳�,��Ҫ���߳��Լ��˳�
	//ͨ��PostQueuedCompletionStatus������һ��Ϊ0������
	//GetQueuedCompletionStatus�ͻ᷵��
	//PostQueuedCompletionStatus


	//�ر�IOCP��Handle
	CloseHandle(m_hIocp);

	for (size_t i = 0; i < 8*2; i++)
	{
		//ǿ�ƽ���IOCP�е��̳߳�
		TerminateThread(m_hThread[i], 0);
	}


	return TRUE;
}
