#pragma once

#define PATH_SHARE		_T("C:\\R2RSet\\Share")
#define PATH_BUFFER		_T("C:\\R2RSet\\Buffer")

typedef struct tagDefPcs
{
	int m_nCamId;			// Cam ID
	int m_nPcsId;			// Piece Number
	int m_nPosX;			// BadPointPosX
	int m_nPosY;			// BadPointPosY
	int m_nDefCode;			// BadCode
	int m_nCell;			// CellNum
	int m_nImgSz;			// ImageSize
	int m_nImg;				// ImageNum
	int m_nMk;				// MarkingCode (-2 : NoMarking)

	tagDefPcs()
	{
		m_nCamId = -1;		// Cam ID
		m_nPcsId = -1;		// Piece Number
		m_nPosX = 0;		// BadPointPosX
		m_nPosY = 0;		// BadPointPosY
		m_nDefCode = -1;	// BadCode
		m_nCell = -1;		// CellNum
		m_nImgSz = -1;		// ImageSize
		m_nImg = -1;		// ImageNum
		m_nMk = -1;			// MarkingCode (-2 : NoMarking)
	}

	~tagDefPcs()
	{
		;
	}
}stDefPcs;

// CPcr
class CPcr
{
	CString m_sPathFolderShare;
	int m_nSerial;
	COleDateTime m_oleDate;

	// Head info
	int m_nErrCode;			// Error Code			// 1(정상), -1(Align Error, 노광불량), -2(Lot End)
	CString m_sModel;		// Model
	CString m_sLayer;		// Layer
	CString m_sLot;			// Lot
	CString m_sItsCode;		// Its Code
	int m_nTotDef;			// Total Defect Numbers

	// Defect PCS info
	stDefPcs *m_pDefPcs;

	BOOL LoadPcr();
	void StringToChar(CString str, char* pCh); // char* returned must be deleted... 
	CString CharToString(const char *szStr);

public:
	CPcr();
	~CPcr();

	void Free();
	void Init(int nSerial, CString sPathFolder = _T("")); // LoadPcr(); //Load pcr file.

	int GetSerial();
	COleDateTime GetDateTime();
	int GetTotalDef();

	int GetCamId(int nIdx);
	int GetPcsId(int nIdx);
	int GetPosX(int nIdx);
	int GetPosY(int nIdx);
	int GetDefCode(int nIdx);
	int GetCell(int nIdx);
	int GetImageSize(int nIdx);
	int GetImageNum(int nIdx);
	int GetMarkingCode(int nIdx);

	void SetSerial(int nSerial);
	void SetDateTime(COleDateTime time);
	void SetTotalDef(int nTotalDef);

	void SetCamId(int nIdx, int nCamId);
	void SetPcsId(int nIdx, int nPcsId);
	void SetPosX(int nIdx, int nPosX);
	void SetPosY(int nIdx, int nPosY);
	void SetDefCode(int nIdx, int nDefCode);
	void SetCell(int nIdx, int nCell);
	void SetImageSize(int nIdx, int nImageSize);
	void SetImageNum(int nIdx, int nImageNum);
	void SetMarkingCode(int nIdx, int nMarkingCode);

protected:

};
typedef CArray <CPcr, CPcr> CArPcr;

