
// MyReelmap.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CMyReelmapApp:
// �� Ŭ������ ������ ���ؼ��� MyReelmap.cpp�� �����Ͻʽÿ�.
//

class CMyReelmapApp : public CWinApp
{
public:
	CMyReelmapApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CMyReelmapApp theApp;