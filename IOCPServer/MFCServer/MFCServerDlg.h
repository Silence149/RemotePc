﻿
// MFCServerDlg.h: 头文件
//

#pragma once
#include <vector>
#include <mutex>


// CMFCServerDlg 对话框
class CMFCServerDlg : public CDialogEx
{
// 构造
public:
	CMFCServerDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


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
	CListCtrl m_List;

	//IOCPServer m_Iocp;//用于IOCP的处理  相当于IOCPBase类

	std::mutex m_ListMtx;//用于List的同步问题


	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnScreen();
};
