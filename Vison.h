
// Vison.h : PROJECT_NAME 应用程序的主头文件
//
#if !defined(AFX_HVViewer_H__572C5FA4_4480_45D2_8AF5_6B4EB4BE114F__INCLUDED_)
#define AFX_HVViewer_H__572C5FA4_4480_45D2_8AF5_6B4EB4BE114F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "halconcpp.h"

#define WIDTH_BYTES(bits)	(((bits) + 31) / 32 * 4)

// CVisonApp:
// 有关此类的实现，请参阅 Vison.cpp
//

class CVisonApp : public CWinApp
{
public:
	CVisonApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CVisonApp theApp;

#endif