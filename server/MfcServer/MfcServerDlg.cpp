
// MfcServerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MfcServer.h"
#include "MfcServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMfcServerDlg 对话框



CMfcServerDlg::CMfcServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMfcServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_Lst);
}

BEGIN_MESSAGE_MAP(CMfcServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()






DWORD CMfcServerDlg::AcceptFuncThread(LPVOID lpParam) {

	CMfcServerDlg* pThis = (CMfcServerDlg*)lpParam;
	//3. accept 接受请求 等待别人连接
	//SOCKET NewConnection;//返回的socket 专门用于与客户端通信，一般这里开线程循环接受
	while (true)
	{

		sockaddr_in ClientAddr = { 0 };
		ClientAddr.sin_family = AF_INET;
		int ClientAddrLen = sizeof(ClientAddr);
		SOCKET NewConnection = accept(pThis->m_hAcceptSocket, (sockaddr*)&ClientAddr, &ClientAddrLen);
		if (NewConnection == SOCKET_ERROR)
		{
			return 0;
		}

		//到这里表示当前有人连接我，记录当前连接我的人是谁，并显示到界面
		//需要记录一个结构，来表示连接我的人

		//创建一个结构体，需要用堆来处理，记录当前连接人的数据
		MySession* pSession = new MySession;
		pSession->client = NewConnection;//保存客户端socket
		memcpy(&(pSession->addr), &ClientAddr, sizeof(ClientAddr));
		pSession->dwLastTickout = GetTickCount();//表示我们当前时间



		//我们需要把他锁起来  这里的{}表示块作用域
		{
			//以后凡是需要对map进行增删改查都需要加上这样一句话
			//每次只允许一个线程访问
			std::lock_guard<std::mutex> lg(pThis->m_AcceptMtx);

			/*
			pThis->m_AcceptMtx.lock();
			//需要锁的资源
			pThis->m_AcceptMtx.unlock();
			*/

			// 对map进行插入操作
			pThis->m_map.insert(std::pair<SOCKET, MySession*>(NewConnection, pSession));

			//这里需要开启一个线程来收数据，便于以后处理
		}

		//两种写法，一种通过api转化  一种改变工程属性

		//Unicode版本
		char* strIp = inet_ntoa(ClientAddr.sin_addr);
		//pSession->strIp = strIp;
		wchar_t wIp[256] = {0};
		////将ip从asscil 转unicode
		mbstowcs(wIp, strIp, strlen(strIp));

		TCHAR szIpAddr[256] = { 0 };

		_stprintf(szIpAddr, _T("%s:%d"),
			wIp,//ip
		ntohs(ClientAddr.sin_port));// 端口

		/*
		//asscil版本

		char szIpAddr[256] = { 0 };
		
		sprintf(szIpAddr, "%s:%d",
		inet_ntoa(ClientAddr.sin_addr),//ip
		ntohs(ClientAddr.sin_port));// 端口
		
		
		*/

	


		//将数据插入到对话框中
		pThis->m_Lst.InsertItem(pThis->m_map.size(),(LPCTSTR)szIpAddr);


	//开启线程，显示收到的数据
		

		std::thread thd([&]() {
			//表示子程序新的起点

			
			while (TRUE)
			{
				//这里添加对包的解析

				/*
					先收8字节头部
					再收剩下的部分
				
				*/

				// 直接收取socket的数据，并显示出来（从客户端cmd里转发过来的）
				char szBuf[256] = { 0 };
				DWORD nRead;
				int nRet = recv(NewConnection, szBuf, 255, 0);
				if (nRet == SOCKET_ERROR)
				{
					//表示tcp断开
					return 0;
				}
				else if (nRet > 0)
				{

					//直接打印数据	
					printf(szBuf);
					std::string str = strIp;
					str += ":";
					str += szBuf;
					OutputDebugStringA(str.c_str());
					//MessageBoxA(0, str.c_str(),"strIp",0);
				}



			}
			});

		//接触thd与线程回调函数的绑定
		thd.detach();
		
		



	}



	return 0;
}




bool CMfcServerDlg::InitAcceptSocket() {



	//1. socket 创建套接字 (可以理解成管道的Pipe句柄，用于后续数据传输接口)
	m_hAcceptSocket = socket(
		AF_INET,//INET协议簇
		SOCK_STREAM,//表示使用TCP协议
		0);
	if (m_hAcceptSocket == INVALID_SOCKET)
	{

		return false;

	}

	//监听的端口可以做出配置文件 ini json xml
	//2. bind /listen  绑定/监听 端口
	sockaddr_in addr;
	int Port = 4999;
	addr.sin_family = AF_INET;//协议
	addr.sin_port = htons(Port);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//不限制客户端IP
	int nRet = bind(m_hAcceptSocket, (SOCKADDR*)&addr, sizeof(sockaddr_in));
	if (nRet == SOCKET_ERROR)
	{
		return false;

	}

	// 监听
	nRet = listen(m_hAcceptSocket, 5);
	if (nRet == SOCKET_ERROR)
	{
		return false;
	}

	//开启等待循环
	//开启线程来处理循环

	//使用WindowsAPI创建线程
	//返回的是线程的句柄
	m_hAcceptThd=CreateThread(NULL, // SD
		0,                        // initial stack size
		(LPTHREAD_START_ROUTINE) AcceptFuncThread,    // thread function
		this,                       // thread argument
		0,                    // creation option 填0表示线程立马执行 CREATE_SUSPENDED 表示线程挂起
		0                        // thread identifier
	);

	
	//恢复线程运行
	//ResumeThread(m_hAcceptThd);
	
	




	return true;
}



// CMfcServerDlg 消息处理程序

BOOL CMfcServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	//将网络初始化代码放这里



	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_MINIMIZE);

	// TODO: 在此添加额外的初始化代码

	m_Lst.InsertColumn(0, _T("ip:port"));
	m_Lst.InsertColumn(1, _T("system"));
	m_Lst.InsertColumn(2, _T("位置"));//通过ip反查

	m_Lst.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_Lst.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
	m_Lst.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);

	//设置风格
	m_Lst.SetExtendedStyle(m_Lst.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	//m_Lst.InsertItem(0,_T("127.0.0.1"));

	//开始初始化网络
	bool bRet= InitAcceptSocket();
	if (!bRet)
	{
		return FALSE;
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMfcServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMfcServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMfcServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

