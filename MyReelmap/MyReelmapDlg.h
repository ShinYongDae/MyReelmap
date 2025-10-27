
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
	int m_nDispPnl[2]; // Left(0), Right(1)

	void InitReelmap();
	void InitOpengl();
	void InitCamMaster();

	void Disp();
	void DrawStrPcs();
	void DrawPnlDefNum(int nSerial);
	void DrawPnlDef(int nSerial);
	void DrawMarkedPcs(int nSerial);

	void SetItsOrgCase(int nCase);

// �����Դϴ�.
public:
	CMyReelmapDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	~CMyReelmapDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYREELMAP_DIALOG };
#endif

public:
	CArPcr& GetAddrArPcr();
	CArPcrMark& GetAddrArPcrMark();
	tagStrPcs& GetAddrStrPcs();
	BOOL GetMatrix(int nPcsId, int &nR, int &nC);
	COLORREF GetDefColor(int nDefCode);

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
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
};
