// SimpleCamMaster.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MyReelmap.h"
#include "SimpleCamMaster.h"


// CSimpleCamMaster

IMPLEMENT_DYNAMIC(CSimpleCamMaster, CWnd)

CSimpleCamMaster::CSimpleCamMaster()
{
	ThreadStart();
}

CSimpleCamMaster::CSimpleCamMaster(CWnd* pParent)
{
	m_pParent = pParent;
	if (pParent)
		m_hParent = pParent->GetSafeHwnd();

	CreateWndForm(WS_CHILD | WS_OVERLAPPED);
	ThreadStart();
}

CSimpleCamMaster::~CSimpleCamMaster()
{
	ThreadStop();
	Sleep(30);
	t1.join();
}


BEGIN_MESSAGE_MAP(CSimpleCamMaster, CWnd)
END_MESSAGE_MAP()



// CSimpleCamMaster 메시지 처리기입니다.
BOOL CSimpleCamMaster::CreateWndForm(DWORD dwStyle)
{
	if (!Create(NULL, _T("SimpleReelmap"), dwStyle, CRect(0, 0, 0, 0), m_pParent, (UINT)this))
	{
		AfxMessageBox(_T("CSimpleReelmap::Create() Failed!!!"));
		return FALSE;
	}

	return TRUE;
}


void CSimpleCamMaster::ProcThrd(const LPVOID lpContext)
{
	CSimpleCamMaster* pSimpleCamMaster = reinterpret_cast<CSimpleCamMaster*>(lpContext);

	while (pSimpleCamMaster->ThreadIsAlive())
	{
		if (!pSimpleCamMaster->ProcCamMaster())
			break;
	}

	pSimpleCamMaster->ThreadEnd();
}

BOOL CSimpleCamMaster::ProcCamMaster()
{
	Sleep(100);
	return TRUE;
}

void CSimpleCamMaster::ThreadStart()
{
	m_bThreadStateEnd = FALSE;
	m_bThreadAlive = TRUE;
	t1 = std::thread(ProcThrd, this);
}

void CSimpleCamMaster::ThreadStop()
{
	m_bThreadAlive = FALSE;
	MSG message;
	const DWORD dwTimeOut = 1000 * 60 * 3; // 3 Minute
	DWORD dwStartTick = GetTickCount();
	Sleep(30);
	while (!m_bThreadStateEnd)
	{
		if (GetTickCount() >= (dwStartTick + dwTimeOut))
		{
			AfxMessageBox(_T("WaitUntilThreadEnd() Time Out!!!", NULL, MB_OK | MB_ICONSTOP));
			return;
		}
		if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
		Sleep(30);
	}
}

void CSimpleCamMaster::ThreadEnd()
{
	m_bThreadStateEnd = TRUE;
}

BOOL CSimpleCamMaster::ThreadIsAlive()
{
	return m_bThreadAlive;
}


BOOL CSimpleCamMaster::LoadStrpcs(CString sPath)
{
	CString str = _T("");
	int nN = 0;
	int nTemp = 0;

	int nCornerNum;
	int nDummy[MAX_PATH];
	//REGIONS_FRAME FrameRgnPix[MAX_FRAME_RGN_NUM];

	CString		strFileNCam;
	CFileFind	find;

	// 1. Find File "strpcs.rgn"
	if (!find.FindFile(sPath))
	{
		AfxMessageBox(_T("StrPcs.bin파일을 찾지못했습니다."));
		return(FALSE);
	}

	// 2. Read Strip & Piece Info
	int		i, j, k, Dummy;
	int		PieceNum, PieceCount;
	CFile	file;
	TCHAR	FileNLoc[1024];
	_stprintf(FileNLoc, _T("%s"), sPath);

	if (!file.Open(FileNLoc, CFile::modeRead))
	{
		if (!file.Open(FileNLoc, CFile::modeRead))
		{
			AfxMessageBox(_T("캠마스터에 피스정보파일을 Open하지 못했습니다."));
			return(FALSE);
		}
	}

	int nFrameSize = 0;

	file.Read((void *)&nFrameSize, sizeof(int));
	m_stStrPcs.m_nFrameRgnNum = nFrameSize;

	file.Read((void *)&m_stStrPcs.m_nPieceRgnNum, sizeof(int));
	file.Read((void *)&nCornerNum, sizeof(int));
	file.Read((void *)&nDummy[0], sizeof(int));	// reserved
	file.Read((void *)&nDummy[1], sizeof(int));	// reserved
	file.Read((void *)&nDummy[2], sizeof(int));	// reserved
	file.Read((void *)&nDummy[3], sizeof(int));	// reserved
	file.Read((void *)&nDummy[4], sizeof(int));	// reserved

	// Init. Buffers
	for (i = 0; i < m_stStrPcs.m_nFrameRgnNum; i++)
	{
		m_stStrPcs.m_stFrameRgnPix[i].iStartX = 0;
		m_stStrPcs.m_stFrameRgnPix[i].iStartY = 0;
		m_stStrPcs.m_stFrameRgnPix[i].iEndX = 0;
		m_stStrPcs.m_stFrameRgnPix[i].iEndY = 0;
		m_stStrPcs.m_stFrameRgnPix[i].FRotate = 0;
		m_stStrPcs.m_stFrameRgnPix[i].FMirror = 0;
		m_stStrPcs.m_stFrameRgnID[i].Col = -1;
		m_stStrPcs.m_stFrameRgnID[i].Row = -1;
	}

	for (i = 0; i < MAX_PIECE_RGN_NUM; i++)
	{
		m_stStrPcs.m_stPieceRgnPix[i].iStartX = 0;
		m_stStrPcs.m_stPieceRgnPix[i].iStartY = 0;
		m_stStrPcs.m_stPieceRgnPix[i].iEndX = 0;
		m_stStrPcs.m_stPieceRgnPix[i].iEndY = 0;
		m_stStrPcs.m_stPieceRgnPix[i].FRotate = 0;
		m_stStrPcs.m_stPieceRgnPix[i].FMirror = 0;

		m_stStrPcs.m_stPieceRgnPix[i].Col = -1;
		m_stStrPcs.m_stPieceRgnPix[i].Row = -1;
	}

	if (m_stStrPcs.m_nPieceRgnNum > MAX_PIECE_RGN_NUM)
	{
		file.Close();
		AfxMessageBox(_T("캠마스터에서 설정한 피스 갯수가 최대치를 초과했습니다."));
		return FALSE;
	}

	//start for previous pcs info.
	CRect rtFrm(10000, 10000, 0, 0);
	double fData1, fData2, fData3, fData4;
	long lTop = 0;
	int nCol = 0, nRow = 0;
	int Size = 0;
	int nPieceRgnNum = 0;

	int nMaxR = 0, nMaxC = 0;
	//double mmPxl = MasterInfo.dPixelSize / 1000.0; // [mm]
	//end for previous pcs info.
	
	CPoint	**PolygonPoints = NULL;
	AllocPolygonRgnData(PolygonPoints, nCornerNum, m_stStrPcs.m_nPieceRgnNum);

	PieceCount = 0;
	for (j = 0; j < m_stStrPcs.m_nFrameRgnNum; j++)
	{
		// 4. Set Strip Info.
		file.Read((void *)&m_stStrPcs.m_stFrameRgnID[j].nId, sizeof(int));						// Strip ID
		file.Read((void *)&m_stStrPcs.m_stFrameRgnID[j].Row, sizeof(int));						// Row
		file.Read((void *)&m_stStrPcs.m_stFrameRgnID[j].Col, sizeof(int));						// Col
		file.Read((void *)&nDummy[5], sizeof(int));								// Rotation Info (0 : 0  1 : 90  2 : 180  3 : 270 [Degree])
		file.Read((void *)&(m_stStrPcs.m_stFrameRgnPix[j]), sizeof(REGIONS_FRAME));			// (int * 6)
		
		// 5. Set Piece Info.
		file.Read((void *)&m_stStrPcs.m_nPieceNum[j], sizeof(int));

		/*
		int iStartX, iStartY;
		int iEndX, iEndY;
		int FMirror;	//0 : 원본 1 : 상하미러  2 : 좌우미러
		int FRotate;	//0 : 0도  1 : 90도  2 : 180도  3 : 270도
		*/

		for (i = 0; i < m_stStrPcs.m_nPieceNum[j]; i++)
		{
			file.Read((void *)&m_stStrPcs.m_stPieceRgnPix[i + PieceCount].nId, sizeof(int));	// Piece ID
			file.Read((void *)&m_stStrPcs.m_stPieceRgnPix[i + PieceCount].Row, sizeof(int));	// Row
			file.Read((void *)&m_stStrPcs.m_stPieceRgnPix[i + PieceCount].Col, sizeof(int));	// Col
			file.Read((void *)&nTemp, sizeof(int));

			m_stStrPcs.m_stPieceRgnPix[i + PieceCount].nId += PieceCount;					// Piece ID : Panel 별로 (0 ~ 시작)

			if (nMaxC < m_stStrPcs.m_stPieceRgnPix[i + PieceCount].Col)
				nMaxC = m_stStrPcs.m_stPieceRgnPix[i + PieceCount].Col;

			if (nMaxR < m_stStrPcs.m_stPieceRgnPix[i + PieceCount].Row)
				nMaxR = m_stStrPcs.m_stPieceRgnPix[i + PieceCount].Row;
			////////////////////////////////////////////////////////////////////////////////////////////////
			// Set Piece position
			Dummy = sizeof(CPoint);
			file.Read((void *)PolygonPoints[i + PieceCount], sizeof(CPoint)*nCornerNum);
			file.Read((void *)&(m_stStrPcs.m_stPieceRgnPix[i + PieceCount].FRotate), sizeof(int));

			// Set PieceRgnPix
			m_stStrPcs.m_stPieceRgnPix[i + PieceCount].iStartX = PolygonPoints[i + PieceCount][0].x;
			m_stStrPcs.m_stPieceRgnPix[i + PieceCount].iStartY = PolygonPoints[i + PieceCount][0].y;
			m_stStrPcs.m_stPieceRgnPix[i + PieceCount].iEndX = PolygonPoints[i + PieceCount][0].x;
			m_stStrPcs.m_stPieceRgnPix[i + PieceCount].iEndY = PolygonPoints[i + PieceCount][0].y;
			for (k = 0; k < nCornerNum; k++)
			{
				m_stStrPcs.m_stPieceRgnPix[i + PieceCount].iStartX = min(m_stStrPcs.m_stPieceRgnPix[i + PieceCount].iStartX, PolygonPoints[i + PieceCount][k].x);
				m_stStrPcs.m_stPieceRgnPix[i + PieceCount].iStartY = min(m_stStrPcs.m_stPieceRgnPix[i + PieceCount].iStartY, PolygonPoints[i + PieceCount][k].y);
				m_stStrPcs.m_stPieceRgnPix[i + PieceCount].iEndX = max(m_stStrPcs.m_stPieceRgnPix[i + PieceCount].iEndX, PolygonPoints[i + PieceCount][k].x);
				m_stStrPcs.m_stPieceRgnPix[i + PieceCount].iEndY = max(m_stStrPcs.m_stPieceRgnPix[i + PieceCount].iEndY, PolygonPoints[i + PieceCount][k].y);
			}
		}
	}// for(j = 0; j < m_stStrPcs.m_nFrameRgnNum; j++)

	//start for previous pcs info.
	nPieceRgnNum = m_stStrPcs.m_nPieceRgnNum;
	if (nPieceRgnNum <= 0)
	{
		file.Close();
		Size = 0;
		FreePolygonRgnData(PolygonPoints);

		AfxMessageBox(_T("캠마스터에서 설정한 피스 갯수가 없습니다."));
		return(FALSE);
	}

	for (i = 0; i < nPieceRgnNum; i++)
	{
		fData1 = (double)m_stStrPcs.m_stPieceRgnPix[i].iStartX;
		fData2 = (double)m_stStrPcs.m_stPieceRgnPix[i].iStartY;
		fData3 = (double)m_stStrPcs.m_stPieceRgnPix[i].iEndX;
		fData4 = (double)m_stStrPcs.m_stPieceRgnPix[i].iEndY;
	}

	double dWidth = rtFrm.right - rtFrm.left;
	double dHeight = rtFrm.bottom - rtFrm.top;
	double dMarginW = dWidth * 0.05;
	double dMarginH = dHeight * 0.05;
	double dMargin = (dMarginW > dMarginH) ? dMarginH : dMarginW;

	file.Close();

	FreePolygonRgnData(PolygonPoints);

	m_stStrPcs.m_nMaxRow = nMaxR * m_stStrPcs.m_nFrameRgnNum;
	m_stStrPcs.m_nMaxCol = nMaxC;

	return TRUE;
}

void CSimpleCamMaster::AllocPolygonRgnData(CPoint**& PolygonPoints, int nCornerNum, int PieceRgnNum)
{
	int i;
	CPoint	**PolygonPointsLocal = NULL;

	FreePolygonRgnData(PolygonPointsLocal);

	PolygonPointsLocal = (CPoint **)malloc(sizeof(CPoint*) * MAX_PIECE_RGN_NUM);
	int nRefinePieceNum = max(1, PieceRgnNum);
	for (i = 0; i < nRefinePieceNum; i++)
		PolygonPointsLocal[i] = (CPoint *)malloc(sizeof(CPoint) * nCornerNum);

	for (i = nRefinePieceNum; i < MAX_PIECE_RGN_NUM; i++)
	{
		PolygonPointsLocal[i] = NULL;
	}

	PolygonPoints = PolygonPointsLocal;
}

void CSimpleCamMaster::FreePolygonRgnData(CPoint** PolygonPoints)
{
	int i;
	if (PolygonPoints != NULL)
	{
		for (i = 0; i < MAX_PIECE_RGN_NUM; i++)
		{
			if (PolygonPoints[i] != NULL)
			{
				free(PolygonPoints[i]);
				PolygonPoints[i] = NULL;
			}
		}

		free(PolygonPoints);
		PolygonPoints = NULL;
	}
}

tagStrPcs& CSimpleCamMaster::GetAddrStrPcs()
{
	return m_stStrPcs;
}