
// AutoEye.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"		// 主符号


// CAutoEyeApp:
// 有关此类的实现，请参阅 AutoEye.cpp
//

class CAutoEyeApp : public CWinApp
{
public:
	CAutoEyeApp();

// 重写
public:
	virtual BOOL InitInstance();

	DWORD AddFilesByPath(LPCTSTR lpszFileDir, BOOL bIncludeSub, LPCTSTR lpszFileExt);
	BOOL AddFilesByFileName(LPCTSTR lpszFileName);

// 实现
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CAutoEyeApp theApp;
