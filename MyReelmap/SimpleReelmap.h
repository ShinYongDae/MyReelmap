#pragma once
#include <thread>

#include "Pcr.h"
#include "PcrYield.h"

#ifndef MAX_STRIP
#define MAX_STRIP	4
#endif

#ifndef MAX_DEF
#define MAX_DEF		30
#endif

#define PATH_CONFIG				_T("C:\\R2RSet\\Config.ini")

// CSimpleReelmap

class CSimpleReelmap : public CWnd
{
	DECLARE_DYNAMIC(CSimpleReelmap)

	CWnd* m_pParent;
	HWND m_hParent;

	CString m_sPathRmap, m_sPathYield;
	BOOL CreateWndForm(DWORD dwStyle);

	BOOL m_bThreadAlive, m_bThreadStateEnd;
	std::thread t1;
	void ThreadStart();
	void ThreadStop();

	CArPcr m_arPcr[2];				// PCR파일 Array : 데이터 [0]-Up, [1]-Dn
	CArPcrYield m_arPcrYield[2];	// PCR수율 Array : 데이터 [0]-Up, [1]-Dn
	BOOL m_bLock;
	int m_nMaxRow, m_nMaxCol, m_nActionCode;

	COLORREF m_rgbDef[MAX_DEF];
	CString m_sKorDef[MAX_DEF], m_sEngDef[MAX_DEF];
	char m_cBigDef[MAX_DEF], m_cSmallDef[MAX_DEF];
	int m_nOdr[MAX_DEF];
	int m_nBkColor[3]; //RGB

	BOOL IsShare(int &nSerial);
	BOOL Add(int nSerial);
	BOOL ShiftToBuffer(int nSerial);

	BOOL CopyPcr(CString sSrc, CString sDest);
	CString ParseFolderName(CString sPathName, CString &sRemainPathName);
	BOOL Copy(CString sSource, CString sDest);
	BOOL DelPcr(CString sPath);
	int MirrorLR(int nPcsId); // 좌우 미러링
	int MirrorUD(int nPcsId); // 상하 미러링
	int Rotate180(int nPcsId); // 180도 회전
	void StringToChar(CString str, char* pCh); // char* returned must be deleted... 
	CString CharToString(const char *szStr);
	void Free();

	int m_nCMstTotPcs;
	BOOL LoadDefectTableIni();
	void InitColor();
	BOOL LoadRmap();
	BOOL LoadYield();
	BOOL SaveRmap();
	BOOL SaveYield();

public:
	CSimpleReelmap(CString sPathRmap, CString sPathYield, CWnd* pParent = NULL);
	virtual ~CSimpleReelmap();
	static void ProcThrd(const LPVOID lpContext);

	void Init(int nMaxRow, int nMaxCol, int nActionCode = 0);
	CString GetTextArPcr(int nIdx);			// nIdx : Up(0), Dn(1)
	CString GetTextArPcrYield(int nIdx);	// nIdx : Up(0), Dn(1)
	BOOL GetMatrix(int nPcsId, int &nR, int &nC);
	BOOL Save();
	BOOL Load();

	CArPcr& GetAddrArPcr();
	COLORREF GetDefColor(int nDefCode);
	char GetCodeBigDef(int nIdx);
	char GetCodeSmallDef(int nIdx);

protected:
	void ThreadEnd();
	BOOL ProcReelmap();
	BOOL ThreadIsAlive();

protected:
	DECLARE_MESSAGE_MAP()
};


