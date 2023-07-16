#pragma once


// CScreenDlg 对话框

class CScreenDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScreenDlg)

public:
	CScreenDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CScreenDlg();

	bool ShowScreen(const char* pBuf, int nLength);


// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SCREENDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
