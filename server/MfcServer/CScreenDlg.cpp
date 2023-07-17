// CScreenDlg.cpp: 实现文件
//

#include "pch.h"
#include "MfcServer.h"
#include "CScreenDlg.h"
#include "afxdialogex.h"

#include "../../client/ReClient_KBH_CMD/MyPacket.h"

// CScreenDlg 对话框

IMPLEMENT_DYNAMIC(CScreenDlg, CDialogEx)

CScreenDlg::CScreenDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SCREENDLG, pParent)
{

}

CScreenDlg::~CScreenDlg()
{


}

void CScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CScreenDlg, CDialogEx)
END_MESSAGE_MAP()


// CScreenDlg 消息处理程序


bool CScreenDlg::ShowScreen(const char* pBuf, int nLength)
{
	//获得当前的屏幕数据, 并显示到当前对话框里面来
	
	tagScreenData* pScreenData = (tagScreenData*)pBuf;


	CDC memDC;//创建一个内存DC
	CBitmap bitMap;//创建一个兼容位图
	CDC* pDC = GetDC();
	memDC.CreateCompatibleDC(pDC);
	bitMap.CreateCompatibleBitmap(pDC, pScreenData->nWidth, pScreenData->nHeight);
	memDC.SelectObject(bitMap);

	//将获取到的数据直接写入到内存dc的bitmap中
	bitMap.SetBitmapBits(nLength - 8, pScreenData->data);

	//接下来将内存dc的数据拷贝给屏幕dc
	pDC->BitBlt(0, 0, pScreenData->nWidth, pScreenData->nHeight, &memDC, 0, 0, SRCCOPY);

	UpdateWindow();

	memDC.DeleteDC();
	bitMap.DeleteObject();
	ReleaseDC(pDC);
	
	return true;
}

