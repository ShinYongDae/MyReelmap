#pragma once
#include <thread>

#include "Pcr.h"

#define MAX_STRIP	4

// CSimpleReelmap

class CSimpleReelmap : public CWnd
{
	DECLARE_DYNAMIC(CSimpleReelmap)

	CWnd* m_pParent;
	HWND m_hParent;

	CString m_sPath;
	BOOL CreateWndForm(DWORD dwStyle);

	BOOL m_bThreadAlive, m_bThreadStateEnd;
	std::thread t1;
	void ThreadStart();
	void ThreadStop();

	CArPcr m_arPcr[2];	// PCR파일 Array 데이터
	BOOL m_bLock;
	int m_nMaxRow, m_nMaxCol, m_nActionCode;

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

public:
	CSimpleReelmap(CString sPath, CWnd* pParent = NULL);
	virtual ~CSimpleReelmap();

	static void ProcThrd(const LPVOID lpContext);

	void Init(int nMaxRow, int nMaxCol, int nActionCode = 0);
	CString GetTextArPcr(int nIdx);
	BOOL GetMatrix(int nPcsId, int &nR, int &nC);
	BOOL Save();
	BOOL Load();

protected:
	void ThreadEnd();
	BOOL ProcReelmap();
	BOOL ThreadIsAlive();

protected:
	DECLARE_MESSAGE_MAP()
};


