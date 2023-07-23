
// MfcServerDlg.h: 头文件
//

#pragma once

#include <map>
#include <mutex>
#include <stdio.h>
#include "CIocpServer.h"







// CMfcServerDlg 对话框
class CMfcServerDlg : public CDialogEx
{
// 构造
public:
	CMfcServerDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~CMfcServerDlg(void);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	//构建一个初始化网络的操作
	bool InitAcceptSocket();

	//定义一个静态成员函数来表示我们的线程回调函数
	static DWORD  AcceptFuncThread(LPVOID lpParam);


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_Lst;

	HANDLE m_hAcceptThd;			//表示接受我们连接的线程

	SOCKET m_hAcceptSocket;			//用于监听连接的socket

	//定义一个hash表来存储连接的所有人  key--value
	std::map<SOCKET, MySession*> m_map;
	//需要一个互斥体
	std::mutex m_AcceptMtx;//用于记录map的同步问题

	std::mutex m_ScreenMtx;//用于屏幕显示的同步问题

	CIocpServer m_Iocp;//用于IOCP的处理  相当于IOCPBase类



	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnScreen();
};
