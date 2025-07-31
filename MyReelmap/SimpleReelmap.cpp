// SimpleReelmap.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MyReelmap.h"
#include "SimpleReelmap.h"


// CSimpleReelmap

IMPLEMENT_DYNAMIC(CSimpleReelmap, CWnd)

CSimpleReelmap::CSimpleReelmap(CString sPath, CWnd* pParent/*=NULL*/)
{
	m_pParent = pParent;
	if (pParent)
		m_hParent = pParent->GetSafeHwnd();
	m_sPath = sPath;

	m_bLock = FALSE;
	m_nMaxRow = 0;
	m_nMaxCol = 0;
	m_nActionCode = 0;

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
		//CPcr Pcr;
		//Pcr.Init(nSerial);
		//m_arPcr[0].Add(Pcr);
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

CString CSimpleReelmap::GetTextArPcr(int nIdx)
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

BOOL CSimpleReelmap::Add(int nSerial)
{
	int i;
	BOOL bAdd = FALSE;
	CPcr Pcr[2];
	Pcr[0].Init(nSerial);
	int nCount = m_arPcr[0].GetSize();
	for (i = 0; i < nCount; i++)
	{
		Pcr[1] = m_arPcr[0].GetAt(i);
		if (Pcr[1].GetSerial() == nSerial)
		{
			Pcr[1].Free();
			m_arPcr[0].SetAt(i, Pcr[0]); // �ε���(i)�� �� Pcr[0] �Է�
			bAdd = TRUE;
			break;
		}
	}
	if(!bAdd)
		m_arPcr[0].Add(Pcr[0]);

	return TRUE;
}

BOOL CSimpleReelmap::Save()
{
	CFile cfile;
	if (!cfile.Open(m_sPath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary | CFile::shareDenyNone, NULL))
	{
		AfxMessageBox(_T("Fail to save."));
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

BOOL CSimpleReelmap::Load()
{
	int i, k;
	int nCount = m_arPcr[1].GetSize();
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

	CFile cfile;
	if (!cfile.Open(m_sPath, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone, NULL))
	{
		AfxMessageBox(_T("Fail to load."));
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
		m_arPcr[1].Add(Pcr);
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
