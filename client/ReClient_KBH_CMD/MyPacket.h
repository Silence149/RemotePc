
//cmd��������
#define PACKET_REQ_CMD 10001//��ʾcmd������������ �ͻ���--�������
#define PACKET_RLY_CMD 10002//��ʾcmd���ͻظ����� �����--���ͻ���
//������������
#define PACKET_REQ_KEYBOARD 20001//��ʾKEYBOARD������������ �ͻ���--�������
#define PACKET_RLY_KEYBOARD 20002//��ʾKEYBOARD���ͻظ����� �����--���ͻ���
//������������

#pragma pack(push)//��ʾԭ�еĽṹ���Сѹջ
#pragma pack(1)
struct MyPacket
{
	unsigned int type;//��ʾ������
	unsigned int length;//��ʾ���ݳ���
	char data[1];//����1�ֽڵ����飬 ��������

};

#pragma pack(pop)//��ʾԭ�еĽṹ������С��ջ
