#pragma once

#ifndef MAX_DEF						
#define MAX_DEF	30
#endif

#ifndef MAX_STRIP						
#define MAX_STRIP	4
#endif

#include "Pcr.h"

class CPcrYield
{
	CWnd* m_pParent;
	int m_nSerial;
	COleDateTime m_oleDate;
	double m_dStripOutRatio;
	int m_nMaxRow;
	int m_nMaxCol;

	// 이전 PCR의 수율에서 누적된 수율 정보
	int m_nTotPcs, m_nGood, m_nBad;
	int m_nDef[MAX_DEF];
	int m_nTotSriptOut;
	int m_nStripOut[MAX_STRIP];
	int m_nDefStrip[MAX_STRIP];
	int m_nStripDef[MAX_STRIP][MAX_DEF];

	BOOL Load(CPcr& Pcr, CPcrYield* pPrevPcrYield = NULL);
	char GetCodeBigDef(int nIdx);
	char GetCodeSmallDef(int nIdx);

public:
	CPcrYield();
	~CPcrYield();

public:
	void Init(int nSerial, CPcr& Pcr, int nMaxRow, int nMaxCol, double dStripOutRatio = 20.0, CWnd* pParent = NULL, CPcrYield* pPrevPcrYield = NULL);
	int GetSerial();
	COleDateTime GetDateTime();
	int GetTotalPcs();
	int GetTotalGood();
	int GetTotalBad();
	int GetDefNum(int nDefCode);
	int GetTotalStripOut();
	int GetStripOut(int nIdxStrip);
	int GetStripTotalBad(int nIdxStrip);
	int GetStripDefNum(int nIdxStrip, int nDefCode);

	void SetSerial(int nSerial);
	void SetDateTime(COleDateTime time);
	void SetTotalPcs(int nTotalPcs);
	void SetTotalGood(int nTotalGood);
	void SetTotalBad(int nTotalBad);
	void SetDefNum(int nDefCode, int nDefNum);
	void SetTotalStripOut(int nTotalStripOut);
	void SetStripOut(int nIdxStrip, int nStripOut);
	void SetStripTotalBad(int nIdxStrip, int nTotalDefNum);
	void SetStripDefNum(int nIdxStrip, int nDefCode, int nDefNum);

protected:
};
typedef CArray <CPcrYield, CPcrYield> CArPcrYield;
