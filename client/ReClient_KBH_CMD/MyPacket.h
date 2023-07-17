

#define HEART_BEAT_TIME 10*1000 //心跳包 30s 发送一次


//cmd类型数据
#define PACKET_REQ_CMD 10001//表示cmd类型请求数据 客户端--》服务端
#define PACKET_RLY_CMD 10002//表示cmd类型回复数据 服务端--》客户端
//键盘类型数据
#define PACKET_REQ_KEYBOARD 20001//表示KEYBOARD类型请求数据 客户端--》服务端
#define PACKET_RLY_KEYBOARD 20002//表示KEYBOARD类型回复数据 服务端--》客户端
//屏幕类型数据
#define PACKET_REQ_SCREEN 30001//表示SCREEN类型一帧请求数据 
#define PACKET_RLY_SCREEN 30002//表示SCREEN类型一帧回复数据
//心跳类型数据
#define PACKET_REQ_BEAT 40001//表示BEAT类型一帧请求数据 
#define PACKET_RLY_BEAT 40002//表示BEAT类型一帧回复数据


/*
	拆分了大包之后，接收方如何拼接


*/
#pragma pack(push)//表示原有的结构体大小压栈
#pragma pack(1)
struct MyPacket
{
	unsigned int type;//表示包类型
	unsigned int length;//表示数据长度
	char data[1];//定义1字节的数组， 柔性数组

};

#pragma pack(pop)//表示原有的结构体对齐大小出栈


struct tagScreenData {
	unsigned int nWidth;//表示屏幕宽度数据
	unsigned int nHeight;//表示屏幕高度数据
	char data[1];//定义1字节的数组， 柔性数组


};





//向目标发送一个命令码,,仅仅包含一个命令码，并不包含数据内容
inline bool SendCommand(SOCKET s,unsigned int nCode) 
{
	MyPacket pkt;
	pkt.length = 0;
	pkt.type = nCode;

	//表示向目标发送具体的命令码
	send(s, (const char*)&pkt, sizeof(unsigned int) * 2, 0);
	//这里还需要做个判断

	return true;
}


//向目标发送指定长度的数据
inline bool SendData(SOCKET s, unsigned int nCode, const char* pBuf, int nBufLen)
{
	MyPacket pkt;
	pkt.length = nBufLen;
	pkt.type = nCode;

	if (nBufLen <= 0 || pBuf == NULL || s == INVALID_SOCKET)
	{
		return false;
	}

	
	//表示向目标发送具体的数据，建议使用循环来做，如果分两步执行send，最好，send需要做同步处理
	int nRet=send(s, (const char*)&pkt, sizeof(unsigned int) * 2, 0);
	if (nRet <= 0)
	{
		return false;
	}
	nRet= send(s, (const char*)pBuf, nBufLen, 0);
	if (nRet <= 0)
	{
		return false;
	}

	//return true;




	//=========================================================================

	//int nSendedHandSize;

	////可能是缓冲区溢出导致send函数返回-1

	////对nBufLen 进行判断  限制单次最高可传输长度1024
	//int nSendedSize = 0;

	//while (nBufLen - nSendedSize >= 1024)
	//{
	//	//头部信息
	//	pkt.length = 1024;
	//	nSendedHandSize = 0;
	//	while (nSendedHandSize < sizeof(unsigned int) * 2)
	//	{
	//		int nRet = send(s, (char*)&pkt, (sizeof(unsigned int) * 2 - nSendedHandSize), 0);
	//		if (nRet <= 0)
	//		{
	//			return false;
	//		}
	//		else
	//		{
	//			nSendedHandSize += nRet;
	//		}

	//	}


	//	

	//	//每次最多发送1024字节
	//	int nRet = send(s, (char*)&pBuf + nSendedSize, 1024, 0);
	//	if (nRet <= 0)
	//	{
	//		return false;
	//	}
	//	else
	//	{
	//		nSendedSize += nRet;
	//	}
	//}

	//while (nSendedSize < nBufLen)
	//{

	//	//头部信息
	//	pkt.length = nBufLen - nSendedSize;
	//	nSendedHandSize = 0;
	//	while (nSendedHandSize < sizeof(unsigned int) * 2)
	//	{
	//		int nRet = send(s, (char*)&pkt, (sizeof(unsigned int) * 2 - nSendedHandSize), 0);
	//		if (nRet <= 0)
	//		{
	//			return false;
	//		}
	//		else
	//		{
	//			nSendedHandSize += nRet;
	//		}

	//	}



	//	//发送不足1024字节的部分
	//	int nRet = send(s, (char*)&pBuf + nSendedSize, nBufLen - nSendedSize, 0);
	//	if (nRet <= 0)
	//	{
	//		return false;
	//	}
	//	else
	//	{
	//		nSendedSize += nRet;
	//	}

	//}


	//==================================================================================


	
	//这里还需要做个判断

	return true;
}



//收取指定大小的数据包
inline bool RecvData(SOCKET s,const char* pBuf,int nBufLen)
{
	//因为recv函数可能收不完全，所有需要循环收取
	if (nBufLen <=0 || pBuf==NULL || s == INVALID_SOCKET)
	{
		return false;
	}

	//表示当前收取的字节数
	int nRecvedSize = 0;
	while (nRecvedSize < nBufLen)
	{
		int nRet = recv(s,(char*) pBuf + nRecvedSize, nBufLen - nRecvedSize, 0);
		if (nRet <=0 )
		{
			return false;
		}
		else
		{
			nRecvedSize += nRet;
		}


	}



	return true;
}


