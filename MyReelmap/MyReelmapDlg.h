
// MyReelmapDlg.h : ��� ����
//

#pragma once
#include "SimpleReelmap.h"


// CMyReelmapDlg ��ȭ ����
class CMyReelmapDlg : public CDialog
{
	CSimpleReelmap* m_pReelmap[4];

	BOOL m_bTimer;
	void Disp();

// �����Դϴ�.
public:
	CMyReelmapDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	~CMyReelmapDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYREELMAP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
