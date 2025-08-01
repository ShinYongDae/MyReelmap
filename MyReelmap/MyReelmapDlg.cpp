
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
	DrawStrPcs();

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
	CString sDisp = m_pReelmap[0]->GetTextArPcr(0);
	GetDlgItem(IDC_EDIT1)->GetWindowText(sCurr);
	if(sCurr != sDisp)
		GetDlgItem(IDC_EDIT1)->SetWindowText(sDisp);
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
	CString sDisp = m_pReelmap[0]->GetTextArPcr(1);
	GetDlgItem(IDC_EDIT2)->GetWindowText(sCurr);
	if (sCurr != sDisp)
		GetDlgItem(IDC_EDIT2)->SetWindowText(sDisp);
}


void CMyReelmapDlg::OnBnClickedButton3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

tagStrPcs& CMyReelmapDlg::GetAddrStrPcs()
{
	return m_CamMaster.GetAddrStrPcs();
}

BOOL CMyReelmapDlg::GetMatrix(int nPcsId, int &nR, int &nC)
{
	return m_pReelmap[0]->GetMatrix(nPcsId, nR, nC);
}
void CMyReelmapDlg::DrawStrPcs()
{
	//m_pReelmap[0]->DrawStrPcs();
	m_pOpengl->DrawStrPcs(GetAddrStrPcs());

	//stVertex v1, v2;
	//v1.x = 500; v1.y = 500; v1.z = 0.0;
	//v2.x = 1100; v2.y = 500; v2.z = 0.0;
	//m_Opengl.AddLine(v1, v2);
	//m_Opengl.Refresh();
/*
	int k, nTotPnl = 6, nSelMarkingPnl = 2;

	for (k = nTotPnl - 1; k >= 0; k--)
	{
		if (k == nSelMarkingPnl)
		{
			; // Panel edge is Red Line 
		}
		else if (k == nSelMarkingPnl + 1)
		{
			; // Panel edge is Red Line 
		}
		else
		{
			; // Panel edge is White Line 
		}

		//int nIdx = pDoc->GetPcrIdx1(m_pPnlNum[k]);
		if (m_pReelmap[0]->Get == -1 || pDoc->m_pPcr[nTestMode][nIdx]->m_nErrPnl == -2)
		{
			// Draw Cross....
			GVertex vtPnt[4];
			vtPnt[0] = m_pFrmRgn[k][0];

			vtPnt[1].x = m_pFrmRgn[k][0].x;
			vtPnt[1].y = m_pFrmRgn[k][1].y;
			vtPnt[1].z = m_pFrmRgn[k][1].z;

			vtPnt[2] = m_pFrmRgn[k][1];

			vtPnt[3].x = m_pFrmRgn[k][1].x;
			vtPnt[3].y = m_pFrmRgn[k][0].y;
			vtPnt[3].z = m_pFrmRgn[k][0].z;

			GVGLDrawInit(GV_LINE, 3, m_rgbRed);
			GVGLDrawVertex(vtPnt[0]);
			GVGLDrawVertex(vtPnt[2]);
			GVGLDrawShow();

			GVGLDrawInit(GV_LINE, 3, m_rgbRed);
			GVGLDrawVertex(vtPnt[1]);
			GVGLDrawVertex(vtPnt[3]);
			GVGLDrawShow();
		}

		for (i = 0; i < m_nTotPcs; i++)
		{
			if (m_pReelMap)//pDoc->
			{
				if (m_pPnlNum[k] <= 0)
				{
					GVGLDrawInit(GV_RECTF, 3, m_rgbWhite);
				}
				else
				{
					nDef = m_pReelMap->pPcsDef[k][i];
					if (nDef)
						int iii = i;

					GVGLDrawInit(GV_RECTF, 3, m_rgbDef[nDef]);
				}
			}
			GVGLDrawRectF(m_pPcsPnt[k][i][0], m_pPcsPnt[k][i][1]); // CamMaster의 PCS 인덱스와 그 인덱스의 영역 정보
			GVGLDrawShow();
		}
	}
	*/
}