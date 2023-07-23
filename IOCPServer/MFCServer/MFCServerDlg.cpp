
// MFCServerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCServer.h"
#include "MFCServerDlg.h"
#include "afxdialogex.h"

#include "IOCP/IOCPBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//对话框的this指针
CMFCServerDlg* g_pDlg = NULL;


class IOCPServer : public IOCPBase
{
public:
	// 新连接
	void OnConnectionEstablished(SocketContext *sockContext)
	{
		//可以保存此时的socket
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%4d-%02d-%02d %02d:%02d:%02d.%03d：Accept a connection，Current connects：%d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, GetConnectCnt());

		//g_pDlg->m_List 指向对话框的list列表
		//sockContext->clientAddr

			//Unicode版本
		char* strIp = inet_ntoa(sockContext->clientAddr.sin_addr);
		//pSession->strIp = strIp;
		wchar_t wIp[256] = { 0 };
		////将ip从asscil 转unicode
		mbstowcs(wIp, strIp, strlen(strIp));

		TCHAR szIpAddr[256] = { 0 };

		_stprintf(szIpAddr, _T("%s:%d"),
			wIp,//ip
			ntohs(sockContext->clientAddr.sin_port));// 端口


		//将数据插入到对话框中末尾
		int nIndexn = g_pDlg->m_List.GetItemCount(); // 获取当前列表项数量 其第一项为0
		int nIndex = g_pDlg->m_List.InsertItem(nIndexn, (LPCTSTR)szIpAddr);
		if (nIndex != -1)
		{
			//将socket与列表行关联起来
			g_pDlg->m_List.SetItemData(nIndex, sockContext->connSocket);
		}


	}

	// 连接关闭
	void OnConnectionClosed(SocketContext *sockContext)
	{
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%4d-%02d-%02d %02d:%02d:%02d.%03d：A connection had closed，Current connects：%d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, GetConnectCnt());
		
		std::lock_guard<std::mutex> lg(g_pDlg->m_ListMtx);//多线程删除，记得同步

		//界面上也去除这一行
		for (int i = 0; i < g_pDlg->m_List.GetItemCount(); i++)
		{
			//获得对应在界面的行数
			if (g_pDlg->m_List.GetItemData(i) == 0)
			{
				g_pDlg->m_List.DeleteItem(i);
			}

			if (g_pDlg->m_List.GetItemData(i) == sockContext->connSocket)
			{
				g_pDlg->m_List.DeleteItem(i);
			}
		
		}


		
	}

	// 连接上发生错误
	void OnConnectionError(SocketContext *sockContext, int error)
	{
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%4d-%02d-%02d %02d:%02d:%02d.%03d：A connection erro： %d，Current connects：%d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, error, GetConnectCnt());
		std::vector<SOCKET> sAry;

		std::lock_guard<std::mutex> lg(g_pDlg->m_ListMtx);//多线程删除，记得同步
		//界面上也去除这一行
		for (int i = 0; i < g_pDlg->m_List.GetItemCount(); i++)
		{
			//获得对应在界面的行数

			//为0的也干掉
			if (g_pDlg->m_List.GetItemData(i) == 0)
			{
				g_pDlg->m_List.DeleteItem(i);
			}

			if (g_pDlg->m_List.GetItemData(i) == sockContext->connSocket)
			{
				g_pDlg->m_List.DeleteItem(i);
			
			}

		}




	}

	// 读操作完成
	void OnRecvCompleted(SocketContext *sockContext, IOContext *ioContext)
	{
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%4d-%02d-%02d %02d:%02d:%02d.%03d：Recv data： %s \n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, ioContext->wsaBuf.buf);
		//数据存放在  ioContext->wsaBuf.buf 中
		//添加对包的解析

	}
	// 写操作完成
	void OnSendCompleted(SocketContext *sockContext, IOContext *ioContext)
	{
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%4d-%02d-%02d %02d:%02d:%02d.%03d：Send data successd！\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		/*
			1.从MFC列表选中要操作的socket
			2.从界面获取其对应的sockContext->connSocket
			3.
		*/

		// 向  sockContext->connSocket  







	}

};












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


// CMFCServerDlg 对话框



CMFCServerDlg::CMFCServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
}

BEGIN_MESSAGE_MAP(CMFCServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CMFCServerDlg::OnNMRClickList1)
	ON_COMMAND(ID_Screen, &CMFCServerDlg::OnScreen)
END_MESSAGE_MAP()


// CMFCServerDlg 消息处理程序

BOOL CMFCServerDlg::OnInitDialog()
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码


	CRect rect;
	m_List.GetClientRect(&rect);

	m_List.SetExtendedStyle(m_List.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 为列表视图控件添加三列   
	m_List.InsertColumn(0, _T("IP:PORT"), LVCFMT_CENTER, rect.Width() / 3, 0);
	m_List.InsertColumn(1, _T("system"), LVCFMT_CENTER, rect.Width() / 3, 1);
	m_List.InsertColumn(2, _T("位置"), LVCFMT_CENTER, rect.Width() / 3, 2);

	IOCPServer *pServer = new IOCPServer;
	
	// 开启服务
	if (pServer->Start())
	{
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%4d-%02d-%02d %02d:%02d:%02d.%03d：Server start succussed on port:%d... \n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, pServer->GetPort());
	}
	else
	{
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		printf("%4d-%02d-%02d %02d:%02d:%02d.%03d：Server start failed！\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
		return 0;
	}

	//对话框的this指针
	g_pDlg = this;


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCServerDlg::OnPaint()
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
HCURSOR CMFCServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCServerDlg::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem != -1)
	{

		CPoint pt;
		GetCursorPos(&pt);
		CMenu menu;
		menu.LoadMenu(IDR_MENU1);
		CMenu *popmenu;
		popmenu = menu.GetSubMenu(0);
		popmenu->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	}


}


void CMFCServerDlg::OnScreen()
{
	// TODO: 在此添加命令处理程序代码

	//获取当前选中行的位置
	int nPos = m_List.GetSelectionMark();
	//获得刚才存储的data，也就是我们的socket
	SOCKET NewConnection = m_List.GetItemData(nPos);

	int n = 1;
	
}
