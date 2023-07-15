
// MfcServer.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// 主符号


// CMfcServerApp:
// 有关此类的实现，请参阅 MfcServer.cpp
//

class CMfcServerApp : public CWinApp
{
public:
	CMfcServerApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CMfcServerApp theApp;
