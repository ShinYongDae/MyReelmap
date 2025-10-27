
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
	m_pOpengl = NULL;

	m_nDispPnl[0] = 0;
	m_nDispPnl[1] = 0;
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
	if (m_pOpengl)
	{
		delete m_pOpengl;
		m_pOpengl = NULL;
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
	ON_BN_CLICKED(IDC_BUTTON3, &CMyReelmapDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CMyReelmapDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CMyReelmapDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_RADIO1, &CMyReelmapDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CMyReelmapDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CMyReelmapDlg::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, &CMyReelmapDlg::OnBnClickedRadio4)
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
	InitReelmap();
	InitOpengl();
	InitCamMaster();
	//DrawStrPcs();
	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
	SetItsOrgCase(0);

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

void CMyReelmapDlg::InitReelmap()
{
	CString sPathInfo = _T("C:\\R2RSet\\Info0.bin");
	CString sPathRmap = _T("C:\\R2RSet\\Reelmap0.bin");
	CString sPathYield = _T("C:\\R2RSet\\Yield0.bin");
	CString sPathMark = _T("C:\\R2RSet\\Mark0.bin");
	m_pReelmap[0] = new CSimpleReelmap(RMapUp, sPathInfo, sPathRmap, sPathYield, sPathMark, this);

	stRmapInfo stInfo;
	//stInfo.m_sMcName = _T("BAO14");
	//stInfo.m_sUserName = _T("������");
	//stInfo.m_sModel = _T("L231115S31");
	//stInfo.m_sLot = _T("200125279");
	//stInfo.m_sLayer = _T("TOP-3-050");
	//stInfo.m_sLayerUp = _T("TOP-3-050");
	//stInfo.m_sLayerDn = _T("BOTTOM-4-050");
	//stInfo.m_sItsCode = _T("200125279");
	//stInfo.m_sProcessCode = _T("VS90");

	stInfo.m_sMcName = _T("BAO14");
	stInfo.m_sUserName = _T("����");
	stInfo.m_sModel = _T("B1313F2ST1");
	stInfo.m_sLot = _T("200180747");
	stInfo.m_sLayer = _T("TOP-0-050");
	stInfo.m_sLayerUp = _T("TOP-0-050");
	stInfo.m_sLayerDn = _T("BOTTOM-0-050");
	stInfo.m_sItsCode = _T("200180747");
	stInfo.m_sProcessCode = _T("AO9L");

	stInfo.m_nActionCode = 3;
	stInfo.m_nMaxRow = 24;
	stInfo.m_nMaxCol = 22;
	//stInfo.m_nMaxRow = 20;
	//stInfo.m_nMaxCol = 30;
	stInfo.m_nMaxStrip = 4;
	stInfo.m_nMaxDefCode = 30;
	m_pReelmap[0]->Init(stInfo);
}

void CMyReelmapDlg::InitOpengl()
{
	m_pOpengl = new CSimpleOpengl(this);
	HWND hWnd = GetDlgItem(IDC_STC_REELMAP_IMG)->GetSafeHwnd();
	m_pOpengl->SubclassDlgItem(IDC_STC_REELMAP_IMG, this);
	m_pOpengl->SetHwnd(hWnd, this);
	m_pOpengl->SetFont(_T("����ü"), 12, TRUE);

	//stVertex v1, v2;
	//v1.x = 10; v1.y = 10; v1.z = 0.0;
	//v2.x = 500; v2.y = 500; v2.z = 0.0;
	//m_Opengl.AddLine(v1, v2);
}

void CMyReelmapDlg::InitCamMaster()
{
	m_CamMaster.LoadStrpcs(_T("C:\\R2RSet\\Temp\\strpcs.bin"));
}

void CMyReelmapDlg::Disp()
{
	CString sCurr = _T("");
	CString sDisp = m_pReelmap[0]->GetTextArPcr();
	GetDlgItem(IDC_EDIT1)->GetWindowText(sCurr);
	if(sCurr != sDisp)
		GetDlgItem(IDC_EDIT1)->SetWindowText(sDisp);

	sDisp = m_pReelmap[0]->GetTextArPcrYield(); 
	GetDlgItem(IDC_EDIT4)->GetWindowText(sCurr);
	if(sCurr != sDisp)
		GetDlgItem(IDC_EDIT4)->SetWindowText(sDisp);
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
	CString sDisp = m_pReelmap[0]->GetTextArPcr();
	GetDlgItem(IDC_EDIT2)->GetWindowText(sCurr);
	if (sCurr != sDisp)
		GetDlgItem(IDC_EDIT2)->SetWindowText(sDisp);

	//sDisp = m_pReelmap[0]->GetTextArPcrYield();
	//GetDlgItem(IDC_EDIT4)->GetWindowText(sCurr);
	//if(sCurr != sDisp)
	//	GetDlgItem(IDC_EDIT4)->SetWindowText(sDisp);
}


void CMyReelmapDlg::OnBnClickedButton3()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString sVal;
	GetDlgItem(IDC_EDIT3)->GetWindowText(sVal);
	int nSerial = _ttoi(sVal);
	if (nSerial < 1) return;
	DrawPnlDef(nSerial);
	DrawPnlDefNum(nSerial);
	DrawMarkedPcs(nSerial);
	int nPrevDispPnl = m_nDispPnl[1];
	m_nDispPnl[1] = nSerial;			// Right Marking
	if (nSerial > 0 && m_nDispPnl[1] != nPrevDispPnl)
		m_nDispPnl[0] = nPrevDispPnl;	// Left Marking
	m_nDispPnl[0] = m_nDispPnl[1] - 1;
	m_pReelmap[0]->SetDispPnl(nSerial);
	m_pOpengl->SetDraw();
}

COLORREF CMyReelmapDlg::GetDefColor(int nDefCode)
{
	return m_pReelmap[0]->GetDefColor(nDefCode);
}

tagStrPcs& CMyReelmapDlg::GetAddrStrPcs()
{
	return m_CamMaster.GetAddrStrPcs();
}

CArPcr& CMyReelmapDlg::GetAddrArPcr()
{
	return m_pReelmap[0]->GetAddrArPcr();
}

CArPcrMark& CMyReelmapDlg::GetAddrArPcrMark()
{
	return m_pReelmap[0]->GetAddrArPcrMark();
}

BOOL CMyReelmapDlg::GetMatrix(int nPcsId, int &nR, int &nC)
{
	return m_pReelmap[0]->GetMatrix(nPcsId, nR, nC);
}

void CMyReelmapDlg::DrawStrPcs()
{
	m_pOpengl->DrawStrPcs(GetAddrStrPcs());
}

void CMyReelmapDlg::DrawPnlDefNum(int nSerial)
{
	m_pOpengl->DrawPnlDefNum(nSerial, GetAddrArPcr(), GetAddrStrPcs());
}

void CMyReelmapDlg::DrawPnlDef(int nSerial)
{
	m_pOpengl->DrawPnlDef(nSerial, GetAddrArPcr(), GetAddrStrPcs());
}

void CMyReelmapDlg::DrawMarkedPcs(int nSerial)
{
	m_pOpengl->DrawMarkedPcs(nSerial, GetAddrArPcrMark(), GetAddrStrPcs());
}


void CMyReelmapDlg::OnBnClickedButton4()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pReelmap[0]->SetPcsMkOut(0); // 0: Left Cam Or 1: Right Cam , �ҷ� �ǽ� �ε��� [ 0 ~ (Total Pcs - 1) ]  // (�ǽ��ε����� CamMaster���� ���� ���� �������� ��.)
	m_pOpengl->DrawMarkedPcs(0, m_nDispPnl[0], GetAddrArPcrMark(), GetAddrStrPcs());
	m_pOpengl->SetDraw();
}


void CMyReelmapDlg::OnBnClickedButton5()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pReelmap[0]->SetPcsMkOut(1); // 0: Left Cam Or 1: Right Cam , �ҷ� �ǽ� �ε��� [ 0 ~ (Total Pcs - 1) ]  // (�ǽ��ε����� CamMaster���� ���� ���� �������� ��.)
	m_pOpengl->DrawMarkedPcs(1, m_nDispPnl[1], GetAddrArPcrMark(), GetAddrStrPcs());
	m_pOpengl->SetDraw();
}

void CMyReelmapDlg::SetItsOrgCase(int nCase)
{
	m_pOpengl->SetItsOrgCase(nCase);
	DrawStrPcs();
	m_pOpengl->SetDraw();
}

void CMyReelmapDlg::OnBnClickedRadio1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SetItsOrgCase(0);
}


void CMyReelmapDlg::OnBnClickedRadio2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SetItsOrgCase(1);
}


void CMyReelmapDlg::OnBnClickedRadio3()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SetItsOrgCase(2);
}


void CMyReelmapDlg::OnBnClickedRadio4()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SetItsOrgCase(3);
}
