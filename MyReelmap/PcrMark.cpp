#include "stdafx.h"
#include "PcrMark.h"


CPcrMark::CPcrMark()
{
	m_pMarkedPcsId = NULL;
	m_nTotalMark = 0;
}


CPcrMark::~CPcrMark()
{
}

void CPcrMark::Free()
{
	if (m_pMarkedPcsId)
	{
		delete[] m_pMarkedPcsId;
		m_pMarkedPcsId = NULL;
	}
}

void CPcrMark::Init(int nSerial, int nTotalDef, CWnd* pParent)
{
	m_pParent = pParent;

	m_nSerial = nSerial;
	SetTotalDef(nTotalDef);
	m_oleDate = COleDateTime::GetCurrentTime();
}

void CPcrMark::SetSerial(int nSerial)
{
	m_nSerial = nSerial;
}

void CPcrMark::SetDateTime(COleDateTime time)
{
	m_oleDate = time;
}

void CPcrMark::SetTotalDef(int nTotalDef)
{
	m_nTotalDef = nTotalDef;
	if (m_pMarkedPcsId)
	{
		delete[] m_pMarkedPcsId;
		m_pMarkedPcsId = NULL;
	}

	if (!m_pMarkedPcsId)
	{
		if (nTotalDef > 0)
		{
			m_pMarkedPcsId = new int[nTotalDef];
			m_nTotalMark = 0;
		}
	}
}

void CPcrMark::SetTotalMark(int nTotalMark)
{
	m_nTotalMark = nTotalMark;
}

void CPcrMark::SetMarkedPcsId(int nMarkedPcsId)
{
	if (m_nTotalDef <= 0 || nMarkedPcsId < 0)
		return;
	if (m_pMarkedPcsId)
	{
		m_pMarkedPcsId[m_nTotalMark] = nMarkedPcsId;
		m_nTotalMark++;
	}
}

int CPcrMark::GetSerial()
{
	return m_nSerial;
}

COleDateTime CPcrMark::GetDateTime()
{
	return m_oleDate;
}

int CPcrMark::GetTotalDef()
{
	return m_nTotalDef;
}

int CPcrMark::GetTotalMark()
{
	return m_nTotalMark;
}

int CPcrMark::GetMarkedPcsId(int nIdx)
{
	return m_pMarkedPcsId[nIdx];
}
