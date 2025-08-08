#include "stdafx.h"
#include "PcrYield.h"

#include "SimpleReelmap.h"

CPcrYield::CPcrYield()
{
	m_pParent = NULL;
	m_nSerial = -1;

	m_nTotPcs = 0;
	m_nGood = 0;
	m_nBad = 0;
	m_nTotSriptOut = 0;
	m_dStripOutRatio = 20.0; // [%]
	m_nMaxRow = 0;
	m_nMaxCol = 0;

	for (int j = 0; j < MAX_STRIP; j++)
	{
		m_nStripOut[j] = 0;
		m_nDefStrip[j] = 0;
		for (int k = 0; k < MAX_DEF; k++)
		{
			if(!j) m_nDef[k] = 0;
			m_nStripDef[j][k] = 0;
		}
	}
}

CPcrYield::~CPcrYield()
{
}

void CPcrYield::Init(int nSerial, CPcr& Pcr, int nMaxRow, int nMaxCol, double dStripOutRatio, CWnd* pParent,CPcrYield* pPrevPcrYield)
{
	m_pParent = pParent;
	m_nSerial = nSerial;
	m_oleDate = COleDateTime::GetCurrentTime();
	m_dStripOutRatio = dStripOutRatio; // [%]
	m_nMaxRow = nMaxRow;
	m_nMaxCol = nMaxCol;

	Load(Pcr, pPrevPcrYield);
}

BOOL CPcrYield::Load(CPcr& Pcr, CPcrYield* pPrevPcrYield)
{
	CSimpleReelmap* pParnet = (CSimpleReelmap*)m_pParent;

	double dStOutRto = m_dStripOutRatio / 100.0;
	int nTotPcs = 0, nGood = 0, nBad = 0;
	int nTotSriptOut = 0;

	int nDefStrip[MAX_STRIP], nStripOut[MAX_STRIP], nDef[MAX_DEF], nStripDef[MAX_STRIP][MAX_DEF];
	nDefStrip[0] = 0; nDefStrip[1] = 0; nDefStrip[2] = 0; nDefStrip[3] = 0;
	nStripOut[0] = 0; nStripOut[1] = 0; nStripOut[2] = 0; nStripOut[3] = 0;

	int i, j, k, nRow, nCol;
	for (k = 0; k < MAX_DEF; k++)
	{
		nDef[k] = 0;

		for (j = 0; j < MAX_STRIP; j++)
		{
			nStripDef[j][k] = 0;
		}
	}

	nTotPcs = m_nMaxRow * m_nMaxCol;
	nBad = Pcr.GetTotalDef();
	nGood = nTotPcs - nBad;

	int nStrip = 0;
	for (i = 0; i < nBad; i++)
	{
		int nDefCode = Pcr.GetDefCode(i);
		nDef[nDefCode]++;

		int nPcsId = Pcr.GetPcsId(i);
		pParnet->GetMatrix(nPcsId, nRow, nCol);
		nStrip = int(nRow / (m_nMaxRow / MAX_STRIP));
		if (nStrip > -1 && nStrip < MAX_STRIP)
		{
			nDefStrip[nStrip]++;
			nStripDef[nStrip][nDefCode]++;
		}
	}

	int nStripPcs = nTotPcs / MAX_STRIP;
	for (nStrip = 0; nStrip < MAX_STRIP; nStrip++)
	{
		if (nDefStrip[nStrip] >= nStripPcs * dStOutRto)
			nStripOut[nStrip]++;
	}

	if (pPrevPcrYield)
	{
		m_nTotPcs = pPrevPcrYield->GetTotalPcs() + nTotPcs;	// 누적된 총 피스 수
		m_nGood = pPrevPcrYield->GetTotalGood() + nGood;	// 누적된 총 양품 피스 수
		m_nBad = pPrevPcrYield->GetTotalBad() + nBad;		// 누적된 총 불량 피스 수
	}
	else
	{
		m_nTotPcs = nTotPcs;	// 누적된 총 피스 수
		m_nGood = nGood;	// 누적된 총 양품 피스 수
		m_nBad = nBad;		// 누적된 총 불량 피스 수
	}

	int nTotStriptOut = 0;
	for (j = 0; j < MAX_STRIP; j++)
	{
		if (pPrevPcrYield)
		{
			m_nDefStrip[j] = pPrevPcrYield->GetStripTotalBad(j) + nDefStrip[j];
			m_nStripOut[j] = pPrevPcrYield->GetStripOut(j) + nStripOut[j];
			nTotStriptOut += nStripOut[j];
			for (k = 1; k < MAX_DEF; k++)
			{
				if (!j) m_nDef[k] = pPrevPcrYield->GetDefNum(k) + nDef[k];
				m_nStripDef[j][k] = pPrevPcrYield->GetStripDefNum(j, k) + nStripDef[j][k];
			}
		}
		else
		{
			m_nDefStrip[j] = nDefStrip[j];
			m_nStripOut[j] = nStripOut[j];
			nTotStriptOut += nStripOut[j];
			for (k = 1; k < MAX_DEF; k++)
			{
				if (!j) m_nDef[k] = nDef[k];
				m_nStripDef[j][k] = nStripDef[j][k];
			}
		}
	}

	if (pPrevPcrYield)
		m_nTotSriptOut = pPrevPcrYield->GetTotalStripOut() + nTotSriptOut;
	else
		m_nTotSriptOut = nTotSriptOut;

	return TRUE;
}

int CPcrYield::GetSerial()
{
	return m_nSerial;
}

COleDateTime CPcrYield::GetDateTime()
{
	return m_oleDate;
}

// 이전 PCR의 수율에서 누적된 수율 정보
int CPcrYield::GetTotalPcs()
{
	return m_nTotPcs;
}

int CPcrYield::GetTotalGood()
{
	return m_nGood;
}

int CPcrYield::GetTotalBad()
{
	return m_nBad;
}

int CPcrYield::GetDefNum(int nDefCode)
{
	return m_nDef[nDefCode];
}

int CPcrYield::GetTotalStripOut()
{
	return m_nTotSriptOut;
}

int CPcrYield::GetStripOut(int nIdxStrip)
{
	return m_nStripOut[nIdxStrip];
}

int CPcrYield::GetStripTotalBad(int nIdxStrip)
{
	return m_nDefStrip[nIdxStrip];
}

int CPcrYield::GetStripDefNum(int nIdxStrip, int nDefCode)
{
	return m_nStripDef[nIdxStrip][nDefCode];
}

char CPcrYield::GetCodeBigDef(int nIdx)
{
	CSimpleReelmap* pParnet = (CSimpleReelmap*)m_pParent;
	if(pParnet)
		return pParnet->GetCodeBigDef(nIdx);
	return 0;
}

char CPcrYield::GetCodeSmallDef(int nIdx)
{
	CSimpleReelmap* pParnet = (CSimpleReelmap*)m_pParent;
	if (pParnet)
		return pParnet->GetCodeSmallDef(nIdx);
	return 0;
}

void CPcrYield::SetSerial(int nSerial)
{
	m_nSerial = nSerial;
}

void CPcrYield::SetDateTime(COleDateTime time)
{
	m_oleDate = time;
}

void CPcrYield::SetTotalPcs(int nTotalPcs)
{
	m_nTotPcs = nTotalPcs;
}

void CPcrYield::SetTotalGood(int nTotalGood)
{
	m_nGood = nTotalGood;
}

void CPcrYield::SetTotalBad(int nTotalBad)
{
	m_nBad = nTotalBad;
}

void CPcrYield::SetDefNum(int nDefCode, int nDefNum)
{
	m_nDef[nDefCode] = nDefNum;
}

void CPcrYield::SetTotalStripOut(int nTotalStripOut)
{
	m_nTotSriptOut = nTotalStripOut;
}

void CPcrYield::SetStripOut(int nIdxStrip, int nStripOut)
{
	m_nStripOut[nIdxStrip] = nStripOut;
}

void CPcrYield::SetStripTotalBad(int nIdxStrip, int nTotalDefNum)
{
	m_nDefStrip[nIdxStrip] = nTotalDefNum;
}

void CPcrYield::SetStripDefNum(int nIdxStrip, int nDefCode, int nDefNum)
{
	m_nStripDef[nIdxStrip][nDefCode] = nDefNum;
}

