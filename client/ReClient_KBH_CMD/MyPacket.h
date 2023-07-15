
//cmd类型数据
#define PACKET_REQ_CMD 10001//表示cmd类型请求数据 客户端--》服务端
#define PACKET_RLY_CMD 10002//表示cmd类型回复数据 服务端--》客户端
//键盘类型数据
#define PACKET_REQ_KEYBOARD 20001//表示KEYBOARD类型请求数据 客户端--》服务端
#define PACKET_RLY_KEYBOARD 20002//表示KEYBOARD类型回复数据 服务端--》客户端
//键盘类型数据

#pragma pack(push)//表示原有的结构体大小压栈
#pragma pack(1)
struct MyPacket
{
	unsigned int type;//表示包类型
	unsigned int length;//表示数据长度
	char data[1];//定义1字节的数组， 柔性数组

};

#pragma pack(pop)//表示原有的结构体对齐大小出栈
