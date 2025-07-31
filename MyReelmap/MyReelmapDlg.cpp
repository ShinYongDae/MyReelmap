
// MyReelmapDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "MyReelmap.h"
#include "MyReelmapDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyReelmapDlg ��ȭ ����



CMyReelmapDlg::CMyReelmapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MYREELMAP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pReelmap[0] = NULL;
	m_pReelmap[1] = NULL;
	m_pReelmap[2] = NULL;
	m_pReelmap[3] = NULL;
}

CMyReelmapDlg::~CMyReelmapDlg()
{
	m_bTimer = FALSE;

	if (m_pReelmap[0])
	{
		delete m_pReelmap[0];
		m_pReelmap[0] = NULL;
	}
	if (m_pReelmap[1])
	{
		delete m_pReelmap[1];
		m_pReelmap[1] = NULL;
	}
	if (m_pReelmap[2])
	{
		delete m_pReelmap[2];
		m_pReelmap[2] = NULL;
	}
	if (m_pReelmap[3])
	{
		delete m_pReelmap[3];
		m_pReelmap[3] = NULL;
	}
}

void CMyReelmapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMyReelmapDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CMyReelmapDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMyReelmapDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CMyReelmapDlg �޽��� ó����

BOOL CMyReelmapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CString sPath0 = _T("C:\\R2RSet\\Reelmap0.bin");
	//CString sPath1 = _T("C:\\R2RSet\\Reelmap1.bin");
	//CString sPath2 = _T("C:\\R2RSet\\Reelmap2.bin");
	//CString sPath3 = _T("C:\\R2RSet\\Reelmap3.bin");
	m_pReelmap[0] = new CSimpleReelmap(sPath0, this);
	//m_pReelmap[1] = new CSimpleReelmap(sPath1, this);
	//m_pReelmap[2] = new CSimpleReelmap(sPath2, this);
	//m_pReelmap[3] = new CSimpleReelmap(sPath3, this);
	m_pReelmap[0]->Init(20, 30, 3);
	//m_pReelmap[1]->Init(20, 30, 3);
	//m_pReelmap[2]->Init(20, 30);
	//m_pReelmap[3]->Init(20, 30);
	m_bTimer = TRUE;
	SetTimer(0, 100, NULL);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CMyReelmapDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CMyReelmapDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMyReelmapDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (nIDEvent == 0)
	{
		KillTimer(0);
		Disp();
		if (m_bTimer)
			SetTimer(0, 100, NULL);
	}

	CDialog::OnTimer(nIDEvent);
}

void CMyReelmapDlg::Disp()
{
	CString sCurr = _T("");
	CString sDisp = m_pReelmap[0]->GetTextArPcr(0);
	GetDlgItem(IDC_EDIT1)->GetWindowText(sCurr);
	if(sCurr != sDisp)
		GetDlgItem(IDC_EDIT1)->SetWindowText(sDisp);
}

void CMyReelmapDlg::OnBnClickedButton1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pReelmap[0]->Save();
}


void CMyReelmapDlg::OnBnClickedButton2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString sCurr = _T("");
	m_pReelmap[0]->Load();
	CString sDisp = m_pReelmap[0]->GetTextArPcr(1);
	GetDlgItem(IDC_EDIT2)->GetWindowText(sCurr);
	if (sCurr != sDisp)
		GetDlgItem(IDC_EDIT2)->SetWindowText(sDisp);
}
