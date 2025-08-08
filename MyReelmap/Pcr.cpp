#include "stdafx.h"
#include "Pcr.h"


CPcr::CPcr()
{
	m_pDefPcs = NULL;
}

CPcr::~CPcr()
{
	//if (m_pDefPcs)
	//{
	//	delete [] m_pDefPcs;
	//	m_pDefPcs = NULL;
	//}
}

void CPcr::Free()
{
	if (m_pDefPcs)
	{
		delete[] m_pDefPcs;
		m_pDefPcs = NULL;
	}
}

void CPcr::Init(int nSerial, CString sPathFolder)
{
	m_sPathFolderShare = sPathFolder;
	m_nSerial = nSerial;
	m_oleDate = COleDateTime::GetCurrentTime();
	m_pDefPcs = NULL;
	LoadPcr();
}

BOOL CPcr::LoadPcr()
{
	int nTotDef;

	FILE *fp;
	char FileD[MAX_PATH];
	char *FileData;
	int nFileSize, nRSize;
	CString sPath, sFileData;

	m_sPathFolderShare = PATH_SHARE;
	sPath.Format(_T("%s\\%04d.pcr"), m_sPathFolderShare, m_nSerial);
	StringToChar(sPath, FileD);
	if ((fp = fopen(FileD, "r")) != NULL)
	{
		fseek(fp, 0, SEEK_END);
		nFileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		/* Allocate space for a path name */
		FileData = (char*)calloc(nFileSize + 1, sizeof(char));
		nRSize = fread(FileData, sizeof(char), nFileSize, fp);
		fclose(fp);

		sFileData = CharToString(FileData);
		free(FileData);
	}
	else
	{
		sFileData.Format(_T("PCR 파일이 존재하지 않습니다.\r\n%s"), sPath);
		AfxMessageBox(sFileData);
		return FALSE;
	}

	// Extract Head information
	int nTemp;
	CString sHeaderErrorInfo, sModel, sLayer, sLot, sItsCode, sTotalBadPieceNum;
	CString sCamID, sPieceID, sBadPointPosX, sBadPointPosY, sBadName;
	CString sCellNum, sImageSize, sImageNum, sMarkingCode;

	// Error Code											// 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	nTemp = sFileData.Find(',', 0);
	sHeaderErrorInfo = sFileData.Left(nTemp);
	sFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;

	// Model
	nTemp = sFileData.Find(',', 0);
	sModel = sFileData.Left(nTemp);
	sFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;

	// Layer
	nTemp = sFileData.Find(',', 0);
	sLayer = sFileData.Left(nTemp);
	sFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;

	// Lot
	nTemp = sFileData.Find(',', 0);
	sLot = sFileData.Left(nTemp);
	sFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;

	// Its Code
	nTemp = sFileData.Find('\n', 0);
	sItsCode = sFileData.Left(nTemp);
	sFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;

	// Total Defect Numbers
	nTemp = sFileData.Find('\n', 0);
	sTotalBadPieceNum = sFileData.Left(nTemp);
	sFileData.Delete(0, nTemp + 1);
	nFileSize = nFileSize - nTemp - 1;


	// Load to local memory
	m_nErrCode = _tstoi(sHeaderErrorInfo);;			// Error Code			// 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	m_sModel = sModel;								// Model
	m_sLayer = sLayer;								// Layer
	m_sLot = sLot;									// Lot
	m_sItsCode = sItsCode;							// Its Code
	m_nTotDef = _tstoi(sTotalBadPieceNum);			// Total Defect Numbers

	if (m_nTotDef == 0)
		return TRUE;
	else if (m_nTotDef < 0)
		return FALSE;

	m_pDefPcs = new stDefPcs[m_nTotDef];
	for (int i = 0; i < m_nTotDef; i++)
	{
		// Cam ID
		nTemp = sFileData.Find(',', 0);
		sCamID = sFileData.Left(nTemp);
		sFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;

		// Piece Number
		nTemp = sFileData.Find(',', 0);
		sPieceID = sFileData.Left(nTemp);
		sFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;

		// BadPointPosX
		nTemp = sFileData.Find(',', 0);
		sBadPointPosX = sFileData.Left(nTemp);
		sFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;

		// BadPointPosY
		nTemp = sFileData.Find(',', 0);
		sBadPointPosY = sFileData.Left(nTemp);
		sFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;

		// BadName
		nTemp = sFileData.Find(',', 0);
		sBadName = sFileData.Left(nTemp);
		sFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;

		// CellNum
		nTemp = sFileData.Find(',', 0);
		sCellNum = sFileData.Left(nTemp);
		sFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;

		// ImageSize
		nTemp = sFileData.Find(',', 0);
		sImageSize = sFileData.Left(nTemp);
		sFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;

		// ImageNum
		nTemp = sFileData.Find(',', 0);
		sImageNum = sFileData.Left(nTemp);
		sFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;

		// sMarkingCode : -2 (NoMarking)
		nTemp = sFileData.Find('\n', 0);
		sMarkingCode = sFileData.Left(nTemp);
		sFileData.Delete(0, nTemp + 1);
		nFileSize = nFileSize - nTemp - 1;

		m_pDefPcs[i].m_nCamId = _tstoi(sCamID);
		m_pDefPcs[i].m_nPcsId = _tstoi(sPieceID);
		m_pDefPcs[i].m_nPosX = _tstoi(sBadPointPosX);
		m_pDefPcs[i].m_nPosY = _tstoi(sBadPointPosY);
		m_pDefPcs[i].m_nDefCode = _tstoi(sBadName);
		m_pDefPcs[i].m_nCell = _tstoi(sCellNum);
		m_pDefPcs[i].m_nImgSz = _tstoi(sImageSize);
		m_pDefPcs[i].m_nImg = _tstoi(sImageNum);
		m_pDefPcs[i].m_nMk = _tstoi(sMarkingCode);
	}
	return TRUE;
}

void CPcr::StringToChar(CString str, char* pCh) // char* returned must be deleted... 
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

CString CPcr::CharToString(const char *szStr)
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

int CPcr::GetSerial()
{
	return m_nSerial;
}

COleDateTime CPcr::GetDateTime()
{
	return m_oleDate;
}

int CPcr::GetTotalDef()
{
	return m_nTotDef;
}

int CPcr::GetCamId(int nIdx)
{
	return m_pDefPcs[nIdx].m_nCamId;
}

int CPcr::GetPcsId(int nIdx)
{
	return m_pDefPcs[nIdx].m_nPcsId;
}

int CPcr::GetPosX(int nIdx)
{
	return m_pDefPcs[nIdx].m_nPosX;
}

int CPcr::GetPosY(int nIdx)
{
	return m_pDefPcs[nIdx].m_nPosY;
}

int CPcr::GetDefCode(int nIdx)
{
	return m_pDefPcs[nIdx].m_nDefCode;
}

int CPcr::GetCell(int nIdx)
{
	return m_pDefPcs[nIdx].m_nCell;
}

int CPcr::GetImageSize(int nIdx)
{
	return m_pDefPcs[nIdx].m_nImgSz;
}

int CPcr::GetImageNum(int nIdx)
{
	return m_pDefPcs[nIdx].m_nImg;
}

int CPcr::GetMarkingCode(int nIdx)
{
	return m_pDefPcs[nIdx].m_nMk;
}

void CPcr::SetSerial(int nSerial)
{
	m_nSerial = nSerial;
}

void CPcr::SetDateTime(COleDateTime time)
{
	m_oleDate = time;
}

void CPcr::SetTotalDef(int nTotalDef)
{
	m_nTotDef = nTotalDef;
	if (nTotalDef < 1)
		return;
	Free();
	m_pDefPcs = new stDefPcs[m_nTotDef];
}

void CPcr::SetCamId(int nIdx, int nCamId)
{
	m_pDefPcs[nIdx].m_nCamId = nCamId;
}

void CPcr::SetPcsId(int nIdx, int nPcsId)
{
	m_pDefPcs[nIdx].m_nPcsId = nPcsId;
}

void CPcr::SetPosX(int nIdx, int nPosX)
{
	m_pDefPcs[nIdx].m_nPosX = nPosX;
}

void CPcr::SetPosY(int nIdx, int nPosY)
{
	m_pDefPcs[nIdx].m_nPosY = nPosY;
}

void CPcr::SetDefCode(int nIdx, int nDefCode)
{
	m_pDefPcs[nIdx].m_nDefCode = nDefCode;
}

void CPcr::SetCell(int nIdx, int nCell)
{
	m_pDefPcs[nIdx].m_nCell = nCell;
}

void CPcr::SetImageSize(int nIdx, int nImageSize)
{
	m_pDefPcs[nIdx].m_nImgSz = nImageSize;
}

void CPcr::SetImageNum(int nIdx, int nImageNum)
{
	m_pDefPcs[nIdx].m_nImg = nImageNum;
}

void CPcr::SetMarkingCode(int nIdx, int nMarkingCode)
{
	m_pDefPcs[nIdx].m_nMk = nMarkingCode;
}
