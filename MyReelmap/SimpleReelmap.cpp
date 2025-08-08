// SimpleReelmap.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MyReelmap.h"
#include "SimpleReelmap.h"

#include "MyReelmapDlg.h"

// CSimpleReelmap

IMPLEMENT_DYNAMIC(CSimpleReelmap, CWnd)

CSimpleReelmap::CSimpleReelmap(CString sPathRmap, CString sPathYield, CWnd* pParent/*=NULL*/)
{
	m_pParent = pParent;
	if (pParent)
		m_hParent = pParent->GetSafeHwnd();
	m_sPathRmap = sPathRmap;
	m_sPathYield = sPathYield;

	m_bLock = FALSE;
	m_nMaxRow = 0;
	m_nMaxCol = 0;
	m_nActionCode = 0;

	InitColor();

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

	LoadDefectTableIni();
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
	nCount = m_arPcr[0].GetSize();
	if (nCount > 0)
	{
		CPcr Pcr;
		for (i = 0; i < nCount; i++)
		{
			Pcr = m_arPcr[0].GetAt(i);
			Pcr.Free();
		}
		m_arPcr[0].RemoveAll();
	}

	nCount = m_arPcr[1].GetSize();
	if (nCount > 0)
	{
		CPcr Pcr;
		for (i = 0; i < nCount; i++)
		{
			Pcr = m_arPcr[1].GetAt(i);
			Pcr.Free();
		}
		m_arPcr[1].RemoveAll();
	}

	nCount = m_arPcrYield[0].GetSize();
	if (nCount > 0)
		m_arPcrYield[0].RemoveAll();

	nCount = m_arPcrYield[1].GetSize();
	if (nCount > 0)
		m_arPcrYield[1].RemoveAll();
}

BEGIN_MESSAGE_MAP(CSimpleReelmap, CWnd)
END_MESSAGE_MAP()



// CSimpleReelmap �޽��� ó�����Դϴ�.
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

void CSimpleReelmap::Init(int nMaxRow, int nMaxCol, int nActionCode)
{
	m_nMaxRow = nMaxRow;
	m_nMaxCol = nMaxCol;
	m_nActionCode = nActionCode;
}

BOOL CSimpleReelmap::IsShare(int &nSerial)
{
	CFileFind cFile;
	CString sPath;
	sPath.Format(_T("%s\\*.pcr"), PATH_SHARE);

	BOOL bExist = cFile.FindFile(sPath);
	if (!bExist)
		return FALSE; // pcr������ �������� ����.

	int nPos;
	CString sFileName, sSerial;
	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (!cFile.IsDirectory())
		{
			// ���ϸ��� ����.
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

void CSimpleReelmap::InitColor()
{
	m_rgbDef[DEF_NONE] = (RGB_WHITE);						// 0=�ҷ�����
	m_rgbDef[DEF_NICK] = (RGB_MAGENTA);					// 1=���
	m_rgbDef[DEF_PROTRUSION] = (RGB_SKYBLUE);				// 2=����
	m_rgbDef[DEF_SPACE] = (RGB_LTGREEN);					// 3=������
	m_rgbDef[DEF_OPEN] = (RGB_LTRED);						// 4=����
	m_rgbDef[DEF_SHORT] = (RGB_RED);						// 5=��Ʈ
	m_rgbDef[DEF_USHORT] = (RGB_LTCYAN);					// 6=u��Ʈ
	m_rgbDef[DEF_PINHOLE] = (RGB_LLTGREEN);				// 7=��Ȧ
	m_rgbDef[DEF_HOLE_MISS] = (RGB_LTBLUE);				// 8=Ȧ����
	m_rgbDef[DEF_EXTRA] = (RGB_CLOUDBLUE);				// 9=�ܵ�
	m_rgbDef[DEF_PAD] = (RGB_LTPURPLE);					// 10=�е�
	m_rgbDef[DEF_HOLE_POSITION] = (RGB_PINK);			// 11=Ȧ���
	m_rgbDef[DEF_POI] = (RGB_LTMAGENTA);					// 12=POI
	m_rgbDef[DEF_VH_POSITION] = (RGB_LTYELLOW);			// 13=VH���
	m_rgbDef[DEF_VH_MISS] = (RGB_BOON);					// 14=VH����
	m_rgbDef[DEF_HOLE_DEFECT] = (RGB_LTPINK);				// 15=Ȧ�ҷ�
	m_rgbDef[DEF_HOLE_OPEN] = (RGB_LTGREEN);				// 16=Ȧ����
	m_rgbDef[DEF_VH_OPEN] = (RGB_LT_DARKMAGENTA);			// 17=VH����
	m_rgbDef[DEF_VH_DEF] = (RGB_ORANGE);					// 18=VH����
	m_rgbDef[DEF_EDGE_NICK] = (RGB_MUSTARD);			// 19 = E.���
	m_rgbDef[DEF_EDGE_PROT] = (RGB_SKY);				// 20 = E.����
	m_rgbDef[DEF_EDGE_SPACE] = (RGB_LT_BROWN);			// 21 = E.������
	m_rgbDef[DEF_USER_DEFINE_1] = (RGB_PEACOCK_GREEN);	// 22 = UDD1
	m_rgbDef[DEF_NARROW] = (RGB_PURPLE);				// 23 = Narrow
	m_rgbDef[DEF_WIDE] = (RGB_FOREST_GREEN);			// 24 = Wide
	m_rgbDef[DEF_FIXED_DEF] = (RGB_RED);				// 25 = �����ҷ�
	m_rgbDef[DEF_VH_SIZE] = (RGB_PURPLE);				// 26 = VHũ��
	m_rgbDef[DEF_VH_EDGE] = (RGB_EVER_GREEN);			// 27 = VH��������
	m_rgbDef[DEF_LIGHT] = (RGB_YELLOW);					// 28 = �뱤�ҷ�
	m_rgbDef[DEF_INNER] = (RGB_PCS_OUT);				// 29 = �����ҷ�
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

	// ���ϸ��� ����.
	nPos = sDest.ReverseFind('\\');
	sFile = sDest.Right(sDest.GetLength() - nPos - 1);
	if (sFile.Find(_T("."), 0) != -1)
	{
		nPos = sFile.Find(_T("."), 0);
		sFile.Delete(nPos, sFile.GetLength() - nPos);
	}
	sFolder = ParseFolderName(sDest, sRemain);
	if (sFolder.IsEmpty()) return FALSE; // ������ �������� ����.
	sDestFolder = sRemain + _T("\\") + sFolder + _T("\\");

	sFolder = ParseFolderName(sSrc, sRemain);
	if (sFolder.IsEmpty()) return FALSE; // ������ �������� ����.
	sSrcFolder = sRemain + _T("\\") + sFolder + _T("\\");

	bExist = cFile.FindFile(sSrcFolder + _T("*.pcr"));
	if (!bExist) return FALSE; // pcr������ �������� ����.

	if (!Copy(sSrc, sDest)) return FALSE; // Copy ����.
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

CString CSimpleReelmap::GetTextArPcr(int nIdx) // nIdx : Up(0), Dn(1)
{
	int nNodeX = m_nMaxCol;
	int nNodeY = m_nMaxRow;
	int nStripY = nNodeY / MAX_STRIP;

	CString sData = _T("");
	CString sTemp, sVal;
	int i, j, k;

	int nCount = m_arPcr[nIdx].GetSize();
	CPcr Pcr;
	for (i = 0; i < nCount; i++)
	{
		if (i)	sData += _T("\r\n");		
		Pcr = m_arPcr[nIdx].GetAt(i);
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
					sVal.Format(_T("%2d\r\n"), pPanel[j][k]);	// �ҷ��ڵ带 2ĭ���� ����
				else
					sVal.Format(_T("%2d,"), pPanel[j][k]);	// �ҷ��ڵ带 2ĭ���� ����
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

CString CSimpleReelmap::GetTextArPcrYield(int nIdx) // nIdx : Up(0), Dn(1)
{
	int nNodeX = m_nMaxCol;
	int nNodeY = m_nMaxRow;
	int nStripY = nNodeY / MAX_STRIP;	

	CString sData = _T("");
	CString sTemp, sVal;
	int i, j, k;

	int nCount = m_arPcr[nIdx].GetSize();
	CPcrYield PcrYield;
	for (i = 0; i < nCount; i++)
	{
		if (i)	sData += _T("\r\n");
		PcrYield = m_arPcrYield[nIdx].GetAt(i);
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
		sTemp.Format(_T("A��[%%] : %.1f\tB��[%%] : %.1f\r\n"), 100.0 - dStripDefRatio[0], 100.0 - dStripDefRatio[1]);
		sData += sTemp;
		sTemp.Format(_T("C��[%%] : %.1f\tD��[%%] : %.1f\r\n"), 100.0 - dStripDefRatio[2], 100.0 - dStripDefRatio[3]);
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
	CPcrYield PcrYield[2];

	int nCount = m_arPcr[0].GetSize();
	for (i = 0; i < nCount; i++)
	{
		Pcr[1] = m_arPcr[0].GetAt(i);
		if (Pcr[1].GetSerial() == nSerial)
		{
			Pcr[1].Free();
			m_arPcr[0].SetAt(i, Pcr[0]); // �ε���(i)�� �� Pcr[0] �Է�

			if (i > 0)
			{
				PcrYield[1] = m_arPcrYield[0].GetAt(i-1);				// PrevPcrYield
				PcrYield[0].Init(nSerial, Pcr[0], m_nMaxRow, m_nMaxCol, 20.0, this, &PcrYield[1]);	// UpdatePcrYield()
			}
			else
				PcrYield[0].Init(nSerial, Pcr[0], m_nMaxRow, m_nMaxCol, 20.0, this);				// UpdatePcrYield()

			m_arPcrYield[0].SetAt(i, PcrYield[0]); // �ε���(i)�� �� PcrYield[0] �Է�

			bAdd = TRUE;
			break;
		}
	}
	if (!bAdd)
	{
		m_arPcr[0].Add(Pcr[0]);

		nCount = m_arPcrYield[0].GetSize();
		if (nCount > 0)
		{
			PcrYield[1] = m_arPcrYield[0].GetAt(nCount - 1);									// PrevPcrYield
			PcrYield[0].Init(nSerial, Pcr[0], m_nMaxRow, m_nMaxCol, 20.0, this, &PcrYield[1]);	// UpdatePcrYield()
		}
		else
			PcrYield[0].Init(nSerial, Pcr[0], m_nMaxRow, m_nMaxCol, 20.0, this);				// UpdatePcrYield()
		m_arPcrYield[0].Add(PcrYield[0]);
	}

	return TRUE;
}

BOOL CSimpleReelmap::Save()
{
	BOOL bRtn[2] = { 0, 0 };
	bRtn[0] = SaveRmap();
	bRtn[1] = SaveYield();

	return (bRtn[0] && bRtn[1]);
}

BOOL CSimpleReelmap::SaveRmap()
{
	CFile cfile;
	if (!cfile.Open(m_sPathRmap, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary | CFile::shareDenyNone, NULL))
	{
		AfxMessageBox(_T("Fail to save Reelmap."));
		return FALSE;
	}

	int nActionCode = m_nActionCode;
	int nNodeX = m_nMaxCol;
	int nNodeY = m_nMaxRow;
	int i, k;
	cfile.Write(&nActionCode, sizeof(int));
	cfile.Write(&nNodeY, sizeof(int));
	cfile.Write(&nNodeX, sizeof(int));
	int nCount = m_arPcr[0].GetSize();
	cfile.Write(&nCount, sizeof(int));
	CPcr Pcr;
	for (i = 0; i < nCount; i++)
	{
		Pcr = m_arPcr[0].GetAt(i);
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
	int nCount = m_arPcrYield[0].GetSize();
	cfile.Write(&nCount, sizeof(int));
	CPcrYield PcrYield;
	for (i = 0; i < nCount; i++)
	{
		PcrYield = m_arPcrYield[0].GetAt(i);
		int nSerial = PcrYield.GetSerial();
		COleDateTime time = PcrYield.GetDateTime();
		int nTotPcs = PcrYield.GetTotalPcs();		// ������ PCS
		int nGood = PcrYield.GetTotalGood();
		int nBad = PcrYield.GetTotalBad();
		int nTotStripOut = PcrYield.GetTotalStripOut();
		int nStripOut[MAX_STRIP], nDefStrip[MAX_STRIP];

		cfile.Write(&nSerial, sizeof(int));
		cfile.Write(&time, sizeof(COleDateTime));
		cfile.Write(&nTotPcs, sizeof(int));
		cfile.Write(&nGood, sizeof(int));
		cfile.Write(&nBad, sizeof(int));
		cfile.Write(&nTotStripOut, sizeof(int));

		for (k = 0; k < MAX_STRIP; k++)
		{
			nStripOut[k] = PcrYield.GetStripOut(k);
			nDefStrip[k] = PcrYield.GetStripTotalBad(k);
			cfile.Write(&nStripOut[k], sizeof(int));
			cfile.Write(&nDefStrip[k], sizeof(int));
		}
		int nStripDef[MAX_STRIP][MAX_DEF];
		int nDef[MAX_DEF];
		for (j = 0; j < MAX_STRIP; j++)
		{
			for (k = 0; k < MAX_DEF; k++)
			{
				if (!j)
				{
					nDef[k] = PcrYield.GetDefNum(k);
					cfile.Write(&nDef[k], sizeof(int));
				}
				nStripDef[j][k] = PcrYield.GetStripDefNum(j, k);
				cfile.Write(&nStripDef[j][k], sizeof(int));
			}
		}
	}
	cfile.Close();

	return TRUE;
}

BOOL CSimpleReelmap::Load()
{
	BOOL bRtn[2] = { 0, 0 };
	bRtn[0] = LoadRmap();
	bRtn[1] = LoadYield();

	return (bRtn[0] && bRtn[1]);
}

BOOL CSimpleReelmap::LoadRmap()
{
	int i, k;
	int nCount = m_arPcr[0].GetSize();
	if (nCount > 0)
	{
		CPcr Pcr;
		for (i = 0; i < nCount; i++)
		{
			Pcr = m_arPcr[0].GetAt(i);
			Pcr.Free();
		}
		m_arPcr[0].RemoveAll();
	}

	CFile cfile;
	if (!cfile.Open(m_sPathRmap, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone, NULL))
	{
		AfxMessageBox(_T("Fail to load Rmap."));
		return FALSE;
	}

	int nActionCode;
	int nNodeY;
	int nNodeX;
	int nTotalPcr;
	cfile.Read((void *)&nActionCode, sizeof(int));
	cfile.Read((void *)&nNodeY, sizeof(int));
	cfile.Read((void *)&nNodeX, sizeof(int));
	cfile.Read((void *)&nTotalPcr, sizeof(int));
	m_nActionCode = nActionCode;
	m_nMaxCol = nNodeX;
	m_nMaxRow = nNodeY;
	for (i = 0; i < nTotalPcr; i++)
	{
		int nSerial;
		COleDateTime time;
		int nTotDef;
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
		m_arPcr[0].Add(Pcr);
	}

	return TRUE;
}

BOOL CSimpleReelmap::LoadYield()
{
	int i, j, k;
	int nCount = m_arPcrYield[0].GetSize();
	if (nCount > 0)
		m_arPcrYield[0].RemoveAll();

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
		int nSerial, nTotPcs, nGood, nBad, nTotStripOut, nStripOut[MAX_STRIP], nDefStrip[MAX_STRIP];		// ������ PCS
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
			cfile.Read((void *)&nDefStrip[j], sizeof(int));
			PcrYield.SetStripOut(j, nStripOut[j]);
			PcrYield.SetStripTotalBad(j, nDefStrip[j]);
		}

		int nStripDef[MAX_STRIP][MAX_DEF];
		int nDef[MAX_DEF];
		for (j = 0; j < MAX_STRIP; j++)
		{
			for (k = 0; k < MAX_DEF; k++)
			{
				if (!j)
				{
					cfile.Read((void *)&nDef[k], sizeof(int));
					PcrYield.SetDefNum(j, nDef[k]);
				}
				cfile.Read((void *)&nStripDef[j][k], sizeof(int));
				PcrYield.SetStripDefNum(j, k, nStripDef[j][k]);
			}
		}

		m_arPcrYield[0].Add(PcrYield);
	}

	return TRUE;
}

BOOL CSimpleReelmap::LoadDefectTableIni()
{
	TCHAR szData[200];
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
	int nNodeX = m_nMaxCol;
	int nNodeY = m_nMaxRow;

	switch (m_nActionCode)	// 0 : Rotation / Mirror ���� ����(CAM Data ����), 1 : �¿� �̷�, 2 : ���� �̷�, 3 : 180 ȸ��, 4 : 270 ȸ��(CCW), 5 : 90 ȸ��(CW)
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
		if (nC % 2)	// Ȧ��.
			nR = nNodeY*(nC + 1) - 1 - nPcsId;
		else		// ¦��.
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

int CSimpleReelmap::MirrorLR(int nPcsId) // �¿� �̷���
{
	int nId, nCol, nRow, nC, nR;
	int nNodeX = m_nMaxCol; // 1 ~
	int nNodeY = m_nMaxRow; // 1 ~

	nCol = int(nPcsId / nNodeY); // 0 ~
	if (nCol % 2)
		nRow = nNodeY*(nCol + 1) - nPcsId - 1;
	else
		nRow = nPcsId - nNodeY*nCol; // 0 ~

	nR = nRow; // 0 ~
	nC = (nNodeX - 1) - nCol; // 0 ~
	if (nC % 2) // Ȧ�� : ���� ����
		nId = nNodeY*(nC + 1) - nR - 1;
	else		// ¦�� : ���� ����
		nId = nR + nNodeY*nC;

	return nId;
}

int CSimpleReelmap::MirrorUD(int nPcsId) // ���� �̷���
{
	int nId, nCol, nRow, nC, nR;
	int nNodeX = m_nMaxCol; // 1 ~
	int nNodeY = m_nMaxRow; // 1 ~

	nCol = int(nPcsId / nNodeY); // 0 ~
	if (nCol % 2)
		nRow = nPcsId - nNodeY*nCol; // 0 ~
	else
		nRow = nNodeY*(nCol + 1) - nPcsId - 1;

	nR = nRow; // 0 ~
	nC = nCol; // 0 ~
	if (nC % 2) // Ȧ�� : ���� ����
		nId = nNodeY*(nC + 1) - nR - 1;
	else		// ¦�� : ���� ����
		nId = nR + nNodeY*nC;

	return nId;
}

int CSimpleReelmap::Rotate180(int nPcsId) // 180�� ȸ��
{
	int nId, nCol, nRow, nC, nR;
	int nNodeX = m_nMaxCol; // 1 ~
	int nNodeY = m_nMaxRow; // 1 ~

	if (nNodeX % 2)		// Ȧ�� : ���� ����
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
	else				// ¦�� : ���� ����
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
	nLenth = WideCharToMultiByte(CP_ACP, 0, wszStr, -1, NULL, 0, NULL, NULL); //char* ���� ���ѱ��̸� ���� 

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
	return m_arPcr[0];
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
