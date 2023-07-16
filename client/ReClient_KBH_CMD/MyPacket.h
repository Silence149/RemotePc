
//cmd��������
#define PACKET_REQ_CMD 10001//��ʾcmd������������ �ͻ���--�������
#define PACKET_RLY_CMD 10002//��ʾcmd���ͻظ����� �����--���ͻ���
//������������
#define PACKET_REQ_KEYBOARD 20001//��ʾKEYBOARD������������ �ͻ���--�������
#define PACKET_RLY_KEYBOARD 20002//��ʾKEYBOARD���ͻظ����� �����--���ͻ���
//��Ļ��������
#define PACKET_REQ_SCREEN 30001//��ʾSCREEN����һ֡�������� 
#define PACKET_RLY_SCREEN 30002//��ʾSCREEN����һ֡�ظ�����




#pragma pack(push)//��ʾԭ�еĽṹ���Сѹջ
#pragma pack(1)
struct MyPacket
{
	unsigned int type;//��ʾ������
	unsigned int length;//��ʾ���ݳ���
	char data[1];//����1�ֽڵ����飬 ��������

};

#pragma pack(pop)//��ʾԭ�еĽṹ������С��ջ


struct tagScreenData {
	unsigned int nWidth;//��ʾ��Ļ�������
	unsigned int nHeight;//��ʾ��Ļ�߶�����
	char data[1];//����1�ֽڵ����飬 ��������


};





//��Ŀ�귢��һ��������,,��������һ�������룬����������������
inline bool SendCommand(SOCKET s,unsigned int nCode) 
{
	MyPacket pkt;
	pkt.length = 0;
	pkt.type = nCode;

	//��ʾ��Ŀ�귢�;����������
	send(s, (const char*)&pkt, sizeof(unsigned int) * 2, 0);
	//���ﻹ��Ҫ�����ж�

	return true;
}


//��Ŀ�귢��ָ�����ȵ�����
inline bool SendData(SOCKET s, unsigned int nCode, const char* pBuf, int nBufLen)
{
	MyPacket pkt;
	pkt.length = nBufLen;
	pkt.type = nCode;

	if (nBufLen <= 0 || pBuf == NULL || s == INVALID_SOCKET)
	{
		return false;
	}

	
	//��ʾ��Ŀ�귢�;�������ݣ�����ʹ��ѭ�����������������ִ��send����ã�send��Ҫ��ͬ������
	send(s, (const char*)&pkt, sizeof(unsigned int) * 2, 0);
	send(s, (const char*)pBuf, nBufLen, 0);


	
	//���ﻹ��Ҫ�����ж�

	return true;
}



//��ȡָ����С�����ݰ�
inline bool RecvData(SOCKET s,const char* pBuf,int nBufLen)
{
	//��Ϊrecv���������ղ���ȫ��������Ҫѭ����ȡ
	if (nBufLen <=0 || pBuf==NULL || s == INVALID_SOCKET)
	{
		return false;
	}

	//��ʾ��ǰ��ȡ���ֽ���
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


