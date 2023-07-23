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

//线程池
//1.这个线程要处理所有客户端
//2.这个地方的收包一定要注意，不是一口气收完
DWORD CIocpServer::ThreadFunc(LPVOID lpParam)
{
	CIocpServer* pThis = (CIocpServer*)lpParam;
	if (pThis == NULL)
	{
		return 0;
	}

	while (TRUE)
	{
		//从队列里面取数据
		DWORD dwNumberOfBytes = 0;//代表I/O通信的字节数
		DWORD dwCompletionKey = 0;//完成键
		MYOVERLAPPED* pOl;

		//该函数一旦返回，就表示某一种类型的通信（收包或发包）成功了
		//由于这里是多个socket同时都在用这个函数来处理返回数据
		BOOL bRet = GetQueuedCompletionStatus(
			pThis->m_hIocp,       // handle to completion port
			&dwNumberOfBytes,     // bytes transferred  通信字节数
			&dwCompletionKey,  // file completion key  绑定的时候填写的socket
			(LPOVERLAPPED*)&pOl,  // buffer 分配到非交换内存中
			INFINITE         // 表示任务等待需要的时间 INFINITE无限等待
		);
		if (bRet)
		{
			//表示成功了
			if (pOl->nType == TYPE_RECV)
			{
				//表示收包成功了  真正拿到数据
				DWORD dwRecvBytes = 0;

				//这里是核心处理代码
				//我们需要根据不同的客户端socke来决定如何处理

				//socket 不能一口气收完数据，无法确定此时收了多长数据
				//包格式 ： 4字节类型 + 4字节长度 + 具体数据
				//要保证收到完整的包格式，才能处理

				//根据对应的socket 找到我们对应的pSession  （根据hash map   键值对查找）
				//我们需要把每一次收到的数据缓存起来，直到收到一个完整的包格式。最后才去处理
				auto item = g_pDlg->m_map.find(dwCompletionKey);//这里的完成键里面放了绑定时候的socket
				if (item != g_pDlg->m_map.cend())
				{
					//表示能够找到对应的Session
					MySession* pSession = item->second;
					//pSession->recvBuf.GetBufferLen() > sizeof(unsigned int)*2) 表示我们包的头部有了
					if (pSession != NULL && dwNumberOfBytes > 0)
					{
						//1.将当前收到的数据放入缓存区
						/*这里不做同步，是因为我们安排单次只会有一个recv请求*/
						pSession->recvBuf.Write((PBYTE)pSession->szRecvBuf, dwNumberOfBytes);

						//2.判断当前缓存区是否已经收到
						//2.a 首先判断有没有收齐前8个字节  允许包只有包头部，没有具体数据
						while (pSession->recvBuf.GetBufferLen() >= sizeof(unsigned int)*2)
						{
							//2.b 一旦收齐了前面8个字节，就可以用来判断是否收齐，后面的真正数据
							MyPacket* pkt =(MyPacket*) pSession->recvBuf.GetBuffer();
							if (pSession->recvBuf.GetBufferLen() - sizeof(unsigned int) * 2 >= pkt->length)
							{
								//到这里表示已经收到了完整数据包，可以开始处理了

								//首先从缓冲区中获取前面的8个字节
								MyPacket head;
								char* pTmpRecvBuf = NULL;

								pSession->recvBuf.Read((PBYTE)&head, sizeof(unsigned int) * 2);

								//pkt->length 为0时说明后面没有数据
								//表示尾部还有数据需要继续收取
								if (head.length !=0)
								{

									//再接收实际的数据

									/*
									pTmpRecvBuf = new char[head.length];

									if (pTmpRecvBuf == NULL)
									{
										//表示已经失败了
										break;
									}

									pSession->recvBuf.Read((PBYTE)pTmpRecvBuf, head.length);
									*/

									//这样写少了内存拷贝，效率更高
									pTmpRecvBuf = (char*) pSession->recvBuf.GetBuffer();

									




									switch (head.type)
									{
									case PACKET_RLY_SCREEN:
									{
										//表示已经从客户端收取了具体的数据可以开始处理了	
										if (pSession->pScreenDlg != NULL)
										{


											//拿获取到的对话框，显示远程发送过来的具体屏幕数据
											if (!pSession->pScreenDlg->IsWindowVisible())
											{
												pSession->pScreenDlg->ShowWindow(SW_SHOWNORMAL);
											}
											//将内容画上我们的对话框
											pSession->pScreenDlg->ShowScreen(pTmpRecvBuf, head.length);


											//通知客户端发下一张
											//SendCommand(pSession->client, PACKET_REQ_SCREEN);

											//可以支持多个线程任意发包   （单一客户端一次发的请求只能一个）
											pThis->SendData(pSession, PACKET_REQ_SCREEN);
										}



									}
									break;

									case PACKET_REQ_BEAT:
									{
										//表示收到心跳包，开始回复数据
										//SendCommand(pSession->client, PACKET_RLY_BEAT);

										pThis->SendData(pSession, PACKET_RLY_BEAT);

									}
									break;


									default:
										break;
									}

									//这样写少了内存拷贝，效率更高
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
							else//代表只收齐了包头部，后面的数据还没有收齐
							{
								break;
							}



						}
				







						//如果发现没有收齐前面的头部，则应该让线程执行任务，直到下一次收齐为止
						//这里还需要再次投递数据,如果忽略这一句，则表示没办法继续收包
						pThis->PostRecv(pSession);
					}
					
				}


			}
			else if (pOl->nType == TYPE_SEND)
			{
				//表示发包,成功发送了数据  长度：dwNumberOfBytes 
				//表示工作线程（线程池）已经把我们缓冲区里面的dwNumberOfBytes数据已经发出去了

				auto item = g_pDlg->m_map.find(dwCompletionKey);
				if (item != g_pDlg->m_map.cend()) 
				{
					MySession* pSession = item->second;
					if (pSession != NULL && dwNumberOfBytes > 0)
					{
						std::lock_guard<std::mutex> lg(pSession->m_sendBufMtx);

						//把已经传送的数据从发送缓存区中删除
						pSession->sendBuf.Delete(dwNumberOfBytes);

						//再次判断发送缓冲区中是否还有没有发送的数据，如果有则继续投递发送请求
						if (pSession->sendBuf.GetBufferLen() > 0)
						{
							//接着投递发送请求
							pThis->PostSend(pSession);
						}


					}
				
				}
				

			}

			//最后需要销毁  这个指针是分配在非交换页里面，所有需要及时销毁
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


//创建
BOOL CIocpServer::Create()
{
	//创建IOCP对象
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

	//还需要创建线程池，提供回调函数
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

//绑定
BOOL CIocpServer::Bind(SOCKET sClient)
{

//将对象与IOCP对象进行关联

//每个连接到客户端需要与IOCP绑定

	HANDLE hIOCP = CreateIoCompletionPort(
		(HANDLE)sClient,              // 需要绑定的socket
		m_hIocp,  // handle to I/O completion port
		sClient,        // completion key 用socket当完成键 省的后续找socket是谁
		8 * 2 // number of threads to execute concurrently
	);
	if (hIOCP == NULL)
	{
		return FALSE;
	}


	return TRUE;
}

//投递一个接收请求
BOOL CIocpServer::PostRecv(MySession* pSession)
{

	if (pSession == NULL)
	{
		return FALSE;
	}

	//接下来如果调用WSARecv

	WSABUF wsaRecvBuf;
	wsaRecvBuf.buf = pSession->szRecvBuf;
	wsaRecvBuf.len = RECV_BUF_SIZE;
	DWORD dwRecvBytes = 0;
	DWORD dwFlags = 0;


	/*
		//MyOVERLAPPED  myol;
		不能这样写，这样写是栈变量，需要丢到队列中去，
		出了这个函数就销毁了，必须要使用堆来分配内存
	*/

	MYOVERLAPPED*  myol = new MYOVERLAPPED;
	myol->nType = TYPE_RECV;
	myol->pBuf = pSession->szRecvBuf; //这个缓冲区也需要new出来



	//异步recv的调用  调用完成，相当于客户端向任务队列发送了一个任务 并没有收到包
	int nRet = WSARecv(pSession->client,
		&wsaRecvBuf,
		1,
		&dwRecvBytes,
		&dwFlags,
		&(myol->ol),//必须传入的是微软规定的重叠结构，我们的内容也放在其中
		NULL);
	if (nRet != -1 && GetLastError() != ERROR_IO_PENDING)
	{
		//表示WSARecv出错了
		return FALSE;
	}

	return TRUE;
}


//投递一个发送请求
BOOL CIocpServer::PostSend(MySession* pSession)
{


	if (pSession == NULL)
	{
		return FALSE;
	}

	//接下来如果调用WSASend

	WSABUF wsaSendBuf;
	wsaSendBuf.buf = (char*) pSession->sendBuf.GetBuffer();
	wsaSendBuf.len = pSession->sendBuf.GetBufferLen();
	DWORD dwRecvBytes = 0;
	DWORD dwFlags = 0;



	/*
		//MyOVERLAPPED  myol;
		不能这样写，这样写是栈变量，需要丢到队列中去，
		出了这个函数就销毁了，必须要使用堆来分配内存
	*/

	MYOVERLAPPED*  myol = new MYOVERLAPPED;
	myol->nType = TYPE_SEND;
	//myol->pBuf = pSession->szRecvBuf; //这个缓冲区也需要new出来



	//异步send的调用  调用完成，相当于客户端向任务队列发送了一个任务 并没有收到包
	int nRet = WSASend(pSession->client,
		&wsaSendBuf,
		1,
		&dwRecvBytes,
		dwFlags,
		&(myol->ol),//必须传入的是微软规定的重叠结构，我们的内容也放在其中
		NULL);
	if (nRet != -1 && GetLastError() != ERROR_IO_PENDING)
	{
		//表示WSARecv出错了
		return FALSE;
	}

	return TRUE;
}


//仅仅只支持我们的类型，不包含具体的数据
BOOL CIocpServer::SendData(MySession* pSession, unsigned int nType, char* pData, int nLength)
{
	//这里不是调用send发包，而是将要发送的数据加入到缓冲区里面
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

		//核心代码
		//判断当前是否只有一个请求包  保证每次请求队列里面只有一个包

		//这个代码表示是否是第一次去请求发送数据
		if (pSession->sendBuf.GetBufferLen() == nLength + sizeof(unsigned int) * 2)
		{
			//表示这里之前并没有投递发包请求
			//此时需要去投递一个发包请求
			PostSend(pSession);
		}

	}





	return TRUE;
}




//销毁
BOOL CIocpServer::Destory()
{
	//需要销毁线程,需要让线程自己退出
	//通过PostQueuedCompletionStatus，发送一个为0的任务
	//GetQueuedCompletionStatus就会返回
	//PostQueuedCompletionStatus


	//关闭IOCP的Handle
	CloseHandle(m_hIocp);

	for (size_t i = 0; i < 8*2; i++)
	{
		//强制结束IOCP中的线程池
		TerminateThread(m_hThread[i], 0);
	}


	return TRUE;
}
