
// MyReelmapDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "MyReelmap.h"
#include "MyReelmapDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyReelmapDlg 대화 상자



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


// CMyReelmapDlg 메시지 처리기

BOOL CMyReelmapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	InitReelmap();
	InitOpengl();
	InitCamMaster();
	//DrawStrPcs();
	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
	SetItsOrgCase(0);

	m_bTimer = TRUE;
	SetTimer(0, 100, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMyReelmapDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMyReelmapDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMyReelmapDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
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
	//stInfo.m_sUserName = _T("서봉율");
	//stInfo.m_sModel = _T("L231115S31");
	//stInfo.m_sLot = _T("200125279");
	//stInfo.m_sLayer = _T("TOP-3-050");
	//stInfo.m_sLayerUp = _T("TOP-3-050");
	//stInfo.m_sLayerDn = _T("BOTTOM-4-050");
	//stInfo.m_sItsCode = _T("200125279");
	//stInfo.m_sProcessCode = _T("VS90");

	stInfo.m_sMcName = _T("BAO14");
	stInfo.m_sUserName = _T("고도욱");
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
	m_pOpengl->SetFont(_T("굴림체"), 12, TRUE);

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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pReelmap[0]->Save();
}


void CMyReelmapDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pReelmap[0]->SetPcsMkOut(0); // 0: Left Cam Or 1: Right Cam , 불량 피스 인덱스 [ 0 ~ (Total Pcs - 1) ]  // (피스인덱스는 CamMaster에서 정한 것을 기준으로 함.)
	m_pOpengl->DrawMarkedPcs(0, m_nDispPnl[0], GetAddrArPcrMark(), GetAddrStrPcs());
	m_pOpengl->SetDraw();
}


void CMyReelmapDlg::OnBnClickedButton5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pReelmap[0]->SetPcsMkOut(1); // 0: Left Cam Or 1: Right Cam , 불량 피스 인덱스 [ 0 ~ (Total Pcs - 1) ]  // (피스인덱스는 CamMaster에서 정한 것을 기준으로 함.)
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetItsOrgCase(0);
}


void CMyReelmapDlg::OnBnClickedRadio2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetItsOrgCase(1);
}


void CMyReelmapDlg::OnBnClickedRadio3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetItsOrgCase(2);
}


void CMyReelmapDlg::OnBnClickedRadio4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetItsOrgCase(3);
}
