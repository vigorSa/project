
// Vison.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//
#if !defined(AFX_HVViewer_H__572C5FA4_4480_45D2_8AF5_6B4EB4BE114F__INCLUDED_)
#define AFX_HVViewer_H__572C5FA4_4480_45D2_8AF5_6B4EB4BE114F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "halconcpp.h"

#define WIDTH_BYTES(bits)	(((bits) + 31) / 32 * 4)

// CVisonApp:
// �йش����ʵ�֣������ Vison.cpp
//

class CVisonApp : public CWinApp
{
public:
	CVisonApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CVisonApp theApp;

#endif