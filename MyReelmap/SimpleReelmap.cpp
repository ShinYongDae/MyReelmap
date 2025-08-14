// SimpleReelmap.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MyReelmap.h"
#include "SimpleReelmap.h"

#include "MyReelmapDlg.h"

// CSimpleReelmap

IMPLEMENT_DYNAMIC(CSimpleReelmap, CWnd)

CSimpleReelmap::CSimpleReelmap(int nLayer, CString sPathInfo, CString sPathRmap, CString sPathYield, CString sPathMark, CWnd* pParent/*=NULL*/)
{
	m_pParent = pParent;
	if (pParent)
		m_hParent = pParent->GetSafeHwnd();
	m_sPathInfo = sPathInfo;
	m_sPathRmap = sPathRmap;
	m_sPathYield = sPathYield;
	m_sPathMark = sPathMark;

	m_bLock = FALSE;

	m_nDispPnl[0] = 0;
	m_nDispPnl[1] = 0;
	m_nMkedPcs[0] = 0;
	m_nMkedPcs[1] = 0;

	m_nLayer = nLayer;

	InitColor();
	InitDef();

	LoadDefectTableIni();
	LoadSapp3CodeIni();
	Free();	
	CreateWndForm(WS_CHILD | WS_OVERLAPPED);
	ThreadStart();
}

CSimpleReelmap::~CSimpleReelmap()
{
	ThreadStop();
	Sleep(30);
	t1.join();

	Free();
}

void CSimpleReelmap::Free()
{
	int i, nCount;
	nCount = m_arPcr.GetSize();
	if (nCount > 0)
	{
		CPcr Pcr;
		for (i = 0; i < nCount; i++)
		{
			Pcr = m_arPcr.GetAt(i);
			Pcr.Free();
		}
		m_arPcr.RemoveAll();
	}

	nCount = m_arPcrYield.GetSize();
	if (nCount > 0)
		m_arPcrYield.RemoveAll();

	nCount = m_arPcrMark.GetSize();
	if (nCount > 0)
	{
		CPcrMark PcrMark;
		for (i = 0; i < nCount; i++)
		{
			PcrMark = m_arPcrMark.GetAt(i);
			PcrMark.Free();
		}
		m_arPcrMark.RemoveAll();
	}
}

BEGIN_MESSAGE_MAP(CSimpleReelmap, CWnd)
END_MESSAGE_MAP()



// CSimpleReelmap 메시지 처리기입니다.
BOOL CSimpleReelmap::CreateWndForm(DWORD dwStyle)
{
	if (!Create(NULL, _T("SimpleReelmap"), dwStyle, CRect(0, 0, 0, 0), m_pParent, (UINT)this))
	{
		AfxMessageBox(_T("CSimpleReelmap::Create() Failed!!!"));
		return FALSE;
	}

	return TRUE;
}


void CSimpleReelmap::ProcThrd(const LPVOID lpContext)
{
	CSimpleReelmap* pSimpleReelmap = reinterpret_cast<CSimpleReelmap*>(lpContext);

	while (pSimpleReelmap->ThreadIsAlive())
	{
		if (!pSimpleReelmap->ProcReelmap())
			break;
	}

	pSimpleReelmap->ThreadEnd();
}

BOOL CSimpleReelmap::ProcReelmap()
{
	int nSerial = 0;
	if (IsShare(nSerial) && !m_bLock)
	{
		m_bLock = TRUE;
		Add(nSerial);
		ShiftToBuffer(nSerial);
		m_bLock = FALSE;
	}
	Sleep(100);
	return TRUE;
}

void CSimpleReelmap::ThreadStart()
{
	m_bThreadStateEnd = FALSE;
	m_bThreadAlive = TRUE;
	t1 = std::thread(ProcThrd, this);
}

void CSimpleReelmap::ThreadStop()
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

void CSimpleReelmap::ThreadEnd()
{
	m_bThreadStateEnd = TRUE;
}

BOOL CSimpleReelmap::ThreadIsAlive()
{
	return m_bThreadAlive;
}

void CSimpleReelmap::Init(stRmapInfo stInfo) //int nMaxRow, int nMaxCol, int nActionCode
{
	m_stInfo = stInfo;
	//m_stInfo.m_nMaxRow = stInfo.m_nMaxRow;
	//m_stInfo.m_nMaxRow = stInfo.m_nMaxRow;
	//m_stInfo.m_nActionCode = stInfo.m_nActionCode;
}

BOOL CSimpleReelmap::IsShare(int &nSerial)
{
	CFileFind cFile;
	CString sPath;
	sPath.Format(_T("%s\\*.pcr"), PATH_SHARE);

	BOOL bExist = cFile.FindFile(sPath);
	if (!bExist)
		return FALSE; // pcr파일이 존재하지 않음.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// 파일명을 얻음.
			sFileName = cFile.GetFileName();
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
				sSerial = sFileName.Left(nPos);

			nSerial = _tstoi(sSerial);
			if (nSerial > 0)
				return TRUE;
			else
			{
				nSerial = 0;
				return FALSE;
			}
		}
	}

	return FALSE;
}

void CSimpleReelmap::InitDef()
{
	m_cBigDef[0] = '*';		//	NONE
	m_cBigDef[1] = 'N';		//	DEF_NICK
	m_cBigDef[2] = 'D';		//	DEF_PROTRUSION
	m_cBigDef[3] = 'A';		//	DEF_SPACE
	m_cBigDef[4] = 'O';		//	DEF_OPEN
	m_cBigDef[5] = 'S';		//	DEF_SHORT
	m_cBigDef[6] = 'U';		//	DEF_USHORT
	m_cBigDef[7] = 'I';		//	DEF_PINHOLE
	m_cBigDef[8] = 'H';		//	DEF_HOLE_MISS
	m_cBigDef[9] = 'E';		//	DEF_EXTRA
	m_cBigDef[10] = 'P';	//	DEF_PAD
	m_cBigDef[11] = 'L';	//	DEF_HOLE_POSITION
	m_cBigDef[12] = 'X';	//	DEF_POI
	m_cBigDef[13] = 'T';	//	DEF_VH_POSITION
	m_cBigDef[14] = 'M';	//	DEF_VH_MISS
	m_cBigDef[15] = 'F';	//	DEF_HOLE_DEFECT		'F'
	m_cBigDef[16] = 'C';	//	DEF_HOLE_OPEN
	m_cBigDef[17] = 'G';	//	DEF_VH_OPEN
	m_cBigDef[18] = 'V';	//	DEF_VH_DEF
	m_cBigDef[19] = 'K';	//	DEF_EDGE_NICK
	m_cBigDef[20] = 'R';	//	DEF_EDGE_PROT
	m_cBigDef[21] = 'B';	//	DEF_EDGE_SPACE
	m_cBigDef[22] = 'J';	//	DEF_USER_DEFINE_1
	m_cBigDef[23] = 'Q';	//	DEF_NARROW
	m_cBigDef[24] = 'W';	//	DEF_WIDE
	m_cBigDef[25] = 'F';	//	DEF_FIXED_DEF		'F'
	m_cBigDef[26] = 'Y';	//	DEF_VH_SIZE
	m_cBigDef[27] = 'Z';	//	DEF_VH_EDGE
	m_cBigDef[28] = '?';	//	DEF_LIGHT
	m_cBigDef[29] = '@';	//	DEF_INNER

	m_cSmallDef[0] = '*';	//	NONE
	m_cSmallDef[1] = 'n';	//	DEF_NICK
	m_cSmallDef[2] = 'd';	//	DEF_PROTRUSION
	m_cSmallDef[3] = 'a';	//	DEF_SPACE
	m_cSmallDef[4] = 'o';	//	DEF_OPEN
	m_cSmallDef[5] = 's';	//	DEF_SHORT
	m_cSmallDef[6] = 'u';	//	DEF_USHORT
	m_cSmallDef[7] = 'i';	//	DEF_PINHOLE
	m_cSmallDef[8] = 'h';	//	DEF_HOLE_MISS
	m_cSmallDef[9] = 'e';	//	DEF_EXTRA
	m_cSmallDef[10] = 'p';	//	DEF_PAD
	m_cSmallDef[11] = 'l';	//	DEF_HOLE_POSITION
	m_cSmallDef[12] = 'x';	//	DEF_POI
	m_cSmallDef[13] = 't';	//	DEF_VH_POSITION
	m_cSmallDef[14] = 'm';	//	DEF_VH_MISS
	m_cSmallDef[15] = 'f';	//	DEF_HOLE_DEFECT		'f'
	m_cSmallDef[16] = 'c';	//	DEF_HOLE_OPEN
	m_cSmallDef[17] = 'g';	//	DEF_VH_OPEN
	m_cSmallDef[18] = 'v';	//	DEF_VH_DEF
	m_cSmallDef[19] = 'k';	//	DEF_EDGE_NICK
	m_cSmallDef[20] = 'r';	//	DEF_EDGE_PROT
	m_cSmallDef[21] = 'b';	//	DEF_EDGE_SPACE
	m_cSmallDef[22] = 'j';	//	DEF_USER_DEFINE_1
	m_cSmallDef[23] = 'q';	//	DEF_NARROW
	m_cSmallDef[24] = 'w';	//	DEF_WIDE
	m_cSmallDef[25] = 'f';	//	DEF_FIXED_DEF		'f'
	m_cSmallDef[26] = 'y';	//	DEF_VH_SIZE
	m_cSmallDef[27] = 'z';	//	DEF_VH_EDGE
	m_cSmallDef[28] = '?';	//	DEF_LIGHT
	m_cSmallDef[29] = '@';	//	DEF_INNER
}

void CSimpleReelmap::InitColor()
{
	m_rgbDef[DEF_NONE] = (RGB_WHITE);					// 0=불량없음
	m_rgbDef[DEF_NICK] = (RGB_MAGENTA);					// 1=결손
	m_rgbDef[DEF_PROTRUSION] = (RGB_SKYBLUE);			// 2=돌기
	m_rgbDef[DEF_SPACE] = (RGB_LTGREEN);				// 3=선간폭
	m_rgbDef[DEF_OPEN] = (RGB_LTRED);					// 4=오픈
	m_rgbDef[DEF_SHORT] = (RGB_RED);					// 5=쇼트
	m_rgbDef[DEF_USHORT] = (RGB_LTCYAN);				// 6=u쇼트
	m_rgbDef[DEF_PINHOLE] = (RGB_LLTGREEN);				// 7=핀홀
	m_rgbDef[DEF_HOLE_MISS] = (RGB_LTBLUE);				// 8=홀없음
	m_rgbDef[DEF_EXTRA] = (RGB_CLOUDBLUE);				// 9=잔동
	m_rgbDef[DEF_PAD] = (RGB_LTPURPLE);					// 10=패드
	m_rgbDef[DEF_HOLE_POSITION] = (RGB_PINK);			// 11=홀편심
	m_rgbDef[DEF_POI] = (RGB_LTMAGENTA);				// 12=POI
	m_rgbDef[DEF_VH_POSITION] = (RGB_LTYELLOW);			// 13=VH편심
	m_rgbDef[DEF_VH_MISS] = (RGB_BOON);					// 14=VH없음
	m_rgbDef[DEF_HOLE_DEFECT] = (RGB_LTPINK);			// 15=홀불량
	m_rgbDef[DEF_HOLE_OPEN] = (RGB_LTGREEN);			// 16=홀오픈
	m_rgbDef[DEF_VH_OPEN] = (RGB_LT_DARKMAGENTA);		// 17=VH오픈
	m_rgbDef[DEF_VH_DEF] = (RGB_ORANGE);				// 18=VH결함
	m_rgbDef[DEF_EDGE_NICK] = (RGB_MUSTARD);			// 19 = E.결손
	m_rgbDef[DEF_EDGE_PROT] = (RGB_SKY);				// 20 = E.돌기
	m_rgbDef[DEF_EDGE_SPACE] = (RGB_LT_BROWN);			// 21 = E.선간폭
	m_rgbDef[DEF_USER_DEFINE_1] = (RGB_PEACOCK_GREEN);	// 22 = UDD1
	m_rgbDef[DEF_NARROW] = (RGB_PURPLE);				// 23 = Narrow
	m_rgbDef[DEF_WIDE] = (RGB_FOREST_GREEN);			// 24 = Wide
	m_rgbDef[DEF_FIXED_DEF] = (RGB_RED);				// 25 = 고정불량
	m_rgbDef[DEF_VH_SIZE] = (RGB_PURPLE);				// 26 = VH크기
	m_rgbDef[DEF_VH_EDGE] = (RGB_EVER_GREEN);			// 27 = VH에지결함
	m_rgbDef[DEF_LIGHT] = (RGB_YELLOW);					// 28 = 노광불량
	m_rgbDef[DEF_INNER] = (RGB_PCS_OUT);				// 29 = 내층불량
}

BOOL CSimpleReelmap::ShiftToBuffer(int nSerial)
{
	CString sSrc, sDest;
	sSrc.Format(_T("%s\\%04d.pcr"), PATH_SHARE, nSerial);
	sDest.Format(_T("%s\\%04d.pcr"), PATH_BUFFER, nSerial);

	if (!CopyPcr(sSrc, sDest))
		return FALSE;
	if (!DelPcr(sSrc))
		return FALSE;

	return TRUE;
}

BOOL CSimpleReelmap::CopyPcr(CString sSrc, CString sDest)
{
	CString sFolder, sRemain, sSrcFolder, sFile;
	CString sFilePath, sDestFolder;
	CFileFind cFile;
	BOOL bExist;
	int nPos;

	// 파일명을 얻음.
	nPos = sDest.ReverseFind('\\');
	sFile = sDest.Right(sDest.GetLength() - nPos - 1);
	if (sFile.Find(_T("."), 0) != -1)
	{
		nPos = sFile.Find(_T("."), 0);
		sFile.Delete(nPos, sFile.GetLength() - nPos);
	}
	sFolder = ParseFolderName(sDest, sRemain);
	if (sFolder.IsEmpty()) return FALSE; // 폴더가 존재하지 않음.
	sDestFolder = sRemain + _T("\\") + sFolder + _T("\\");

	sFolder = ParseFolderName(sSrc, sRemain);
	if (sFolder.IsEmpty()) return FALSE; // 폴더가 존재하지 않음.
	sSrcFolder = sRemain + _T("\\") + sFolder + _T("\\");

	bExist = cFile.FindFile(sSrcFolder + _T("*.pcr"));
	if (!bExist) return FALSE; // pcr파일이 존재하지 않음.

	if (!Copy(sSrc, sDest)) return FALSE; // Copy 실패.
	return TRUE;
}

CString CSimpleReelmap::ParseFolderName(CString sPathName, CString &sRemainPathName)
{
	CString sFolderName = sPathName;
	int pos = sFolderName.ReverseFind('\\');
	if (pos != -1)
	{
		sFolderName.Delete(pos, sFolderName.GetLength() - pos);
		sRemainPathName = sFolderName;
	}

	pos = sFolderName.ReverseFind('\\');
	if (pos != -1)
	{
		sFolderName = sFolderName.Right(sFolderName.GetLength() - pos - 1);
		sRemainPathName.Delete(pos, sRemainPathName.GetLength() - pos);
	}
	else
	{
		sRemainPathName = sFolderName;
		sFolderName.Empty();
	}

	return sFolderName;
}

BOOL CSimpleReelmap::Copy(CString sSource, CString sDest)
{
	if (!CopyFile(sSource, sDest, TRUE))
		return FALSE;

	return TRUE;
}

BOOL CSimpleReelmap::DelPcr(CString sPath)
{
	CFileFind cFile;
	BOOL bExist;

	bExist = cFile.FindFile(sPath);
	if (!bExist)
		return FALSE;

	if (!DeleteFile(sPath))
		return FALSE;

	return TRUE; // Sucess.
}

BOOL CSimpleReelmap::GetResult()
{
	int j, k;
	//int nTotPcr = m_arPcr.GetCount();
	//if (nTotPcr < 1) return FALSE;
	//int nMaxStrip = m_stInfo.m_nMaxStrip;
	//int nPnlPcs = m_stInfo.m_nMaxCol * m_stInfo.m_nMaxRow;

	int nTotPcr = m_arPcrYield.GetCount();
	if (nTotPcr < 1) return FALSE;
	CPcrYield PcrYield = m_arPcrYield.GetAt(nTotPcr - 1);
	
	m_stResult.nTotalPcr = nTotPcr;
	m_stResult.nTotStrip = nTotPcr * MAX_STRIP; //nTotPcr * nMaxStrip;
	m_stResult.nTotStOut = PcrYield.GetTotalStripOut();
	m_stResult.nTotPcs = PcrYield.GetTotalPcs(); //nTotPcr * nPnlPcs;
	m_stResult.nTotGood = PcrYield.GetTotalGood();
	m_stResult.nTotBad = PcrYield.GetTotalBad();
	
	for (j = 0; j < MAX_STRIP; j++)
	{
		m_stResult.nStripOut[j] = PcrYield.GetStripOut(j);
		m_stResult.nStripDef[j] = PcrYield.GetStripTotalBad(j);
		m_stResult.nTotDef[j] = PcrYield.GetStripTotalBad(j);
		for (k = 0; k < MAX_DEF; k++)
		{
			if (!j) m_stResult.nTotDef[k] = PcrYield.GetDefNum(j);
			m_stResult.nDefPerStrip[j][k] = PcrYield.GetStripDefNum(j, k);
		}
	}

	return TRUE;
}

CString CSimpleReelmap::GetTextResult()
{
	CString sData = _T("");
	CString sTemp, sVal;
	COleDateTime time;
	int j, k;

	if (!GetResult()) return sData;

	CString sMcName = m_stInfo.m_sMcName;
	CString sUserName = m_stInfo.m_sUserName;
	CString sModel = m_stInfo.m_sModel;
	CString sLot = m_stInfo.m_sLot;
	CString sLayer = m_stInfo.m_sLayer;
	CString sLayerUp = m_stInfo.m_sLayerUp;
	CString sLayerDn = m_stInfo.m_sLayerDn;
	CString sItsCode = m_stInfo.m_sItsCode;
	CString sProcessCode = m_stInfo.m_sProcessCode;
	double dEntireSpeed = m_stInfo.m_dEntireSpeed;
	COleDateTime timeLotStart = m_stInfo.m_timeLotStart;
	COleDateTime timeLotRun = m_stInfo.m_timeLotRun;
	COleDateTime timeLotEnd = m_stInfo.m_timeLotEnd;
	int nLastSerial = m_stInfo.m_nLastSerial;
	int nActionCode = m_stInfo.m_nActionCode;
	int nNodeY = m_stInfo.m_nMaxRow;
	int nNodeX = m_stInfo.m_nMaxCol;
	int nMaxStrip = m_stInfo.m_nMaxStrip;
	int nMaxDefCode = m_stInfo.m_nMaxDefCode;
	time = timeLotRun - timeLotStart;

	int nTotalPcr = m_stResult.nTotalPcr;
	if (nTotalPcr < 1) return _T("");
	int nTotStrip = m_stResult.nTotStrip;
	int nTotStOut = m_stResult.nTotStOut;
	int nTotPcs = m_stResult.nTotPcs; 
	int nTotGood = m_stResult.nTotGood;
	int nTotBad = m_stResult.nTotBad;
	int nStripOut[MAX_STRIP];
	int nStripDef[MAX_STRIP];
	int nTotDef[MAX_DEF];
	int nDefPerStrip[MAX_STRIP][MAX_DEF];
	for (j = 0; j < MAX_STRIP; j++)
	{
		nStripOut[j] = m_stResult.nStripOut[j];
		nStripDef[j] = m_stResult.nStripDef[j];
		for (k = 0; k < MAX_DEF; k++)
		{
			if(!j) nTotDef[k] = m_stResult.nTotDef[k];
			nDefPerStrip[j][k] = m_stResult.nDefPerStrip[j][k];
		}
	}

	//리포트 작성. =====================================================================

	sData = _T("");
	sData += _T("1. 일반 정보\r\n");

	sData += _T("\t장비호기명 : ");
	sData += sMcName;
	sData += _T("\r\n");
	sData += _T("\t운  용  자  : ");
	sData += sUserName;
	sData += _T("\r\n");
	sData += _T("\t모      델  : ");
	sData += sModel;
	sData += _T("\r\n");
	sData += _T("\t로      트  : ");
	sData += sLot;
	sData += _T("\r\n");
	sData += _T("\t상면레이어 : ");
	sData += sLayerUp;
	sData += _T("\r\n");
	sData += _T("\t하면레이어 : ");
	sData += sLayerDn;
	sData += _T("\r\n");
	sData += _T("\tITS 코드  : ");
	sData += sItsCode;
	sData += _T("\r\n");
	sData += _T("\t공정 코드  : ");
	sData += sProcessCode;
	sData += _T("\r\n");
	sData += _T("\t전체 속도  : ");
	sTemp.Format(_T("%.2f"), dEntireSpeed);
	sData += sTemp;
	sData += _T("\r\n");
	sData += _T("\t작업 시작  : ");
	sTemp.Format(_T("%s"), timeLotStart.Format(_T("%Y/%m/%d %H:%M:%S")));
	sData += sTemp;
	sData += _T("\r\n");
	sData += _T("\t진행 시간  : ");
	sTemp.Format(_T("%s"), time.Format(_T("%H:%M:%S")));
	sData += sTemp;
	sData += _T("\r\n");
	sData += _T("\t작업 종료  : ");
	sTemp.Format(_T("%s"), timeLotEnd.Format(_T("%Y/%m/%d %H:%M:%S")));
	sData += sTemp;
	sData += _T("\r\n");
	sData += _T("\r\n");

	sData += _T("2. 제품 검사 결과\r\n");
	sData += _T("\t----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	sData += _T("\t  구분  \t\t\t\t\t      검 사  수 량\t\r\n");
	sData += _T("\t----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	sData += _T("\t        \t\t   총 검 사\t\t   양 품 수\t\t   불 량 수\t\t   양 품 율\t\t   불 량 율\t\r\n");
	sData += _T("\t----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	sTemp.Format(_T("\t Strip  \t\t%8d\t\t%8d\t\t%8d\t\t%8.2f\t\t%8.2f\t\r\n"), nTotStrip, nTotStrip - nTotStOut, nTotStOut,
		100.0*(double)(nTotStrip - nTotStOut) / (double)nTotStrip, 100.0*(double)nTotStOut / (double)nTotStrip);
	sData += sTemp;
	sData += _T("\t----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	sTemp.Format(_T("\t  Unit  \t\t%8d\t\t%8d\t\t%8d\t\t%8.2f\t\t%8.2f\t\r\n"), nTotPcs, nTotGood, nTotBad,
		(nTotPcs > 0) ? 100.0*(double)(nTotGood) / (double)(nTotPcs) : 0.0,
		(nTotPcs > 0) ? 100.0*(double)(nTotBad) / (double)(nTotPcs) : 0.0);
	sData += sTemp;
	sData += _T("\t----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	sData += _T("\r\n");


	sData += _T("3. 열별 검사 결과\r\n");
	sData += _T("\t----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	sData += _T("\t\t  구분  \t\t\t     1 열  \t\t     2 열  \t\t     3 열  \t\t     4 열  \t\r\n");
	sData += _T("\t----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	sTemp.Format(_T("\t검 사 수\t\t Strip  \t\t%8d\t\t%8d\t\t%8d\t\t%8d\t\r\n"), nTotStrip / nMaxStrip, nTotStrip / nMaxStrip, nTotStrip / nMaxStrip, nTotStrip / nMaxStrip);
	sData += sTemp;
	sData += _T("\t        \t\t------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	sTemp.Format(_T("\t        \t\t  Unit  \t\t%8d\t\t%8d\t\t%8d\t\t%8d\t\r\n"), nTotPcs / nMaxStrip, nTotPcs / nMaxStrip, nTotPcs / nMaxStrip, nTotPcs / nMaxStrip);
	sData += sTemp;
	sData += _T("\t----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sTemp.Format(_T("\t양 품 수\t\t Strip  \t\t%8d\t\t%8d\t\t%8d\t\t%8d\t\r\n"), nTotStrip / nMaxStrip - nStripOut[0], nTotStrip / nMaxStrip - nStripOut[1], nTotStrip / nMaxStrip - nStripOut[2], nTotStrip / nMaxStrip - nStripOut[3]);
	sData += sTemp;
	sData += _T("\t        \t\t------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	sTemp.Format(_T("\t        \t\t  Unit  \t\t%8d\t\t%8d\t\t%8d\t\t%8d\t\r\n"), nTotPcs / nMaxStrip - nStripDef[0], nTotPcs / nMaxStrip - nStripDef[1], nTotPcs / nMaxStrip - nStripDef[2], nTotPcs / nMaxStrip - nStripDef[3]);
	sData += sTemp;
	sData += _T("\t----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sTemp.Format(_T("\t불 량 수\t\t Strip  \t\t%8d\t\t%8d\t\t%8d\t\t%8d\t\r\n"), nStripOut[0], nStripOut[1], nStripOut[2], nStripOut[3]);
	sData += sTemp;
	sData += _T("\t        \t\t------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	sTemp.Format(_T("\t        \t\t  Unit  \t\t%8d\t\t%8d\t\t%8d\t\t%8d\t\r\n"), (nStripDef[0]), (nStripDef[1]), (nStripDef[2]), (nStripDef[3]));
	sData += sTemp;
	sData += _T("\t----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sTemp.Format(_T("\t양 품 율\t\t Strip  \t\t%8.2f\t\t%8.2f\t\t%8.2f\t\t%8.2f\t\r\n"), 100.0*(double)(nTotStrip / nMaxStrip - nStripOut[0]) / (double)(nTotStrip / nMaxStrip), 100.0*(double)(nTotStrip / nMaxStrip - nStripOut[1]) / (double)(nTotStrip / nMaxStrip),
		100.0*(double)(nTotStrip / nMaxStrip - nStripOut[2]) / (double)(nTotStrip / nMaxStrip), 100.0*(double)(nTotStrip / nMaxStrip - nStripOut[3]) / (double)(nTotStrip / nMaxStrip));
	sData += sTemp;
	sData += _T("\t        \t\t------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	sTemp.Format(_T("\t   %%   \t\t  Unit  \t\t%8.2f\t\t%8.2f\t\t%8.2f\t\t%8.2f\t\r\n"), 100.0*(double)(nTotPcs / nMaxStrip - nStripDef[0]) / (double)((nTotPcs) / nMaxStrip), 100.0*(double)(nTotPcs / nMaxStrip - nStripDef[1]) / (double)((nTotPcs) / nMaxStrip),
		100.0*(double)(nTotPcs / nMaxStrip - nStripDef[2]) / (double)((nTotPcs) / nMaxStrip), 100.0*(double)(nTotPcs / nMaxStrip - nStripDef[3]) / (double)((nTotPcs) / nMaxStrip));
	sData += sTemp;
	sData += _T("\t----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sTemp.Format(_T("\t불 량 율\t\t Strip  \t\t%8.2f\t\t%8.2f\t\t%8.2f\t\t%8.2f\t\r\n"), 100.0*(double)nStripOut[0] / (double)(nTotStrip / nMaxStrip), 100.0*(double)nStripOut[1] / (double)(nTotStrip / nMaxStrip),
		100.0*(double)nStripOut[2] / (double)(nTotStrip / nMaxStrip), 100.0*(double)nStripOut[3] / (double)(nTotStrip / nMaxStrip));
	sData += sTemp;
	sData += _T("\t        \t\t------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	sTemp.Format(_T("\t   %%   \t\t  Unit  \t\t%8.2f\t\t%8.2f\t\t%8.2f\t\t%8.2f\t\r\n"), 100.0*(double)(nStripDef[0]) / (double)((nTotPcs) / nMaxStrip), 100.0*(double)(nStripDef[1]) / (double)((nTotPcs) / nMaxStrip),
		100.0*(double)(nStripDef[2]) / (double)((nTotPcs) / nMaxStrip), 100.0*(double)(nStripDef[3]) / (double)((nTotPcs) / nMaxStrip));
	sData += sTemp;
	sData += _T("\t----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sData += _T("\r\n");


	sData += _T("4. 불량 내역\r\n");
	sData += _T("\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	sData += _T("\t번호\t불량유형\t\t1열\t\t2열\t\t3열\t\t4열\t\t유형별 불량수\t\r\n");
	sData += _T("\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sTemp.Format(_T("\t1\t%s\t\t"), m_sKorDef[DEF_OPEN]);
	sData += sTemp; // "\t 1\t    오픈    \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_OPEN]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_OPEN]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_OPEN]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_OPEN]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_OPEN]);
	sData += sTemp;
	sData += _T("\r\n");
	sData += _T("\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sTemp.Format(_T("\t2\t%s\t\t"), m_sKorDef[DEF_SHORT]);
	sData += sTemp;
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_SHORT]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_SHORT]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_SHORT]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_SHORT]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_SHORT]);
	sData += sTemp;
	sData += _T("\r\n");

	sTemp.Format(_T("\t3\t%s\t\t"), m_sKorDef[DEF_USHORT]);
	sData += sTemp;
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_USHORT]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[0][DEF_SHORT] + nDefPerStrip[0][DEF_USHORT]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_USHORT]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[1][DEF_SHORT] + nDefPerStrip[1][DEF_USHORT]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_USHORT]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[2][DEF_SHORT] + nDefPerStrip[2][DEF_USHORT]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_USHORT]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[3][DEF_SHORT] + nDefPerStrip[3][DEF_USHORT]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_USHORT]);
	sData += sTemp;
	sTemp.Format(_T("  \t%3d"), nTotDef[DEF_SHORT] + nTotDef[DEF_USHORT]);
	sData += sTemp;
	sData += _T("\r\n");
	sData += _T("\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sTemp.Format(_T("\t4\t%s\t\t"), m_sKorDef[DEF_NICK]);
	sData += sTemp; // "\t 4\t    결손    \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_NICK]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_NICK]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_NICK]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_NICK]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_NICK]);
	sData += sTemp;
	sData += _T("\r\n");
	sData += _T("\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sTemp.Format(_T("\t5\t%s\t\t"), m_sKorDef[DEF_SPACE]);
	sData += sTemp; // "\t 5\t  선간폭  \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_SPACE]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_SPACE]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_SPACE]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_SPACE]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_SPACE]);
	sData += sTemp;
	sData += _T("\r\n");

	sTemp.Format(_T("\t6\t%s\t\t"), m_sKorDef[DEF_EXTRA]);
	sData += sTemp; // "\t 6\t    잔동    \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_EXTRA]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_EXTRA]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_EXTRA]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_EXTRA]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_EXTRA]);
	sData += sTemp;
	sData += _T("\r\n");

	sTemp.Format(_T("\t7\t%s\t\t"), m_sKorDef[DEF_PROTRUSION]);
	sData += sTemp; // "\t 7\t    돌기    \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_PROTRUSION]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[0][DEF_SPACE] + nDefPerStrip[0][DEF_EXTRA] + nDefPerStrip[0][DEF_PROTRUSION]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_PROTRUSION]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[1][DEF_SPACE] + nDefPerStrip[1][DEF_EXTRA] + nDefPerStrip[1][DEF_PROTRUSION]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_PROTRUSION]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[2][DEF_SPACE] + nDefPerStrip[2][DEF_EXTRA] + nDefPerStrip[2][DEF_PROTRUSION]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_PROTRUSION]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[3][DEF_SPACE] + nDefPerStrip[3][DEF_EXTRA] + nDefPerStrip[3][DEF_PROTRUSION]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_PROTRUSION]);
	sData += sTemp;
	sTemp.Format(_T("  \t%3d"), nTotDef[DEF_SPACE] + nTotDef[DEF_EXTRA] + nTotDef[DEF_PROTRUSION]);
	sData += sTemp;
	sData += _T("\r\n");
	sData += _T("\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sTemp.Format(_T("\t8\t%s\t\t"), m_sKorDef[DEF_PINHOLE]);
	sData += sTemp; // "\t 8\t    핀홀    \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_PINHOLE]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_PINHOLE]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_PINHOLE]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_PINHOLE]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_PINHOLE]);
	sData += sTemp;
	sData += _T("\r\n");

	sTemp.Format(_T("\t9\t%s\t\t"), m_sKorDef[DEF_PAD]);
	sData += sTemp; // "\t 9\t패드결함\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_PAD]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[0][DEF_PINHOLE] + nDefPerStrip[0][DEF_PAD]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_PAD]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[1][DEF_PINHOLE] + nDefPerStrip[1][DEF_PAD]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_PAD]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[2][DEF_PINHOLE] + nDefPerStrip[2][DEF_PAD]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_PAD]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[3][DEF_PINHOLE] + nDefPerStrip[3][DEF_PAD]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_PAD]);
	sData += sTemp;
	sTemp.Format(_T("  \t%3d"), nTotDef[DEF_PINHOLE] + nTotDef[DEF_PAD]);
	sData += sTemp;
	sData += _T("\r\n");
	sData += _T("\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sTemp.Format(_T("\t10\t%s\t\t"), m_sKorDef[DEF_HOLE_OPEN]);
	sData += sTemp; // "\t10\t  홀오픈  \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_HOLE_OPEN]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_HOLE_OPEN]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_HOLE_OPEN]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_HOLE_OPEN]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_HOLE_OPEN]);
	sData += sTemp;
	sData += _T("\r\n");
	sData += _T("\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sTemp.Format(_T("\t11\t%s\t\t"), m_sKorDef[DEF_HOLE_MISS]);
	sData += sTemp; // "\t11\t  홀없음  \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_HOLE_MISS]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_HOLE_MISS]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_HOLE_MISS]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_HOLE_MISS]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_HOLE_MISS]);
	sData += sTemp;
	sData += _T("\r\n");

	sTemp.Format(_T("\t12\t%s\t\t"), m_sKorDef[DEF_HOLE_POSITION]);
	sData += sTemp; // "\t12\t  홀편심  \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_HOLE_POSITION]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_HOLE_POSITION]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_HOLE_POSITION]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_HOLE_POSITION]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_HOLE_POSITION]);
	sData += sTemp;
	sData += _T("\r\n");

	sTemp.Format(_T("\t13\t%s\t\t"), m_sKorDef[DEF_HOLE_DEFECT]);
	sData += sTemp; // "\t13\t홀내불량\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_HOLE_DEFECT]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[0][DEF_HOLE_MISS] + nDefPerStrip[0][DEF_HOLE_POSITION] + nDefPerStrip[0][DEF_HOLE_DEFECT]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_HOLE_DEFECT]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[1][DEF_HOLE_MISS] + nDefPerStrip[1][DEF_HOLE_POSITION] + nDefPerStrip[1][DEF_HOLE_DEFECT]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_HOLE_DEFECT]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[2][DEF_HOLE_MISS] + nDefPerStrip[2][DEF_HOLE_POSITION] + nDefPerStrip[2][DEF_HOLE_DEFECT]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_HOLE_DEFECT]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[3][DEF_HOLE_MISS] + nDefPerStrip[3][DEF_HOLE_POSITION] + nDefPerStrip[3][DEF_HOLE_DEFECT]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_HOLE_DEFECT]);
	sData += sTemp;
	sTemp.Format(_T("  \t%3d"), nTotDef[DEF_HOLE_MISS] + nTotDef[DEF_HOLE_POSITION] + nTotDef[DEF_HOLE_DEFECT]);
	sData += sTemp;
	sData += _T("\r\n");
	sData += _T("\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sTemp.Format(_T("\t14\t%s\t\t"), m_sKorDef[DEF_POI]);
	sData += sTemp; // "\t14\t      POI      \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_POI]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_POI]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_POI]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_POI]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_POI]);
	sData += sTemp;
	sData += _T("\r\n");
	sData += _T("\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sTemp.Format(_T("\t15\t%s\t\t"), m_sKorDef[DEF_VH_OPEN]);
	sData += sTemp; // "\t15\t  VH오픈  \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_VH_OPEN]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_VH_OPEN]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_VH_OPEN]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_VH_OPEN]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_VH_OPEN]);
	sData += sTemp;
	sData += _T("\r\n");
	sData += _T("\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sTemp.Format(_T("\t16\t%s\t\t"), m_sKorDef[DEF_VH_MISS]);
	sData += sTemp; // "\t16\t  VH없음  \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_VH_MISS]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_VH_MISS]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_VH_MISS]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_VH_MISS]); // MAX_STRIP_NUM
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_VH_MISS]);
	sData += sTemp;
	sData += _T("\r\n");

	sTemp.Format(_T("\t17\t%s\t\t"), m_sKorDef[DEF_VH_POSITION]);
	sData += sTemp; // "\t17\t  VH편심  \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_VH_POSITION]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_VH_POSITION]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_VH_POSITION]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_VH_POSITION]); // MAX_STRIP_NUM
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_VH_POSITION]);
	sData += sTemp;
	sData += _T("\r\n");

	sTemp.Format(_T("\t18\t%s\t\t"), m_sKorDef[DEF_VH_DEF]);
	sData += sTemp; // "\t18\t  VH결함  \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_VH_DEF]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[0][DEF_VH_MISS] + nDefPerStrip[0][DEF_VH_POSITION] + nDefPerStrip[0][DEF_VH_DEF]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_VH_DEF]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[1][DEF_VH_MISS] + nDefPerStrip[1][DEF_VH_POSITION] + nDefPerStrip[1][DEF_VH_DEF]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_VH_DEF]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[2][DEF_VH_MISS] + nDefPerStrip[2][DEF_VH_POSITION] + nDefPerStrip[2][DEF_VH_DEF]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_VH_DEF]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[3][DEF_VH_MISS] + nDefPerStrip[3][DEF_VH_POSITION] + nDefPerStrip[3][DEF_VH_DEF]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_VH_DEF]);
	sData += sTemp;
	sTemp.Format(_T("  \t%3d"), nTotDef[DEF_VH_MISS] + nTotDef[DEF_VH_POSITION] + nTotDef[DEF_VH_DEF]);
	sData += sTemp;
	sData += _T("\r\n");
	sData += _T("\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sTemp.Format(_T("\t19\t%s\t\t"), m_sKorDef[DEF_EDGE_NICK]);
	sData += sTemp; // "\t19\t  E.Nick  \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_EDGE_NICK]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_EDGE_NICK]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_EDGE_NICK]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_EDGE_NICK]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_EDGE_NICK]);
	sData += sTemp;
	sData += _T("\r\n");

	sTemp.Format(_T("\t20\t%s\t\t"), m_sKorDef[DEF_EDGE_PROT]);
	sData += sTemp; // "\t20\t  E.Prot  \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_EDGE_PROT]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_EDGE_PROT]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_EDGE_PROT]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_EDGE_PROT]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_EDGE_PROT]);
	sData += sTemp;
	sData += _T("\r\n");

	sTemp.Format(_T("\t21\t%s\t\t"), m_sKorDef[DEF_EDGE_SPACE]);
	sData += sTemp; // "\t21\t  E.Space  \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_EDGE_SPACE]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[0][DEF_EDGE_NICK] + nDefPerStrip[0][DEF_EDGE_PROT] + nDefPerStrip[0][DEF_EDGE_SPACE]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_EDGE_SPACE]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[1][DEF_EDGE_NICK] + nDefPerStrip[1][DEF_EDGE_PROT] + nDefPerStrip[1][DEF_EDGE_SPACE]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_EDGE_SPACE]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[2][DEF_EDGE_NICK] + nDefPerStrip[2][DEF_EDGE_PROT] + nDefPerStrip[2][DEF_EDGE_SPACE]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_EDGE_SPACE]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[3][DEF_EDGE_NICK] + nDefPerStrip[3][DEF_EDGE_PROT] + nDefPerStrip[3][DEF_EDGE_SPACE]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_EDGE_SPACE]);
	sData += sTemp;
	sTemp.Format(_T("  \t%3d"), nTotDef[DEF_EDGE_NICK] + nTotDef[DEF_EDGE_PROT] + nTotDef[DEF_EDGE_SPACE]);
	sData += sTemp;
	sData += _T("\r\n");
	sData += _T("\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sTemp.Format(_T("\t22\t%s\t\t"), m_sKorDef[DEF_USER_DEFINE_1]);
	sData += sTemp; // "\t22\t  UDD1  \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_USER_DEFINE_1]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_USER_DEFINE_1]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_USER_DEFINE_1]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_USER_DEFINE_1]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_USER_DEFINE_1]);
	sData += sTemp;
	sData += _T("\r\n");
	sData += _T("\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sTemp.Format(_T("\t23\t%s\t\t"), m_sKorDef[DEF_NARROW]);
	sData += sTemp; // "\t23\t  선폭감소  \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_NARROW]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_NARROW]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_NARROW]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_NARROW]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_NARROW]);
	sData += sTemp;
	sData += _T("\r\n");

	sTemp.Format(_T("\t24\t%s\t\t"), m_sKorDef[DEF_WIDE]);
	sData += sTemp; // "\t24\t  선폭증가  \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_WIDE]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[0][DEF_NARROW] + nDefPerStrip[0][DEF_WIDE]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_WIDE]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[1][DEF_NARROW] + nDefPerStrip[1][DEF_WIDE]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_WIDE]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[2][DEF_NARROW] + nDefPerStrip[2][DEF_WIDE]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_WIDE]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[3][DEF_NARROW] + nDefPerStrip[3][DEF_WIDE]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_WIDE]);
	sData += sTemp;
	sTemp.Format(_T("  \t%3d"), nTotDef[DEF_NARROW] + nTotDef[DEF_WIDE]);
	sData += sTemp;
	sData += _T("\r\n");
	sData += _T("\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sTemp.Format(_T("\t25\t%s\t\t"), m_sKorDef[DEF_FIXED_DEF]);
	sData += sTemp; // "\t25\t  고정불량  \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_FIXED_DEF]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_FIXED_DEF]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_FIXED_DEF]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_FIXED_DEF]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_FIXED_DEF]);
	sData += sTemp;
	sData += _T("\r\n");
	sData += _T("\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sTemp.Format(_T("\t26\t%s\t\t"), m_sKorDef[DEF_VH_SIZE]);
	sData += sTemp; // "\t26\t  VH크기  \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_VH_SIZE]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_VH_SIZE]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_VH_SIZE]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_VH_SIZE]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_VH_SIZE]);
	sData += sTemp;
	sData += _T("\r\n");

	sTemp.Format(_T("\t27\t%s\t"), m_sKorDef[DEF_VH_EDGE]);
	sData += sTemp; // "\t27\t  VH에지결함  \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_VH_EDGE]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[0][DEF_VH_SIZE] + nDefPerStrip[0][DEF_VH_EDGE]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_VH_EDGE]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[1][DEF_VH_SIZE] + nDefPerStrip[1][DEF_VH_EDGE]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_VH_EDGE]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[2][DEF_VH_SIZE] + nDefPerStrip[2][DEF_VH_EDGE]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_VH_EDGE]);
	sData += sTemp;
	sTemp.Format(_T("\t%3d"), nDefPerStrip[3][DEF_VH_SIZE] + nDefPerStrip[3][DEF_VH_EDGE]);
	sData += sTemp;
	sData += _T("\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_VH_EDGE]);
	sData += sTemp;
	sTemp.Format(_T("  \t%3d"), nTotDef[DEF_VH_SIZE] + nTotDef[DEF_VH_EDGE]);
	sData += sTemp;
	sData += _T("\r\n");
	sData += _T("\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");

	sTemp.Format(_T("\t28\t%s\t\t"), m_sKorDef[DEF_LIGHT]);
	sData += sTemp; // "\t28\t    노광    \t");
	sTemp.Format(_T("%3d"), nDefPerStrip[0][DEF_LIGHT]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[1][DEF_LIGHT]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[2][DEF_LIGHT]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%3d"), nDefPerStrip[3][DEF_LIGHT]);
	sData += sTemp;
	sData += _T("\t\t");
	sTemp.Format(_T("%13d"), nTotDef[DEF_LIGHT]);
	sData += sTemp;
	sData += _T("\r\n");
	sData += _T("\t-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	sData += _T("\r\n");

	return sData;
}

CString CSimpleReelmap::GetTextConverse()
{
	int nNodeX = m_stInfo.m_nMaxCol;
	int nNodeY = m_stInfo.m_nMaxRow;
	int nStripY = nNodeY / MAX_STRIP;

	CString sData = _T("");
	CString sTemp, sVal;
	int i, j, k;
	COleDateTime time = m_stInfo.m_timeLotRun - m_stInfo.m_timeLotStart;

	sTemp.Format(_T("장비호기명 : %s\r\n"), m_stInfo.m_sMcName);
	sData += sTemp;
	sTemp.Format(_T("운  용  자  : %s\r\n"), m_stInfo.m_sUserName);
	sData += sTemp;
	sTemp.Format(_T("모      델  : %s\r\n"), m_stInfo.m_sModel);
	sData += sTemp;
	sTemp.Format(_T("로      트  : %s\r\n"), m_stInfo.m_sLot);
	sData += sTemp;
	sTemp.Format(_T("상면레이어 : %s\r\n"), m_stInfo.m_sLayerUp);
	sData += sTemp;
	sTemp.Format(_T("하면레이어 : %s\r\n\r\n"), m_stInfo.m_sLayerDn);
	sData += sTemp;
	sTemp.Format(_T("ITS 코드  : %s\r\n"), m_stInfo.m_sItsCode);
	sData += sTemp;
	sTemp.Format(_T("공정 코드  : %s\r\n"), m_stInfo.m_sProcessCode);
	sData += sTemp;
	sTemp.Format(_T("전체 속도  : %.2f\r\n\r\n"), m_stInfo.m_dEntireSpeed);
	sData += sTemp;
	sTemp.Format(_T("작업 시작  : %s\r\n"), m_stInfo.m_timeLotStart.Format(_T("%Y/%m/%d %H:%M:%S")));
	sData += sTemp;
	sTemp.Format(_T("진행 시간  : %s\r\n"), time.Format(_T("%H:%M:%S")));
	sData += sTemp;
	sTemp.Format(_T("작업 종료  : %s\r\n\r\n"), m_stInfo.m_timeLotEnd.Format(_T("%Y/%m/%d %H:%M:%S")));
	sData += sTemp;
	sTemp.Format(_T("ActionCode : %d\r\n"), m_stInfo.m_nActionCode);
	sData += sTemp;
	sTemp.Format(_T("Max Colum  : %d\r\n"), m_stInfo.m_nMaxCol);
	sData += sTemp;
	sTemp.Format(_T("Max Row    : %d\r\n\r\n"), m_stInfo.m_nMaxRow);
	sData += sTemp;
	sTemp.Format(_T("Last Serial : %d\r\n\r\n"), m_stInfo.m_nLastSerial);
	sData += sTemp;

	int nCount = m_arPcr.GetSize();
	CPcr Pcr;
	for (i = 0; i < nCount; i++)
	{
		if (i)	sData += _T("\r\n");		
		Pcr = m_arPcr.GetAt(i);
		int nSerial = Pcr.GetSerial();
		COleDateTime time = Pcr.GetDateTime();
		sTemp.Format(_T("%d\r\n%s\r\n"), nSerial, time.Format(_T("%Y/%m/%d %H:%M:%S")));
		sData += sTemp;

		int nTotDef = Pcr.GetTotalDef();

		short **pPanel;
		pPanel = new short*[nNodeY];
		for (j = 0; j < nNodeY; j++)
		{
			pPanel[j] = new short[nNodeX];
			memset(pPanel[j], 0, sizeof(short)*nNodeX);
		}

		int nR, nC;
		for (k = 0; k < nTotDef; k++)
		{
			int nPcsId = Pcr.GetPcsId(k);
			int nDefCode = Pcr.GetDefCode(k);
			GetMatrix(nPcsId, nR, nC);
			pPanel[nR][nC] = (short)nDefCode;
		}

		sTemp.Format(_T("Total Defects : %d\r\n"), nTotDef);
		sData += sTemp;
		for (j = 0; j < nNodeY; j++)
		{
			if (!(j%nStripY))	sData += _T("\r\n");
			sTemp = _T("");
			for (k = 0; k < nNodeX; k++)
			{
				if (k == nNodeX - 1)
					sVal.Format(_T("%c\r\n"), m_cBigDef[pPanel[j][k]]);	// 불량코드를 2칸으로 설정
				else
					sVal.Format(_T("%c "), m_cBigDef[pPanel[j][k]]);	// 불량코드를 2칸으로 설정
				sTemp += sVal;
			}
			sData += sTemp;
		}

		for (j = 0; j < nNodeY; j++)
			delete[] pPanel[j];
		delete[] pPanel;
	}

	return sData;
}

CString CSimpleReelmap::GetTextArPcr()
{
	int nNodeX = m_stInfo.m_nMaxCol;
	int nNodeY = m_stInfo.m_nMaxRow;
	int nStripY = nNodeY / MAX_STRIP;

	CString sData = _T("");
	CString sTemp, sVal;
	int i, j, k;

	int nCount = m_arPcr.GetSize();
	CPcr Pcr;
	for (i = 0; i < nCount; i++)
	{
		if (i)	sData += _T("\r\n");		
		Pcr = m_arPcr.GetAt(i);
		int nSerial = Pcr.GetSerial();
		COleDateTime time = Pcr.GetDateTime();
		sTemp.Format(_T("%d\r\n%s\r\n"), nSerial, time.Format(_T("%Y/%m/%d %H:%M:%S")));
		sData += sTemp;

		int nTotDef = Pcr.GetTotalDef();

		short **pPanel;
		pPanel = new short*[nNodeY];
		for (j = 0; j < nNodeY; j++)
		{
			pPanel[j] = new short[nNodeX];
			memset(pPanel[j], 0, sizeof(short)*nNodeX);
		}

		int nR, nC;
		for (k = 0; k < nTotDef; k++)
		{
			int nPcsId = Pcr.GetPcsId(k);
			int nDefCode = Pcr.GetDefCode(k);
			GetMatrix(nPcsId, nR, nC);
			pPanel[nR][nC] = (short)nDefCode;
		}

		sTemp.Format(_T("Total Defects : %d\r\n"), nTotDef);
		sData += sTemp;
		for (j = 0; j < nNodeY; j++)
		{
			if (!(j%nStripY))	sData += _T("\r\n");
			sTemp = _T("");
			for (k = 0; k < nNodeX; k++)
			{
				if (k == nNodeX - 1)
					sVal.Format(_T("%2d\r\n"), pPanel[j][k]);	// 불량코드를 2칸으로 설정
				else
					sVal.Format(_T("%2d,"), pPanel[j][k]);	// 불량코드를 2칸으로 설정
				sTemp += sVal;
			}
			sData += sTemp;
		}

		for (j = 0; j < nNodeY; j++)
			delete[] pPanel[j];
		delete[] pPanel;
	}

	return sData;
}

CString CSimpleReelmap::GetTextArPcrYield()
{
	int nNodeX = m_stInfo.m_nMaxCol;
	int nNodeY = m_stInfo.m_nMaxRow;
	int nStripY = nNodeY / MAX_STRIP;	

	CString sData = _T("");
	CString sTemp, sVal;
	int i, j, k;

	int nCount = m_arPcr.GetSize();
	CPcrYield PcrYield;
	for (i = 0; i < nCount; i++)
	{
		if (i)	sData += _T("\r\n");
		PcrYield = m_arPcrYield.GetAt(i);
		int nSerial = PcrYield.GetSerial();
		COleDateTime time = PcrYield.GetDateTime();
		sTemp.Format(_T("%d\r\n%s\r\n"), nSerial, time.Format(_T("%Y/%m/%d %H:%M:%S")));
		sData += sTemp;

		int nTotPcs = PcrYield.GetTotalPcs();
		int nTotGood = PcrYield.GetTotalGood();
		int nTotBad = PcrYield.GetTotalBad();
		int nStripTotPcs = nTotPcs / MAX_STRIP;
		sTemp.Format(_T("Total Pcs : %d\r\n"), nTotPcs);
		sData += sTemp;
		sTemp.Format(_T("Total Good : %d\tTotal Bad : %d\r\n"), nTotGood, nTotBad);
		sData += sTemp;

		int nTotStriptOut = PcrYield.GetTotalStripOut();
		int nStripOut[MAX_STRIP], nStripDef[MAX_STRIP][MAX_DEF], nStripTotDef[MAX_STRIP];
		double dStripDefRatio[MAX_STRIP];
		for (j = 0; j < MAX_STRIP; j++)
		{
			nStripOut[j] = PcrYield.GetStripOut(j);
			nStripTotDef[j] = 0;
			for (k = 1; k < MAX_DEF; k++)
			{
				nStripDef[j][k] = PcrYield.GetStripDefNum(j, k);
				nStripTotDef[j] += nStripDef[j][k];
			}
		}
		sTemp.Format(_T("Total StripOut : %d\r\n"), nTotStriptOut);
		sData += sTemp;
		sTemp.Format(_T("A-StripOut : %d\tB-StripOut : %d\r\n"), nStripOut[0], nStripOut[1]);
		sData += sTemp;
		sTemp.Format(_T("C-StripOut : %d\tD-StripOut : %d\r\n"), nStripOut[2], nStripOut[3]);
		sData += sTemp;

		dStripDefRatio[0] = (double)nStripTotDef[0] / (double)nStripTotPcs;
		dStripDefRatio[1] = (double)nStripTotDef[1] / (double)nStripTotPcs;
		dStripDefRatio[2] = (double)nStripTotDef[2] / (double)nStripTotPcs;
		dStripDefRatio[3] = (double)nStripTotDef[3] / (double)nStripTotPcs;
		sTemp.Format(_T("A열[%%] : %.1f\tB열[%%] : %.1f\r\n"), 100.0 * (1.0 - dStripDefRatio[0]), 100.0 * (1.0 - dStripDefRatio[1]));
		sData += sTemp;
		sTemp.Format(_T("C열[%%] : %.1f\tD열[%%] : %.1f\r\n"), 100.0 *(1.0 - dStripDefRatio[2]), 100.0 * (1.0 - dStripDefRatio[3]));
		sData += sTemp;

		int nDef[MAX_DEF];
		for (k = 1; k < MAX_DEF; k++)
		{
			nDef[k] = PcrYield.GetDefNum(k);
			if (!(k % 3))
				sTemp.Format(_T("%c(%d)\r\n"), GetCodeBigDef(k), nDef[k]);
			else if (k == MAX_DEF - 1)
				sTemp.Format(_T("%c(%d)\r\n"), GetCodeBigDef(k), nDef[k]);
			else
				sTemp.Format(_T("%c(%d)\t"), GetCodeBigDef(k), nDef[k]);
			sData += sTemp;
		}
	}

	return sData;
}

BOOL CSimpleReelmap::Add(int nSerial)
{
	int i;
	BOOL bAdd = FALSE;

	CPcr Pcr[2];
	Pcr[0].Init(nSerial); // LoadPcr()
	CPcrMark PcrMark;
	CPcrYield PcrYield[2];

	int nTotDef = 0;
	int nCount = m_arPcr.GetSize();
	for (i = 0; i < nCount; i++)
	{
		Pcr[1] = m_arPcr.GetAt(i);
		if (Pcr[1].GetSerial() == nSerial)
		{
			Pcr[1].Free();
			m_arPcr.SetAt(i, Pcr[0]); // 인덱스(i)에 값 Pcr[0] 입력

			nTotDef = Pcr[1].GetTotalDef();
			PcrMark.Init(nSerial, nTotDef, this); 
			m_arPcrMark.SetAt(i, PcrMark);

			if (i > 0)
			{
				PcrYield[1] = m_arPcrYield.GetAt(i-1);				// PrevPcrYield
				PcrYield[0].Init(nSerial, Pcr[0], m_stInfo.m_nMaxRow, m_stInfo.m_nMaxCol, 20.0, this, &PcrYield[1]);	// UpdatePcrYield()
			}
			else
				PcrYield[0].Init(nSerial, Pcr[0], m_stInfo.m_nMaxRow, m_stInfo.m_nMaxCol, 20.0, this);				// UpdatePcrYield()
			m_arPcrYield.SetAt(i, PcrYield[0]); // 인덱스(i)에 값 PcrYield[0] 입력

			bAdd = TRUE;
			break;
		}
	}

	if (!bAdd)
	{
		m_arPcr.Add(Pcr[0]);
		m_stInfo.m_nLastSerial = nSerial;

		nTotDef = Pcr[0].GetTotalDef();
		PcrMark.Init(nSerial, nTotDef, this);
		m_arPcrMark.Add(PcrMark);

		nCount = m_arPcrYield.GetSize();
		if (nCount > 0)
		{
			PcrYield[1] = m_arPcrYield.GetAt(nCount - 1);									// PrevPcrYield
			PcrYield[0].Init(nSerial, Pcr[0], m_stInfo.m_nMaxRow, m_stInfo.m_nMaxCol, 20.0, this, &PcrYield[1]);	// UpdatePcrYield()
		}
		else
			PcrYield[0].Init(nSerial, Pcr[0], m_stInfo.m_nMaxRow, m_stInfo.m_nMaxCol, 20.0, this);				// UpdatePcrYield()
		m_arPcrYield.Add(PcrYield[0]);
	}

	return TRUE;
}

BOOL CSimpleReelmap::Save()
{
	BOOL bRtn[4] = { 0, 0, 0, 0 };
	bRtn[0] = SaveInfo();
	bRtn[1] = SaveRmap();
	bRtn[2] = SaveYield();
	bRtn[3] = SaveMark();

	return (bRtn[0] && bRtn[1] && bRtn[2] && bRtn[3]);
}

BOOL CSimpleReelmap::SaveInfo()
{
	CFile cfile;
	if (!cfile.Open(m_sPathInfo, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary | CFile::shareDenyNone, NULL))
	{
		AfxMessageBox(_T("Fail to save Info."));
		return FALSE;
	}

	CString sMcName = m_stInfo.m_sMcName;
	CString sUserName = m_stInfo.m_sUserName;
	CString sModel = m_stInfo.m_sModel;
	CString sLot = m_stInfo.m_sLot;
	CString sLayer = m_stInfo.m_sLayer;
	CString sLayerUp = m_stInfo.m_sLayerUp;
	CString sLayerDn = m_stInfo.m_sLayerDn;
	CString sItsCode = m_stInfo.m_sItsCode;
	CString sProcessCode = m_stInfo.m_sProcessCode;

	sMcName += ",";
	cfile.Write(sMcName, sMcName.GetLength() * sizeof(TCHAR));
	sUserName += ",";
	cfile.Write(sUserName, sUserName.GetLength() * sizeof(TCHAR));
	sModel += ",";
	cfile.Write(sModel, sModel.GetLength() * sizeof(TCHAR));
	sLot += ",";
	cfile.Write(sLot, sLot.GetLength() * sizeof(TCHAR));
	sLayer += ",";
	cfile.Write(sLayer, sLayer.GetLength() * sizeof(TCHAR));
	sLayerUp += ",";
	cfile.Write(sLayerUp, sLayerUp.GetLength() * sizeof(TCHAR));
	sLayerDn += ",";
	cfile.Write(sLayerDn, sLayerDn.GetLength() * sizeof(TCHAR));
	sItsCode += ",";
	cfile.Write(sItsCode, sItsCode.GetLength() * sizeof(TCHAR));
	sItsCode += "\r\n";
	cfile.Write(sProcessCode, sProcessCode.GetLength() * sizeof(TCHAR));

	cfile.Close();
	return TRUE;
}

BOOL CSimpleReelmap::SaveRmap()
{
	CFile cfile;
	if (!cfile.Open(m_sPathRmap, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary | CFile::shareDenyNone, NULL))
	{
		AfxMessageBox(_T("Fail to save Reelmap."));
		return FALSE;
	}

	double dEntireSpeed = m_stInfo.m_dEntireSpeed;
	COleDateTime timeLotStart = m_stInfo.m_timeLotStart;
	COleDateTime timeLotRun = m_stInfo.m_timeLotRun;
	COleDateTime timeLotEnd = m_stInfo.m_timeLotEnd;
	int nLastSerial = m_stInfo.m_nLastSerial;
	int nActionCode = m_stInfo.m_nActionCode;
	int nNodeY = m_stInfo.m_nMaxRow;
	int nNodeX = m_stInfo.m_nMaxCol;
	int nMaxStrip = m_stInfo.m_nMaxStrip;
	int nMaxDefCode = m_stInfo.m_nMaxDefCode;

	cfile.Write(&dEntireSpeed, sizeof(double));
	cfile.Write(&timeLotStart, sizeof(COleDateTime));
	cfile.Write(&timeLotRun, sizeof(COleDateTime));
	cfile.Write(&timeLotEnd, sizeof(COleDateTime));
	cfile.Write(&nLastSerial, sizeof(int));
	cfile.Write(&nActionCode, sizeof(int));
	cfile.Write(&nNodeY, sizeof(int));
	cfile.Write(&nNodeX, sizeof(int));
	cfile.Write(&nMaxStrip, sizeof(int));
	cfile.Write(&nMaxDefCode, sizeof(int));

	int i, k;
	int nCount = m_arPcr.GetSize();
	cfile.Write(&nCount, sizeof(int));
	CPcr Pcr;
	for (i = 0; i < nCount; i++)
	{
		Pcr = m_arPcr.GetAt(i);
		int nSerial = Pcr.GetSerial();
		COleDateTime time = Pcr.GetDateTime();
		int nTotDef = Pcr.GetTotalDef();
		cfile.Write(&nSerial, sizeof(int));
		cfile.Write(&time, sizeof(COleDateTime));
		cfile.Write(&nTotDef, sizeof(int));
		for (k = 0; k < nTotDef; k++)
		{
			int nCamId = Pcr.GetCamId(k);
			int nPcsId = Pcr.GetPcsId(k);
			int nPosX = Pcr.GetPosX(k);
			int nPosY = Pcr.GetPosY(k);
			int nDefCode = Pcr.GetDefCode(k);
			int nCell = Pcr.GetCell(k);
			int nImageSize = Pcr.GetImageSize(k);
			int nImageNum = Pcr.GetImageNum(k);
			int nMarkingCode = Pcr.GetMarkingCode(k);

			cfile.Write(&nCamId, sizeof(int));
			cfile.Write(&nPcsId, sizeof(int));
			cfile.Write(&nPosX, sizeof(int));
			cfile.Write(&nPosY, sizeof(int));
			cfile.Write(&nDefCode, sizeof(int));
			cfile.Write(&nCell, sizeof(int));
			cfile.Write(&nImageSize, sizeof(int));
			cfile.Write(&nImageNum, sizeof(int));
			cfile.Write(&nMarkingCode, sizeof(int));
		}
	}
	cfile.Close();

	return TRUE;
}

BOOL CSimpleReelmap::SaveYield()
{
	CFile cfile;
	if (!cfile.Open(m_sPathYield, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary | CFile::shareDenyNone, NULL))
	{
		AfxMessageBox(_T("Fail to save Yield."));
		return FALSE;
	}

	int i, j, k;
	int nCount = m_arPcrYield.GetSize();
	cfile.Write(&nCount, sizeof(int));
	CPcrYield PcrYield;
	for (i = 0; i < nCount; i++)
	{
		PcrYield = m_arPcrYield.GetAt(i);
		int nSerial = PcrYield.GetSerial();
		COleDateTime time = PcrYield.GetDateTime();
		int nTotPcs = PcrYield.GetTotalPcs();		// 누적된 PCS
		int nGood = PcrYield.GetTotalGood();
		int nBad = PcrYield.GetTotalBad();
		int nTotStripOut = PcrYield.GetTotalStripOut();
		int nStripOut[MAX_STRIP], nStripDef[MAX_STRIP];

		cfile.Write(&nSerial, sizeof(int));
		cfile.Write(&time, sizeof(COleDateTime));
		cfile.Write(&nTotPcs, sizeof(int));
		cfile.Write(&nGood, sizeof(int));
		cfile.Write(&nBad, sizeof(int));
		cfile.Write(&nTotStripOut, sizeof(int));

		for (j = 0; j < MAX_STRIP; j++)
		{
			nStripOut[j] = PcrYield.GetStripOut(j);
			nStripDef[j] = PcrYield.GetStripTotalBad(j);
			cfile.Write(&nStripOut[j], sizeof(int));
			cfile.Write(&nStripDef[j], sizeof(int));
		}
		int nDefPerStrip[MAX_STRIP][MAX_DEF];
		int nDef[MAX_DEF];
		for (k = 0; k < MAX_DEF; k++)
		{
			nDef[k] = PcrYield.GetDefNum(k);
			cfile.Write(&nDef[k], sizeof(int));
		}
		for (j = 0; j < MAX_STRIP; j++)
		{
			for (k = 0; k < MAX_DEF; k++)
			{
				nDefPerStrip[j][k] = PcrYield.GetStripDefNum(j, k);
				cfile.Write(&nDefPerStrip[j][k], sizeof(int));
			}
		}
	}
	cfile.Close();

	return TRUE;
}

BOOL CSimpleReelmap::SaveMark()
{
	CFile cfile;
	if (!cfile.Open(m_sPathMark, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary | CFile::shareDenyNone, NULL))
	{
		AfxMessageBox(_T("Fail to save Mark."));
		return FALSE;
	}

	int i, j, k;
	int nCount = m_arPcrMark.GetSize();
	cfile.Write(&nCount, sizeof(int));
	CPcrMark PcrMark;
	for (i = 0; i < nCount; i++)
	{
		PcrMark = m_arPcrMark.GetAt(i);
		int nSerial = PcrMark.GetSerial();
		COleDateTime time = PcrMark.GetDateTime();
		int nTotDef = PcrMark.GetTotalDef();		
		int nTotMark = PcrMark.GetTotalMark();

		cfile.Write(&nSerial, sizeof(int));
		cfile.Write(&time, sizeof(COleDateTime));
		cfile.Write(&nTotDef, sizeof(int));
		cfile.Write(&nTotMark, sizeof(int));
		
		for (j = 0; j < nTotMark; j++)
		{
			int nMarkedPcsId = PcrMark.GetMarkedPcsId(j);
			cfile.Write(&nMarkedPcsId, sizeof(int));
		}
	}
	cfile.Close();

	return TRUE;
}

BOOL CSimpleReelmap::Load()
{
	BOOL bRtn[4] = { 0, 0, 0, 0 };
	bRtn[0] = LoadInfo();
	bRtn[1] = LoadRmap();
	bRtn[2] = LoadYield();
	bRtn[3] = LoadMark();

	return (bRtn[0] && bRtn[1] && bRtn[2] && bRtn[3]);
}

BOOL CSimpleReelmap::LoadInfo()
{
	int i, k;
	int nCount = m_arPcr.GetSize();
	if (nCount > 0)
	{
		CPcr Pcr;
		for (i = 0; i < nCount; i++)
		{
			Pcr = m_arPcr.GetAt(i);
			Pcr.Free();
		}
		m_arPcr.RemoveAll();
	}

	CString sMcName;
	CString sUserName;
	CString sModel;
	CString sLot;
	CString sLayer;
	CString sLayerUp;
	CString sLayerDn;
	CString sItsCode;
	CString sProcessCode;

	CFile cfile;
	if (!cfile.Open(m_sPathInfo, CFile::modeRead | CFile::shareDenyNone, NULL))
	{
		AfxMessageBox(_T("Fail to load Info."));
		return FALSE;
	}
	BOOL bExist = TRUE;
	CString str, sData=_T("");
	CArchive ar(&cfile, CArchive::load);
	while (bExist)
	{
		bExist = ar.ReadString(str);
		if (bExist)
			sData += (str + '\n');
	}
	ar.Close();
	cfile.Close();

	int nPos;
	int nSize = sData.GetLength();
	// McName
	nPos = sData.Find(',', 0);
	sMcName = sData.Left(nPos);
	sData.Delete(0, nPos + 1);
	nSize = nSize - nPos - 1;
	// UserName
	nPos = sData.Find(',', 0);
	sUserName = sData.Left(nPos);
	sData.Delete(0, nPos + 1);
	nSize = nSize - nPos - 1;
	// Model
	nPos = sData.Find(',', 0);
	sModel = sData.Left(nPos);
	sData.Delete(0, nPos + 1);
	nSize = nSize - nPos - 1;
	// Lot
	nPos = sData.Find(',', 0);
	sLot = sData.Left(nPos);
	sData.Delete(0, nPos + 1);
	nSize = nSize - nPos - 1;
	// Layer
	nPos = sData.Find(',', 0);
	sLayer = sData.Left(nPos);
	sData.Delete(0, nPos + 1);
	nSize = nSize - nPos - 1;
	// LayerUp
	nPos = sData.Find(',', 0);
	sLayerUp = sData.Left(nPos);
	sData.Delete(0, nPos + 1);
	nSize = nSize - nPos - 1;
	// LayerDn
	nPos = sData.Find(',', 0);
	sLayerDn = sData.Left(nPos);
	sData.Delete(0, nPos + 1);
	nSize = nSize - nPos - 1;
	// ItsCode
	nPos = sData.Find(',', 0);
	sItsCode = sData.Left(nPos);
	sData.Delete(0, nPos + 1);
	nSize = nSize - nPos - 1;
	// ProcessCode
	nPos = sData.Find('\n', 0);
	sProcessCode = sData.Left(nPos);
	sData.Delete(0, nPos + 1);
	nSize = nSize - nPos - 1;

	m_stInfo.m_sMcName = sMcName;
	m_stInfo.m_sUserName = sUserName;
	m_stInfo.m_sModel = sModel;
	m_stInfo.m_sLot = sLot;
	m_stInfo.m_sLayer = sLayer;
	m_stInfo.m_sLayerUp = sLayerUp;
	m_stInfo.m_sLayerDn = sLayerDn;
	m_stInfo.m_sItsCode = sItsCode;
	m_stInfo.m_sProcessCode = sProcessCode;

	return TRUE;
}

BOOL CSimpleReelmap::LoadRmap()
{
	int i, k;
	int nCount = m_arPcr.GetSize();
	if (nCount > 0)
	{
		CPcr Pcr;
		for (i = 0; i < nCount; i++)
		{
			Pcr = m_arPcr.GetAt(i);
			Pcr.Free();
		}
		m_arPcr.RemoveAll();
	}

	CFile cfile;
	if (!cfile.Open(m_sPathRmap, CFile::modeRead | CFile::shareDenyNone, NULL))
	{
		AfxMessageBox(_T("Fail to load Rmap."));
		return FALSE;
	}

	double dEntireSpeed;
	COleDateTime timeLotStart;
	COleDateTime timeLotRun;
	COleDateTime timeLotEnd;
	int nLastSerial;
	int nActionCode;
	int nNodeY;
	int nNodeX;
	int nMaxStrip;
	int nMaxDefCode;

	cfile.Read((void *)&dEntireSpeed, sizeof(double));
	cfile.Read((void *)&timeLotStart, sizeof(COleDateTime));
	cfile.Read((void *)&timeLotRun, sizeof(COleDateTime));
	cfile.Read((void *)&timeLotEnd, sizeof(COleDateTime));
	cfile.Read((void *)&nLastSerial, sizeof(int));
	cfile.Read((void *)&nActionCode, sizeof(int));
	cfile.Read((void *)&nNodeY, sizeof(int));
	cfile.Read((void *)&nNodeX, sizeof(int));
	cfile.Read((void *)&nMaxStrip, sizeof(int));
	cfile.Read((void *)&nMaxDefCode, sizeof(int));

	int nTotalPcr;
	cfile.Read((void *)&nTotalPcr, sizeof(int));

	m_stInfo.m_dEntireSpeed = dEntireSpeed;
	m_stInfo.m_timeLotStart = timeLotStart;
	m_stInfo.m_timeLotRun = timeLotRun;
	m_stInfo.m_timeLotEnd = timeLotEnd;
	m_stInfo.m_nLastSerial = nLastSerial;
	m_stInfo.m_nActionCode = nActionCode;
	m_stInfo.m_nMaxCol = nNodeX;
	m_stInfo.m_nMaxRow = nNodeY;
	m_stInfo.m_nMaxStrip = nMaxStrip;
	m_stInfo.m_nMaxDefCode = nMaxDefCode;

	for (i = 0; i < nTotalPcr; i++)
	{
		int nSerial;
		COleDateTime time;
		int nTotDef = 0;
		cfile.Read((void *)&nSerial, sizeof(int));
		cfile.Read((void *)&time, sizeof(COleDateTime));
		cfile.Read((void *)&nTotDef, sizeof(int));

		CPcr Pcr;
		Pcr.SetSerial(nSerial);
		Pcr.SetDateTime(time);
		Pcr.SetTotalDef(nTotDef); // allocate stDefPcs
		for (k = 0; k < nTotDef; k++)
		{
			int nCamId;
			int nPcsId;
			int nPosX;
			int nPosY;
			int nDefCode;
			int nCell;
			int nImageSize;
			int nImageNum;
			int nMarkingCode;
			cfile.Read((void *)&nCamId, sizeof(int));
			cfile.Read((void *)&nPcsId, sizeof(int));
			cfile.Read((void *)&nPosX, sizeof(int));
			cfile.Read((void *)&nPosY, sizeof(int));
			cfile.Read((void *)&nDefCode, sizeof(int));
			cfile.Read((void *)&nCell, sizeof(int));
			cfile.Read((void *)&nImageSize, sizeof(int));
			cfile.Read((void *)&nImageNum, sizeof(int));
			cfile.Read((void *)&nMarkingCode, sizeof(int));

			Pcr.SetCamId(k, nCamId);
			Pcr.SetPcsId(k, nPcsId);
			Pcr.SetPosX(k, nPosX);
			Pcr.SetPosY(k, nPosY);
			Pcr.SetDefCode(k, nDefCode);
			Pcr.SetCell(k, nCell);
			Pcr.SetImageSize(k, nImageSize);
			Pcr.SetImageNum(k, nImageNum);
			Pcr.SetMarkingCode(k, nMarkingCode);
		}
		m_arPcr.Add(Pcr);
		m_stInfo.m_nLastSerial = nSerial;
	}

	cfile.Close();
	return TRUE;
}

BOOL CSimpleReelmap::LoadYield()
{
	int i, j, k;
	int nCount = m_arPcrYield.GetSize();
	if (nCount > 0)
		m_arPcrYield.RemoveAll();

	CFile cfile;
	if (!cfile.Open(m_sPathYield, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone, NULL))
	{
		AfxMessageBox(_T("Fail to load Yield."));
		return FALSE;
	}

	int nTotalPcr = 0;
	cfile.Read((void *)&nTotalPcr, sizeof(int));

	for (i = 0; i < nTotalPcr; i++)
	{
		CPcrYield PcrYield;
		int nSerial, nTotPcs, nGood, nBad, nTotStripOut, nStripOut[MAX_STRIP], nStripDef[MAX_STRIP];		// 누적된 PCS
		COleDateTime time;

		cfile.Read((void *)&nSerial, sizeof(int));
		cfile.Read((void *)&time, sizeof(COleDateTime));
		cfile.Read((void *)&nTotPcs, sizeof(int));
		cfile.Read((void *)&nGood, sizeof(int));
		cfile.Read((void *)&nBad, sizeof(int));
		cfile.Read((void *)&nTotStripOut, sizeof(int));
		PcrYield.SetSerial(nSerial);
		PcrYield.SetDateTime(time);
		PcrYield.SetTotalPcs(nTotPcs);
		PcrYield.SetTotalGood(nGood);
		PcrYield.SetTotalBad(nBad);
		PcrYield.SetTotalStripOut(nTotStripOut);

		for (j = 0; j < MAX_STRIP; j++)
		{
			cfile.Read((void *)&nStripOut[j], sizeof(int));
			cfile.Read((void *)&nStripDef[j], sizeof(int));
			PcrYield.SetStripOut(j, nStripOut[j]);
			PcrYield.SetStripTotalBad(j, nStripDef[j]);
		}

		int nDefPerStrip[MAX_STRIP][MAX_DEF];
		int nDef[MAX_DEF];
		for (k = 0; k < MAX_DEF; k++)
		{
			cfile.Read((void *)&nDef[k], sizeof(int));
			PcrYield.SetDefNum(k, nDef[k]);
		}
		for (j = 0; j < MAX_STRIP; j++)
		{
			for (k = 0; k < MAX_DEF; k++)
			{
				cfile.Read((void *)&nDefPerStrip[j][k], sizeof(int));
				PcrYield.SetStripDefNum(j, k, nDefPerStrip[j][k]);
			}
		}

		m_arPcrYield.Add(PcrYield);
	}

	return TRUE;
}

BOOL CSimpleReelmap::LoadMark()
{
	CFile cfile;
	if (!cfile.Open(m_sPathMark, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone, NULL))
	{
		AfxMessageBox(_T("Fail to load Mark."));
		return FALSE;
	}

	int i, j, k;
	int nCount = m_arPcrMark.GetSize();
	if (nCount > 0)
	{
		CPcrMark PcrMark;
		for (i = 0; i < nCount; i++)
		{
			PcrMark = m_arPcrMark.GetAt(i);
			PcrMark.Free();
		}
		m_arPcrMark.RemoveAll();
	}

	int nTotalPcr = 0;
	cfile.Read((void *)&nTotalPcr, sizeof(int));

	for (i = 0; i < nTotalPcr; i++)
	{
		CPcrMark PcrMark;

		COleDateTime time;
		int nSerial, nTotDef, nTotMark;
		int *pMarkedPcsId;

		cfile.Read((void *)&nSerial, sizeof(int));
		cfile.Read((void *)&time, sizeof(COleDateTime));
		cfile.Read((void *)&nTotDef, sizeof(int));
		cfile.Read((void *)&nTotMark, sizeof(int));

		PcrMark.Init(nSerial, nTotDef, this);

		for (j = 0; j < nTotMark; j++)
		{
			int nMarkedPcsId;
			cfile.Read((void *)&nMarkedPcsId, sizeof(int));
			PcrMark.SetMarkedPcsId(nMarkedPcsId);
		}

		m_arPcrMark.Add(PcrMark);
	}

	return TRUE;
}

BOOL CSimpleReelmap::LoadSapp3CodeIni()
{
	CString sPath = PATH_SAPP3;
	TCHAR szData[MAX_PATH];

	if (0 < ::GetPrivateProfileString(_T("Sapp3Code"), _T("OPEN"), NULL, szData, sizeof(szData), sPath))
		m_nSapp3Code[Sapp3Open] = _ttoi(szData);
	else
		m_nSapp3Code[Sapp3Open] = 0;

	if (0 < ::GetPrivateProfileString(_T("Sapp3Code"), _T("SHORT"), NULL, szData, sizeof(szData), sPath))
		m_nSapp3Code[Sapp3Short] = _ttoi(szData);
	else
		m_nSapp3Code[Sapp3Short] = 0;

	if (0 < ::GetPrivateProfileString(_T("Sapp3Code"), _T("USHORT"), NULL, szData, sizeof(szData), sPath))
		m_nSapp3Code[Sapp3UShort] = _ttoi(szData);
	else
		m_nSapp3Code[Sapp3UShort] = 0;

	if (0 < ::GetPrivateProfileString(_T("Sapp3Code"), _T("NICK"), NULL, szData, sizeof(szData), sPath))
		m_nSapp3Code[Sapp3Nick] = _tstoi(szData);
	else
		m_nSapp3Code[Sapp3Nick] = 0;

	if (0 < ::GetPrivateProfileString(_T("Sapp3Code"), _T("SPACE_EXTRA_PROTRUSION"), NULL, szData, sizeof(szData), sPath))
		m_nSapp3Code[Sapp3SpaceExtraProt] = _ttoi(szData);
	else
		m_nSapp3Code[Sapp3SpaceExtraProt] = 0;

	if (0 < ::GetPrivateProfileString(_T("Sapp3Code"), _T("PINHOLE"), NULL, szData, sizeof(szData), sPath))
		m_nSapp3Code[Sapp3PinHole] = _ttoi(szData);
	else
		m_nSapp3Code[Sapp3PinHole] = 0;

	if (0 < ::GetPrivateProfileString(_T("Sapp3Code"), _T("PAD"), NULL, szData, sizeof(szData), sPath))
		m_nSapp3Code[Sapp3Pad] = _ttoi(szData);
	else
		m_nSapp3Code[Sapp3Pad] = 0;

	if (0 < ::GetPrivateProfileString(_T("Sapp3Code"), _T("HOPEN"), NULL, szData, sizeof(szData), sPath))
		m_nSapp3Code[Sapp3HOpen] = _ttoi(szData);
	else
		m_nSapp3Code[Sapp3HOpen] = 0;

	if (0 < ::GetPrivateProfileString(_T("Sapp3Code"), _T("HMISS_HPOS_HBAD"), NULL, szData, sizeof(szData), sPath))
		m_nSapp3Code[Sapp3HMissHPosHBad] = _tstoi(szData);
	else
		m_nSapp3Code[Sapp3HMissHPosHBad] = 0;

	if (0 < ::GetPrivateProfileString(_T("Sapp3Code"), _T("VHOPEN_NOVH_VHALIGN_VHDEF"), NULL, szData, sizeof(szData), sPath))
		m_nSapp3Code[Sapp3VHOpenVHAlignVHDefNoVH] = _tstoi(szData);
	else
		m_nSapp3Code[Sapp3VHOpenVHAlignVHDefNoVH] = 0;

	return TRUE;
}

BOOL CSimpleReelmap::LoadDefectTableIni()
{	
	TCHAR szData[MAX_PATH];
	TCHAR sep[] = { _T(",;\r\n\t") };
	CString sIdx, sVal;
	int k;

	for (k = 1; k < MAX_DEF; k++)
	{
		sIdx.Format(_T("%d"), k);
		if (0 < ::GetPrivateProfileString(_T("DEFECT"), sIdx, NULL, szData, sizeof(szData), PATH_CONFIG))
		{
			sVal = _tcstok(szData, sep);
			m_sEngDef[k].Format(_T("%s"), sVal);
			sVal = _tcstok(NULL, sep);
			m_sKorDef[k].Format(_T("%s"), sVal);
			sVal = _tcstok(NULL, sep);
			m_cBigDef[k] = sVal.GetAt(0);
			sVal = _tcstok(NULL, sep);
			m_cSmallDef[k] = sVal.GetAt(0);
			sVal = _tcstok(NULL, sep);
			m_rgbDef[k] = (COLORREF)_tstoi(sVal);
			sVal = _tcstok(NULL, sep);
			m_nOdr[k] = _tstoi(sVal);
		}
		else
		{
			AfxMessageBox(_T("Error - LoadDefectTableIni()"));
			return FALSE;
		}
	}

	if (0 < ::GetPrivateProfileString(_T("REELMAP"), _T("BackGround"), NULL, szData, sizeof(szData), PATH_CONFIG))
	{
		sVal = _tcstok(szData, sep);
		m_nBkColor[0] = _tstoi(sVal);
		sVal = _tcstok(NULL, sep);
		m_nBkColor[1] = _tstoi(sVal);
		sVal = _tcstok(NULL, sep);
		m_nBkColor[2] = _tstoi(sVal);
	}
	else
	{
		AfxMessageBox(_T("Error - LoadBackGroundColor."));
		return FALSE;
	}

	return TRUE;
}

BOOL CSimpleReelmap::GetMatrix(int nPcsId, int &nR, int &nC)
{
	int nNodeX = m_stInfo.m_nMaxCol;
	int nNodeY = m_stInfo.m_nMaxRow;

	switch (m_stInfo.m_nActionCode)	// 0 : Rotation / Mirror 적용 없음(CAM Data 원본), 1 : 좌우 미러, 2 : 상하 미러, 3 : 180 회전, 4 : 270 회전(CCW), 5 : 90 회전(CW)
	{
	case 0:
		break;
	case 1:
		nPcsId = MirrorLR(nPcsId);
		break;
	case 2:
		nPcsId = MirrorUD(nPcsId);
		break;
	case 3:
		nPcsId = Rotate180(nPcsId);
		break;
	default:
		break;
	}

	if (-1 < nPcsId && nPcsId < (nNodeX*nNodeY))
	{
		nC = int(nPcsId / nNodeY);
		if (nC % 2)	// 홀수.
			nR = nNodeY*(nC + 1) - 1 - nPcsId;
		else		// 짝수.
			nR = nPcsId - nC*nNodeY;
	}
	else
	{
		nC = -1;
		nR = -1;
		return FALSE;
	}

	return TRUE;
}

int CSimpleReelmap::MirrorLR(int nPcsId) // 좌우 미러링
{
	int nId, nCol, nRow, nC, nR;
	int nNodeX = m_stInfo.m_nMaxCol; // 1 ~
	int nNodeY = m_stInfo.m_nMaxRow; // 1 ~

	nCol = int(nPcsId / nNodeY); // 0 ~
	if (nCol % 2)
		nRow = nNodeY*(nCol + 1) - nPcsId - 1;
	else
		nRow = nPcsId - nNodeY*nCol; // 0 ~

	nR = nRow; // 0 ~
	nC = (nNodeX - 1) - nCol; // 0 ~
	if (nC % 2) // 홀수 : 시작 감소
		nId = nNodeY*(nC + 1) - nR - 1;
	else		// 짝수 : 시작 증가
		nId = nR + nNodeY*nC;

	return nId;
}

int CSimpleReelmap::MirrorUD(int nPcsId) // 상하 미러링
{
	int nId, nCol, nRow, nC, nR;
	int nNodeX = m_stInfo.m_nMaxCol; // 1 ~
	int nNodeY = m_stInfo.m_nMaxRow; // 1 ~

	nCol = int(nPcsId / nNodeY); // 0 ~
	if (nCol % 2)
		nRow = nPcsId - nNodeY*nCol; // 0 ~
	else
		nRow = nNodeY*(nCol + 1) - nPcsId - 1;

	nR = nRow; // 0 ~
	nC = nCol; // 0 ~
	if (nC % 2) // 홀수 : 시작 감소
		nId = nNodeY*(nC + 1) - nR - 1;
	else		// 짝수 : 시작 증가
		nId = nR + nNodeY*nC;

	return nId;
}

int CSimpleReelmap::Rotate180(int nPcsId) // 180도 회전
{
	int nId, nCol, nRow, nC, nR;
	int nNodeX = m_stInfo.m_nMaxCol; // 1 ~
	int nNodeY = m_stInfo.m_nMaxRow; // 1 ~

	if (nNodeX % 2)		// 홀수 : 시작 감소
	{
		nCol = (nNodeX - 1) - int(nPcsId / nNodeY); // 0 ~
		if (nCol % 2)
			nRow = nPcsId - nNodeY * (nNodeX - nCol - 1); // 0 ~
		else
			nRow = nNodeY * (nNodeX - nCol) - nPcsId - 1; // 0 ~

		nR = nRow;
		nC = nCol;

		if (nC % 2)
			nId = nNodeY*(nC + 1) - nR - 1; // 0 ~
		else
			nId = nR + nNodeY*nC;
	}
	else				// 짝수 : 시작 증가
	{
		nCol = (nNodeX - 1) - int(nPcsId / nNodeY); // 0 ~
		if (nCol % 2)
			nRow = nNodeY * (nNodeX - nCol) - nPcsId - 1; // 0 ~
		else
			nRow = nPcsId - nNodeY * (nNodeX - nCol - 1); // 0 ~

		nR = nRow;
		nC = nCol;

		if (nC % 2)
			nId = nNodeY*(nC + 1) - nR - 1; // 0 ~
		else
			nId = nR + nNodeY*nC;
	}
	return nId;
}

void CSimpleReelmap::StringToChar(CString str, char* pCh) // char* returned must be deleted... 
{
	wchar_t*	wszStr;
	int			nLenth;

	USES_CONVERSION;
	//1. CString to wchar_t* conversion
	wszStr = T2W(str.GetBuffer(str.GetLength()));

	//2. wchar_t* to char* conversion
	nLenth = WideCharToMultiByte(CP_ACP, 0, wszStr, -1, NULL, 0, NULL, NULL); //char* 형에 대한길이를 구함 

																			  //3. wchar_t* to char* conversion
	WideCharToMultiByte(CP_ACP, 0, wszStr, -1, pCh, nLenth, 0, 0);
	return;
}

CString CSimpleReelmap::CharToString(const char *szStr)
{
	CString strRet;

	int nLen = strlen(szStr) + sizeof(char);
	wchar_t *tszTemp = NULL;
	tszTemp = new WCHAR[nLen];

	MultiByteToWideChar(CP_ACP, 0, szStr, -1, tszTemp, nLen * sizeof(WCHAR));

	strRet.Format(_T("%s"), (CString)tszTemp);
	if (tszTemp)
	{
		delete[] tszTemp;
		tszTemp = NULL;
	}
	return strRet;
}

CArPcr& CSimpleReelmap::GetAddrArPcr()
{
	return m_arPcr;
}

CArPcrMark& CSimpleReelmap::GetAddrArPcrMark()
{
	return m_arPcrMark;
}

COLORREF CSimpleReelmap::GetDefColor(int nDefCode)
{
	return m_rgbDef[nDefCode];
}

char CSimpleReelmap::GetCodeBigDef(int nIdx)
{
	return m_cBigDef[nIdx];
}

char CSimpleReelmap::GetCodeSmallDef(int nIdx)
{
	return m_cSmallDef[nIdx];
}

void CSimpleReelmap::SetDispPnl(int nSerial)
{
	if (m_nDispPnl[1] != nSerial)		// Right Marking
	{
		m_nMkedPcs[0] = 0;				// Left Marking
		m_nMkedPcs[1] = 0;				// Right Marking
	}

	if (nSerial > 0 && m_nDispPnl[1] > 0)
		m_nDispPnl[0] = m_nDispPnl[1];	// Left Marking
	m_nDispPnl[1] = nSerial;			// Right Marking
}

void CSimpleReelmap::SetPcsMkOut(int nCam) // 0: Left Cam Or 1: Right Cam , 불량 피스 인덱스 [ 0 ~ (Total Pcs - 1) ]  // (피스인덱스는 CamMaster에서 정한 것을 기준으로 함.)
{
	int nPcsIdx = m_nMkedPcs[nCam];
	CPcrMark PcrMark;
	int nCount = m_arPcrMark.GetCount();
	if (nCount < 1) return;

	int nSerial, nCurSerial;
	for (int i = 0; i < nCount; i++)
	{
		PcrMark = m_arPcrMark.GetAt(i);
		nSerial = PcrMark.GetSerial();
		if (nCam == 0)	// Left
		{
			nCurSerial = m_nDispPnl[0];	// Left
			if (!nCurSerial)
				nCurSerial = m_nDispPnl[1] - 1;

			if (nSerial == nCurSerial)
			{
				int nPcrIdx = GetPcrIdx(0);
				if (nPcrIdx > -1)
				{
					CPcr Pcr = m_arPcr.GetAt(nPcrIdx);
					int nMarkedPcsId = Pcr.GetPcsId(nPcsIdx);
					PcrMark.SetMarkedPcsId(nMarkedPcsId);
					m_arPcrMark.SetAt(i, PcrMark);
					m_nMkedPcs[nCam]++;
				}
			}
		}
		else if (nCam == 1)	// Right
		{
			nCurSerial = m_nDispPnl[1];	// Right
			if (nSerial == nCurSerial)
			{
				int nPcrIdx = GetPcrIdx(1);
				if (nPcrIdx > -1)
				{
					CPcr Pcr = m_arPcr.GetAt(nPcrIdx);
					int nMarkedPcsId = Pcr.GetPcsId(nPcsIdx);
					PcrMark.SetMarkedPcsId(nMarkedPcsId);
					m_arPcrMark.SetAt(i, PcrMark);
					m_nMkedPcs[nCam]++;
				}
			}
		}
	}
}

int CSimpleReelmap::GetPcrIdx(int nCam)
{
	int nCurrPcr, nPcrIdx = -1;

	int nCount = m_arPcr.GetSize();

	if (nCam == 0)	// Left
	{
		nCurrPcr = m_nDispPnl[0];	// Left
	}
	else if (nCam == 1)	// Right
	{
		nCurrPcr = m_nDispPnl[1];	// Right
	}

	for (int i = 0; i < nCount; i++)
	{
		CPcr Pcr = m_arPcr.GetAt(i);
		if (Pcr.GetSerial() == nCurrPcr)
		{
			nPcrIdx = i;
			break;
		}
	}

	return nPcrIdx;
}

CString CSimpleReelmap::GetTimeIts()
{
	CString sData;
	COleDateTime oleDate = COleDateTime::GetCurrentTime();
	sData.Format(_T("%s"), oleDate.Format(_T("%Y%m%d%H%M%S")));
	return sData;
}

CString CSimpleReelmap::GetPathSapp3()
{
	CString sPath = _T("");
	CString str;
	CString sPathFolder = PATH_SAPP3_FOLDER;
	sPath.Format(_T("%s\\%9s_%4s_%5s.txt"), sPathFolder, m_stInfo.m_sLot, m_stInfo.m_sProcessCode, m_stInfo.m_sMcName);
	return sPath;
}

CString CSimpleReelmap::GetPathIts(int nSerial)
{
	CString sPath = _T("");
	CString str;
	CString sPathFolder = PATH_ITS_FOLDER;
	CString sTime = GetTimeIts();

	switch (m_nLayer)
	{
	case RMapUp: // 외층 Top
		str.Format(_T("%s_L2_%04d_T_%s_%s_AVR01_%s.dat"), m_stInfo.m_sItsCode, nSerial, m_stInfo.m_sUserName, m_stInfo.m_sMcName, sTime);
		sPath.Format(_T("%s\\%s"), sPathFolder, str);
		break;
	case RMapDn: // 외층 Bottom
		str.Format(_T("%s_L2_%04d_B_%s_%s_AVR01_%s.dat"), m_stInfo.m_sItsCode, nSerial, m_stInfo.m_sUserName, m_stInfo.m_sMcName, sTime);
		sPath.Format(_T("%s\\%s"), sPathFolder, str);
		break;
	case RMapUpInner: // 내층 Top
		str.Format(_T("%s_L1_%04d_T_%s_%s_AVR01_%s.dat"), m_stInfo.m_sItsCode, nSerial, m_stInfo.m_sUserName, m_stInfo.m_sMcName, sTime);
		sPath.Format(_T("%s\\%s"), sPathFolder, str);
		break;
	case RMapDnInner: // 내층 Bottom
		str.Format(_T("%s_L1_%04d_B_%s_%s_AVR01_%s.dat"), m_stInfo.m_sItsCode, nSerial, m_stInfo.m_sUserName, m_stInfo.m_sMcName, sTime);
		sPath.Format(_T("%s\\%s"), sPathFolder, str);
		break;
	case RMapUpOutter: // 외층 Top
		str.Format(_T("%s_L2_%04d_T_%s_%s_AVR01_%s.dat"), m_stInfo.m_sItsCode, nSerial, m_stInfo.m_sUserName, m_stInfo.m_sMcName, sTime);
		sPath.Format(_T("%s\\%s"), sPathFolder, str);
		break;
	case RMapDnOutter: // 외층 Bottom
		str.Format(_T("%s_L2_%04d_B_%s_%s_AVR01_%s.dat"), m_stInfo.m_sItsCode, nSerial, m_stInfo.m_sUserName, m_stInfo.m_sMcName, sTime);
		sPath.Format(_T("%s\\%s"), sPathFolder, str);
		break;
	}

	return sPath;
}

BOOL CSimpleReelmap::MakeIts(int nSerial)
{
	CString sPath = GetPathIts(nSerial);
	CFileFind cFile;
	if (cFile.FindFile(sPath))
		DeleteFile(sPath);
	CString sData = GetTextIts(nSerial);
	CFile cfile;
	if (!cfile.Open(sPath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone, NULL))
	{
		AfxMessageBox(_T("Fail to make ITS."));
		return FALSE;
	}
	cfile.Write(sData, sData.GetLength() * sizeof(TCHAR));
	cfile.Close();
	return TRUE;
}

CString CSimpleReelmap::GetTextIts(int nSerial)
{
	CString sData = _T("");

	CString sItsCode = m_stInfo.m_sItsCode;
	int nNodeX = m_stInfo.m_nMaxCol;
	int nNodeY = m_stInfo.m_nMaxRow;
	int nStripY = nNodeY / MAX_STRIP; // Strip(1~4);

	CPcr Pcr;
	int nCount = m_arPcr.GetCount();
	if (nCount < 1) return sData;

	BOOL bExist = FALSE;
	int i, nCurSerial;
	for (i = 0; i < nCount; i++)
	{
		Pcr = m_arPcr.GetAt(i);
		nCurSerial = Pcr.GetSerial();

		if (nSerial == nCurSerial)
		{
			bExist = TRUE;
			break;
		}
	}
	if(!bExist) return sData;

	CString str = _T(""), sSide = _T(""), sTemp = _T("");
	int nTotStrip[4] = { 0 };
	int nTotVerifyed = 0;

	CString sStripA = _T("");
	CString sStripB = _T("");
	CString sStripC = _T("");
	CString sStripD = _T("");

	switch (m_nLayer)
	{
	case RMapUp:
		sSide = _T("T");
		break;
	case RMapDn:
		sSide = _T("B");
		break;
	case RMapUpInner:
		sSide = _T("T");
		break;
	case RMapDnInner:
		sSide = _T("B");
		break;
	}

	int nR, nC;
	int nPcsId, nMkCode, nDefCode;
	int nTotDef = Pcr.GetTotalDef();
	for (i = 0; i < nTotDef; i++)
	{
		nPcsId = Pcr.GetPcsId(i);
		if (GetMatrix(nPcsId, nR, nC))
		{
			nMkCode = Pcr.GetMarkingCode(i);
			nDefCode = Pcr.GetDefCode(i);
			if (nDefCode > 0 && nMkCode != -2)
			{
				if (nR < nStripY)			// Strip A
				{
					nTotStrip[0]++;
					str.Format(_T("%s,%04d,%s,A,%d,%d,B%d\n"), sItsCode, nSerial, sSide, nC + 1, nR + 1, GetItsDefCode(nDefCode));
					sStripA += str;
				}
				else if (nR < 2 * nStripY)	// Strip B
				{
					nTotStrip[1]++;
					str.Format(_T("%s,%04d,%s,B,%d,%d,B%d\n"), sItsCode, nSerial, sSide, nC + 1, nR + 1, GetItsDefCode(nDefCode));
					sStripB += str;
				}
				else if (nR < 3 * nStripY)	// Strip C
				{
					nTotStrip[2]++;
					str.Format(_T("%s,%04d,%s,C,%d,%d,B%d\n"), sItsCode, nSerial, sSide, nC + 1, nR + 1, GetItsDefCode(nDefCode));
					sStripC += str;
				}
				else if (nR < 4 * nStripY)	// Strip D
				{
					nTotStrip[3]++;
					str.Format(_T("%s,%04d,%s,D,%d,%d,B%d\n"), sItsCode, nSerial, sSide, nC + 1, nR + 1, GetItsDefCode(nDefCode));
					sStripD += str;
				}
				else
					return sData;
			}
			else if (nDefCode > 0 && nMkCode == -2)
			{
				nTotVerifyed++;
			}
			else
				return sData;
		}
	}	

	// Strip A
	str.Format(_T("%d,%s,%04d\n"), nTotDef - nTotVerifyed, sItsCode, nSerial);
	sData = str;
	str.Format(_T("%d,%s,%04d,%s,A\n"), nTotStrip[0], sItsCode, nSerial, sSide);
	sData += str;
	sData += sStripA;
	str.Format(_T("%s,%04d,%s,A,EOS\n"), sItsCode, nSerial, sSide);
	sData += str;

	// Strip B
	str.Format(_T("%d,%s,%04d,%s,B\n"), nTotStrip[1], sItsCode, nSerial, sSide);
	sData += str;
	sData += sStripB;
	str.Format(_T("%s,%04d,%s,B,EOS\n"), sItsCode, nSerial, sSide);
	sData += str;

	// Strip C
	str.Format(_T("%d,%s,%04d,%s,C\n"), nTotStrip[2], sItsCode, nSerial, sSide);
	sData += str;
	sData += sStripC;
	str.Format(_T("%s,%04d,%s,C,EOS\n"), sItsCode, nSerial, sSide);
	sData += str;

	// Strip D
	str.Format(_T("%d,%s,%04d,%s,D\n"), nTotStrip[3], sItsCode, nSerial, sSide);
	sData += str;
	sData += sStripD;
	str.Format(_T("%s,%04d,%s,D,EOS\n"), sItsCode, nSerial, sSide);
	sData += str;
	str.Format(_T("%s,%04d,%s,EOP\n"), sItsCode, nSerial, sSide);
	sData += str;

	return sData;
}

int CSimpleReelmap::GetItsDefCode(int nDefCode)
{
	CString sDefCode;
	sDefCode.Format(_T("%c"), m_cBigDef[nDefCode]);

	// [Sapp3Code]
	if (sDefCode == _T("N"))		//1 NICK = 137 -> m_nSapp3Code[SAPP3_NICK]
		return m_nSapp3Code[Sapp3Nick];
	else if (sDefCode == _T("D"))	//2 SPACE_EXTRA_PROTRUSION = 160 -> m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] : PROTRUSION
		return m_nSapp3Code[Sapp3SpaceExtraProt];
	else if (sDefCode == _T("A"))	//3 SPACE_EXTRA_PROTRUSION = 160 -> m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] : SPACE 
		return m_nSapp3Code[Sapp3SpaceExtraProt];
	else if (sDefCode == _T("O"))	//4 OPEN = 102 -> m_nSapp3Code[SAPP3_OPEN]
		return m_nSapp3Code[Sapp3Open];
	else if (sDefCode == _T("S"))	//5 SHORT = 129 -> m_nSapp3Code[SAPP3_SHORT]
		return m_nSapp3Code[Sapp3Short];
	else if (sDefCode == _T("U"))	//6 USHORT = 129 -> m_nSapp3Code[SAPP3_USHORT]
		return m_nSapp3Code[Sapp3UShort];
	else if (sDefCode == _T("I"))	//7 PINHOLE = 134 -> m_nSapp3Code[SAPP3_PINHOLE]
		return m_nSapp3Code[Sapp3PinHole];
	else if (sDefCode == _T("H"))	//8 HMISS_HPOS_HBAD = 309 -> m_nSapp3Code[	SAPP3_HMISS_HPOS_HBAD] : No Hole
		return m_nSapp3Code[Sapp3HMissHPosHBad];
	else if (sDefCode == _T("E"))	//9 SPACE_EXTRA_PROTRUSION = 160 -> m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION] : EXTRA
		return m_nSapp3Code[Sapp3SpaceExtraProt];
	else if (sDefCode == _T("P"))	//10 PAD = 316 -> m_nSapp3Code[SAPP3_PAD]
		return m_nSapp3Code[Sapp3Pad];
	else if (sDefCode == _T("L"))	//11 HMISS_HPOS_HBAD = 309 -> m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] : Hole Align
		return m_nSapp3Code[Sapp3HMissHPosHBad];
	else if (sDefCode == _T("X"))	//12 POI -> m_nSapp3Code[SAPP3_SHORT]
		return m_nSapp3Code[Sapp3Short];
	else if (sDefCode == _T("T"))	//13 VH_POSITION = 379 -> m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] : VH Align
		return m_nSapp3Code[Sapp3VHOpenVHAlignVHDefNoVH];
	else if (sDefCode == _T("M"))	//14 VH_MISS = 379 -> m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] : No VH
		return m_nSapp3Code[Sapp3VHOpenVHAlignVHDefNoVH];
	else if (sDefCode == _T("F"))	//15 HMISS_HPOS_HBAD = 309 -> m_nSapp3Code[SAPP3_HMISS_HPOS_HBAD] : Hole Defect
		return m_nSapp3Code[Sapp3HMissHPosHBad];
	else if (sDefCode == _T("C"))	//16 HOPEN = 308 -> m_nSapp3Code[SAPP3_HOPEN]
		return m_nSapp3Code[Sapp3HOpen];
	else if (sDefCode == _T("G"))	//17 VH_OPEN = 379 -> m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] : VH Open
		return m_nSapp3Code[Sapp3VHOpenVHAlignVHDefNoVH];
	else if (sDefCode == _T("V"))	//18 VH_DEF = 379 -> m_nSapp3Code[SAPP3_VHOPEN_NOVH_VHALIGN_VHDEF] : VH Def
		return m_nSapp3Code[Sapp3VHOpenVHAlignVHDefNoVH];
	else if (sDefCode == _T("K"))	//19 E.Nick = 137 -> m_nSapp3Code[SAPP3_NICK]
		return m_nSapp3Code[Sapp3Nick];
	else if (sDefCode == _T("R"))	//20 E.Prot = 160 -> m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION]
		return m_nSapp3Code[Sapp3SpaceExtraProt];
	else if (sDefCode == _T("B"))	//21 E.Space = 160 -> m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION]
		return m_nSapp3Code[Sapp3SpaceExtraProt];
	else if (sDefCode == _T("J"))	//22 UDD1 = 160 -> m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION]
		return m_nSapp3Code[Sapp3SpaceExtraProt];
	else if (sDefCode == _T("Q"))	//23 Narrow = 160 -> m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION]
		return m_nSapp3Code[Sapp3SpaceExtraProt];
	else if (sDefCode == _T("W"))	//24 Wide = 160 -> m_nSapp3Code[SAPP3_SPACE_EXTRA_PROTRUSION]
		return m_nSapp3Code[Sapp3SpaceExtraProt];
	else if (sDefCode == _T("F"))	//25 FixedDefect = 0
		return 0;
	else if (sDefCode == _T("Y"))	//26 VH.Size = 379
		return m_nSapp3Code[Sapp3VHOpenVHAlignVHDefNoVH];
	else if (sDefCode == _T("Z"))	//27 VHEdge = 379
		return m_nSapp3Code[Sapp3VHOpenVHAlignVHDefNoVH];
	else if (sDefCode == _T("?"))	//28 Light = 0
		return 0;

	return 0;
}

CString CSimpleReelmap::GetTextListSapp3File(int nIdx) // Default : All
{
	CString sData = _T("");
	CString sPathFolder = PATH_SAPP3_FOLDER;
	CString sPath = sPathFolder + _T("\\*.txt");

	CFileFind cFile;
	BOOL bExist = cFile.FindFile(sPath);
	if (!bExist) return sData; // pcr파일이 존재하지 않음.

	int nPos, nLine = 0;
	CString sFileName, sItsFile, sTemp;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// 파일명을 얻음.
			sFileName = cFile.GetFileName();
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
			{
				sItsFile = sFileName.Left(nPos);
				sTemp.Format(_T("%s\r\n"), sItsFile);
				sData += sTemp;

				if (nLine == nIdx)
					return sItsFile;
				nLine++;
			}
		}
	}

	return sData;
}

CString CSimpleReelmap::GetTextListItsFile(int nIdx)
{
	CString sData = _T("");
	CString sPathFolder = PATH_ITS_FOLDER;
	CString sPath = sPathFolder + _T("\\*.dat");

	CFileFind cFile;
	BOOL bExist = cFile.FindFile(sPath);
	if (!bExist) return sData; // pcr파일이 존재하지 않음.

	int nPos, nLine = 0;
	CString sFileName, sItsFile, sTemp;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// 파일명을 얻음.
			sFileName = cFile.GetFileName();
			nPos = sFileName.ReverseFind('.');
			if (nPos > 0)
			{
				sItsFile = sFileName.Left(nPos);
				sTemp.Format(_T("%s\r\n"), sItsFile);
				sData += sTemp;

				if (nLine == nIdx)
					return sItsFile;
				nLine++;
			}
		}
	}

	return sData;
}

CString CSimpleReelmap::GetTextSapp3File(int nIdx)
{
	CString sLine, sData = _T("");
	if (nIdx < 0) return sData;

	CString sPathFolder = PATH_SAPP3_FOLDER;
	CString sPath;
	sPath.Format(_T("%s\\%s.txt"), sPathFolder, GetTextListSapp3File(nIdx));

	CFile cfile;
	if (!cfile.Open(sPath, CFile::modeRead | CFile::shareDenyNone, NULL))
	{
		AfxMessageBox(_T("Fail to load Info."));
		return FALSE;
	}
	BOOL bExist = TRUE;
	CString str;
	CArchive ar(&cfile, CArchive::load);
	while (bExist)
	{
		bExist = ar.ReadString(str);
		if (bExist)
		{
			sLine.Format(_T("%s\r\n"), str);
			sData += sLine;
		}
	}
	ar.Close();
	cfile.Close();

	return sData;
}

CString CSimpleReelmap::GetTextItsFile(int nIdx)
{
	CString sLine, sData = _T("");
	if (nIdx < 0) return sData;

	CString sPathFolder = PATH_ITS_FOLDER;
	CString sPath;
	sPath.Format(_T("%s\\%s.dat"), sPathFolder, GetTextListItsFile(nIdx));

	CFile cfile;
	if (!cfile.Open(sPath, CFile::modeRead | CFile::shareDenyNone, NULL))
	{
		AfxMessageBox(_T("Fail to load Info."));
		return FALSE;
	}
	BOOL bExist = TRUE;
	CString str;
	CArchive ar(&cfile, CArchive::load);
	while (bExist)
	{
		bExist = ar.ReadString(str);
		if (bExist)
		{
			sLine.Format(_T("%s\r\n"), str);
			sData += sLine;
		}
	}
	ar.Close();
	cfile.Close();

	return sData;
}

BOOL CSimpleReelmap::MakeSapp3()
{
	CString sPath = GetPathSapp3();
	CFileFind cFile;
	if (cFile.FindFile(sPath))
		DeleteFile(sPath);
	CString sData = GetTextSapp3();
	CFile cfile;
	if (!cfile.Open(sPath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone, NULL))
	{
		AfxMessageBox(_T("Fail to make SAPP3."));
		return FALSE;
	}
	cfile.Write(sData, sData.GetLength() * sizeof(TCHAR));
	cfile.Close();
	return TRUE;
}


CString CSimpleReelmap::GetTextSapp3()
{
	CString sTemp, sData = _T("");
	int j, k;
	int nTotPcr = m_arPcrYield.GetCount();
	if (nTotPcr < 1) return sData;
	CPcrYield PcrYield = m_arPcrYield.GetAt(nTotPcr - 1);

	int nTotPcs = PcrYield.GetTotalPcs();
	int nStripPcs = nTotPcs / MAX_STRIP;

	int nDefStrip[MAX_STRIP], nDefPerStrip[MAX_STRIP][MAX_DEF];
	for (j = 0; j < MAX_STRIP; j++)
	{
		nDefStrip[j] = PcrYield.GetStripTotalBad(j);
		for(k=0; k<MAX_DEF; k++)
			nDefPerStrip[j][k] = PcrYield.GetStripDefNum(j, k);
	}
	double dEntireSpeed = m_stInfo.m_dEntireSpeed;

	int nSum;
	double dRateBeforeVerify, dRateAfterVerify;

	// 파일 이름.
	CString sFile;
	sData.Format(_T("FileName : %9s_%4s_%5s.txt\r\n\r\n"), m_stInfo.m_sLot, m_stInfo.m_sProcessCode, m_stInfo.m_sMcName);

	// 열별 투입/완성/수율 Data.
	sData += _T("1Q\r\n");
	dRateBeforeVerify = 100.0 * (nStripPcs - nDefStrip[0]) / nStripPcs;
	dRateAfterVerify = 100.0 * (nStripPcs - nDefStrip[0]) / nStripPcs;
	sTemp.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nStripPcs - nDefStrip[0], dRateBeforeVerify, dRateAfterVerify); // 투입수량, 완성수량, Verify전 수량, Verify후 수량
	sData += sTemp;

	sData += _T("2Q\r\n");
	dRateBeforeVerify = 100.0 * (nStripPcs - nDefStrip[1]) / nStripPcs;
	dRateAfterVerify = 100.0 * (nStripPcs - nDefStrip[1]) / nStripPcs;
	sTemp.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nStripPcs - nDefStrip[1], dRateBeforeVerify, dRateAfterVerify); // 투입수량, 완성수량, Verify전 수량, Verify후 수량
	sData += sTemp;

	sData += _T("3Q\r\n");
	dRateBeforeVerify = 100.0 * (nStripPcs - nDefStrip[2]) / nStripPcs;
	dRateAfterVerify = 100.0 * (nStripPcs - nDefStrip[2]) / nStripPcs;
	sTemp.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nStripPcs - nDefStrip[2], dRateBeforeVerify, dRateAfterVerify); // 투입수량, 완성수량, Verify전 수량, Verify후 수량
	sData += sTemp;

	sData += _T("4Q\r\n");
	dRateBeforeVerify = 100.0 * (nStripPcs - nDefStrip[3]) / nStripPcs;
	dRateAfterVerify = 100.0 * (nStripPcs - nDefStrip[3]) / nStripPcs;
	sTemp.Format(_T("%d,%d,%.1f,%.1f\r\n"), nStripPcs, nStripPcs - nDefStrip[3], dRateBeforeVerify, dRateAfterVerify); // 투입수량, 완성수량, Verify전 수량, Verify후 수량
	sData += sTemp;
	sData += _T("\r\n");

	// 열별 불량 Data.
	sData += _T("1X\r\n");
	if (nDefPerStrip[0][DEF_OPEN] > 0 && m_nSapp3Code[Sapp3Open] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3Open], nDefPerStrip[0][DEF_OPEN]); // 오픈(B102)
		sData += sTemp;
	}

	nSum = nDefPerStrip[0][DEF_SHORT];// + nDefPerStrip[0][DEF_USHORT];
	if (nSum > 0 && m_nSapp3Code[Sapp3Short] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3Short], nSum); // 쇼트(B129) // +u쇼트
		sData += sTemp;
	}

	nSum = nDefPerStrip[0][DEF_USHORT];
	if (nSum > 0 && m_nSapp3Code[Sapp3UShort] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3UShort], nSum); // u쇼트(B314)
		sData += sTemp;
	}

	nSum = nDefPerStrip[0][DEF_NICK] + nDefPerStrip[0][DEF_EDGE_NICK];
	if (nSum > 0 && m_nSapp3Code[Sapp3Nick] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3Nick], nSum); // 결손+엣지결손(B137)
		sData += sTemp;
	}

	nSum = nDefPerStrip[0][DEF_SPACE] + nDefPerStrip[0][DEF_EXTRA] + nDefPerStrip[0][DEF_PROTRUSION] + nDefPerStrip[0][DEF_EDGE_SPACE] + nDefPerStrip[0][DEF_EDGE_PROT];
	if (nSum > 0 && m_nSapp3Code[Sapp3SpaceExtraProt] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3SpaceExtraProt], nSum); // 선간폭+잔동+돌기+엣지선간폭+엣지돌기(B160)
		sData += sTemp;
	}

	nSum = nDefPerStrip[0][DEF_PINHOLE];
	if (nSum > 0 && m_nSapp3Code[Sapp3PinHole] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3PinHole], nSum); // 핀홀(B134)
		sData += sTemp;
	}

	nSum = nDefPerStrip[0][DEF_PAD];
	if (nSum > 0 && m_nSapp3Code[Sapp3Pad] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3Pad], nSum); // 패드(B316)
		sData += sTemp;
	}

	if (nSum > 0 && m_nSapp3Code[Sapp3HOpen] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3HOpen], nDefPerStrip[0][DEF_HOLE_OPEN]);
		sData += sTemp;
	}
	nSum = nDefPerStrip[0][DEF_HOLE_MISS] + nDefPerStrip[0][DEF_HOLE_POSITION] + nDefPerStrip[0][DEF_HOLE_DEFECT];
	if (nSum > 0 && m_nSapp3Code[Sapp3HMissHPosHBad] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3HMissHPosHBad], nSum);
		sData += sTemp;
	}

	nSum = nDefPerStrip[0][DEF_VH_OPEN] + nDefPerStrip[0][DEF_VH_MISS] + nDefPerStrip[0][DEF_VH_POSITION] + nDefPerStrip[0][DEF_VH_DEF];
	if (nSum > 0 && m_nSapp3Code[Sapp3VHOpenVHAlignVHDefNoVH] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3VHOpenVHAlignVHDefNoVH], nSum);
		sData += sTemp;
	}


	sData += _T("2X\r\n");

	if (nDefPerStrip[1][DEF_OPEN] > 0 && m_nSapp3Code[Sapp3Open] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3Open], nDefPerStrip[1][DEF_OPEN]); // 오픈(B102)
		sData += sTemp;
	}

	nSum = nDefPerStrip[1][DEF_SHORT];// + nDefPerStrip[1][DEF_USHORT];
	if (nSum > 0 && m_nSapp3Code[Sapp3Short] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3Short], nSum); // 쇼트(B129) // +u쇼트
		sData += sTemp;
	}

	nSum = nDefPerStrip[1][DEF_USHORT];
	if (nSum > 0 && m_nSapp3Code[Sapp3UShort] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3UShort], nSum); // u쇼트(B314)
		sData += sTemp;
	}

	nSum = nDefPerStrip[1][DEF_NICK] + nDefPerStrip[1][DEF_EDGE_NICK];
	if (nSum > 0 && m_nSapp3Code[Sapp3Nick] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3Nick], nSum); // 결손+엣지결손(B137)
		sData += sTemp;
	}

	nSum = nDefPerStrip[1][DEF_SPACE] + nDefPerStrip[1][DEF_EXTRA] + nDefPerStrip[1][DEF_PROTRUSION] + nDefPerStrip[1][DEF_EDGE_SPACE] + nDefPerStrip[1][DEF_EDGE_PROT];
	if (nSum > 0 && m_nSapp3Code[Sapp3SpaceExtraProt] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3SpaceExtraProt], nSum); // 선간폭+잔동+돌기+엣지선간폭+엣지돌기(B160)
		sData += sTemp;
	}

	nSum = nDefPerStrip[1][DEF_PINHOLE];
	if (nSum > 0 && m_nSapp3Code[Sapp3PinHole] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3PinHole], nSum); // 핀홀(B134)
		sData += sTemp;
	}

	nSum = nDefPerStrip[1][DEF_PAD];
	if (nSum > 0 && m_nSapp3Code[Sapp3Pad] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3Pad], nSum); // 패드(B316)
		sData += sTemp;
	}

	if (nSum > 0 && m_nSapp3Code[Sapp3HOpen] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3HOpen], nDefPerStrip[1][DEF_HOLE_OPEN]);
		sData += sTemp;
	}

	nSum = nDefPerStrip[1][DEF_HOLE_MISS] + nDefPerStrip[1][DEF_HOLE_POSITION] + nDefPerStrip[1][DEF_HOLE_DEFECT];
	if (nSum > 0 && m_nSapp3Code[Sapp3HMissHPosHBad] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3HMissHPosHBad], nSum);
		sData += sTemp;
	}

	nSum = nDefPerStrip[1][DEF_VH_OPEN] + nDefPerStrip[1][DEF_VH_MISS] + nDefPerStrip[1][DEF_VH_POSITION] + nDefPerStrip[1][DEF_VH_DEF];
	if (nSum > 0 && m_nSapp3Code[Sapp3VHOpenVHAlignVHDefNoVH] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3VHOpenVHAlignVHDefNoVH], nSum);
		sData += sTemp;
	}


	sData += _T("3X\r\n");

	if (nDefPerStrip[2][DEF_OPEN] > 0 && m_nSapp3Code[Sapp3Open] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3Open], nDefPerStrip[2][DEF_OPEN]); // 오픈(B102)
		sData += sTemp;
	}

	nSum = nDefPerStrip[2][DEF_SHORT];// + nDefPerStrip[2][DEF_USHORT];
	if (nSum > 0 && m_nSapp3Code[Sapp3Short] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3Short], nSum); // 쇼트(B129) // +u쇼트
		sData += sTemp;
	}

	nSum = nDefPerStrip[2][DEF_USHORT];
	if (nSum > 0 && m_nSapp3Code[Sapp3UShort] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3UShort], nSum); // u쇼트(B314)
		sData += sTemp;
	}

	nSum = nDefPerStrip[2][DEF_NICK] + nDefPerStrip[2][DEF_EDGE_NICK];
	if (nSum > 0 && m_nSapp3Code[Sapp3Nick] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3Nick], nSum); // 결손+엣지결손(B137)
		sData += sTemp;
	}

	nSum = nDefPerStrip[2][DEF_SPACE] + nDefPerStrip[2][DEF_EXTRA] + nDefPerStrip[2][DEF_PROTRUSION] + nDefPerStrip[2][DEF_EDGE_SPACE] + nDefPerStrip[2][DEF_EDGE_PROT];
	if (nSum > 0 && m_nSapp3Code[Sapp3SpaceExtraProt] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3SpaceExtraProt], nSum); // 선간폭+잔동+돌기+엣지선간폭+엣지돌기(B160)
		sData += sTemp;
	}

	nSum = nDefPerStrip[2][DEF_PINHOLE];
	if (nSum > 0 && m_nSapp3Code[Sapp3PinHole] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3PinHole], nSum); // 핀홀(B134)
		sData += sTemp;
	}

	nSum = nDefPerStrip[2][DEF_PAD];
	if (nSum > 0 && m_nSapp3Code[Sapp3Pad] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3Pad], nSum); // 패드(B316)
		sData += sTemp;
	}

	if (nSum > 0 && m_nSapp3Code[Sapp3HOpen] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3HOpen], nDefPerStrip[2][DEF_HOLE_OPEN]);
		sData += sTemp;
	}

	nSum = nDefPerStrip[2][DEF_HOLE_MISS] + nDefPerStrip[2][DEF_HOLE_POSITION] + nDefPerStrip[2][DEF_HOLE_DEFECT];
	if (nSum > 0 && m_nSapp3Code[Sapp3HMissHPosHBad] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3HMissHPosHBad], nSum);
		sData += sTemp;
	}

	nSum = nDefPerStrip[2][DEF_VH_OPEN] + nDefPerStrip[2][DEF_VH_MISS] + nDefPerStrip[2][DEF_VH_POSITION] + nDefPerStrip[2][DEF_VH_DEF];
	if (nSum > 0 && m_nSapp3Code[Sapp3VHOpenVHAlignVHDefNoVH] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3VHOpenVHAlignVHDefNoVH], nSum);
		sData += sTemp;
	}


	sData += _T("4X\r\n");

	if (nDefPerStrip[3][DEF_OPEN] > 0 && m_nSapp3Code[Sapp3Open] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3Open], nDefPerStrip[3][DEF_OPEN]); // 오픈(B102)
		sData += sTemp;
	}

	nSum = nDefPerStrip[3][DEF_SHORT];// + nDefPerStrip[3][DEF_USHORT];
	if (nSum > 0 && m_nSapp3Code[Sapp3Short] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3Short], nSum); // 쇼트(B129) // +u쇼트
		sData += sTemp;
	}

	nSum = nDefPerStrip[3][DEF_USHORT];
	if (nSum > 0 && m_nSapp3Code[Sapp3UShort] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3UShort], nSum); // u쇼트(B314)
		sData += sTemp;
	}

	nSum = nDefPerStrip[3][DEF_NICK] + nDefPerStrip[3][DEF_EDGE_NICK];
	if (nSum > 0 && m_nSapp3Code[Sapp3Nick] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3Nick], nSum); // 결손+엣지결손(B137)
		sData += sTemp;
	}

	nSum = nDefPerStrip[3][DEF_SPACE] + nDefPerStrip[3][DEF_EXTRA] + nDefPerStrip[3][DEF_PROTRUSION] + nDefPerStrip[3][DEF_EDGE_SPACE] + nDefPerStrip[3][DEF_EDGE_PROT];
	if (nSum > 0 && m_nSapp3Code[Sapp3SpaceExtraProt] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3SpaceExtraProt], nSum); // 선간폭+잔동+돌기+엣지선간폭+엣지돌기(B160)
		sData += sTemp;
	}

	nSum = nDefPerStrip[3][DEF_PINHOLE];
	if (nSum > 0 && m_nSapp3Code[Sapp3PinHole] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3PinHole], nSum); // 핀홀(B134)
		sData += sTemp;
	}

	nSum = nDefPerStrip[3][DEF_PAD];
	if (nSum > 0 && m_nSapp3Code[Sapp3Pad] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3Pad], nSum); // 패드(B316)
		sData += sTemp;
	}

	if (nSum > 0 && m_nSapp3Code[Sapp3HOpen] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3HOpen], nDefPerStrip[3][DEF_HOLE_OPEN]);
		sData += sTemp;
	}

	nSum = nDefPerStrip[3][DEF_HOLE_MISS] + nDefPerStrip[3][DEF_HOLE_POSITION] + nDefPerStrip[3][DEF_HOLE_DEFECT];
	if (nSum > 0 && m_nSapp3Code[Sapp3HMissHPosHBad] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3HMissHPosHBad], nSum);
		sData += sTemp;
	}

	nSum = nDefPerStrip[3][DEF_VH_OPEN] + nDefPerStrip[3][DEF_VH_MISS] + nDefPerStrip[3][DEF_VH_POSITION] + nDefPerStrip[3][DEF_VH_DEF];
	if (nSum > 0 && m_nSapp3Code[Sapp3VHOpenVHAlignVHDefNoVH] > 0)
	{
		sTemp.Format(_T("B%d,%d\r\n"), m_nSapp3Code[Sapp3VHOpenVHAlignVHDefNoVH], nSum);
		sData += sTemp;
	}

	// 속도.
	sData += _T("\r\nS\r\n");
	sTemp.Format(_T("%.2f"), dEntireSpeed);
	sData += sTemp;
	sData += _T("\r\n");


	return sData;


	return sData;
}
