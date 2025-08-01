
// MyReelmapDlg.h : ��� ����
//

#pragma once
#include "SimpleCamMaster.h"
#include "SimpleOpengl.h"
#include "SimpleReelmap.h"

// CMyReelmapDlg ��ȭ ����
class CMyReelmapDlg : public CDialog
{
	CSimpleReelmap* m_pReelmap[4];
	CSimpleOpengl* m_pOpengl;
	CSimpleCamMaster m_CamMaster;

	BOOL m_bTimer;

	void InitReelmap();
	void InitOpengl();
	void InitCamMaster();

	void Disp();
	void DrawStrPcs();

// �����Դϴ�.
public:
	CMyReelmapDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	~CMyReelmapDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYREELMAP_DIALOG };
#endif

public:
	tagStrPcs& GetAddrStrPcs();
	BOOL GetMatrix(int nPcsId, int &nR, int &nC);

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
	afx_msg void OnBnClickedButton3();
};
