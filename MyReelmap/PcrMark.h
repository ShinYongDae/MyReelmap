#pragma once
class CPcrMark
{
	CWnd* m_pParent;
	int m_nSerial;
	COleDateTime m_oleDate;
	int m_nTotalDef;
	int m_nTotalMark;
	int *m_pMarkedPcsId;			// Piece Number

	void SetSerial(int nSerial);
	void SetTotalDef(int nTotalDef);
	void SetDateTime(COleDateTime time);
	void SetTotalMark(int nTotalMark);

public:
	CPcrMark();
	~CPcrMark();

public:
	void Init(int nSerial, int nTotalDef, CWnd* pParent = NULL);
	void Free();
	void SetMarkedPcsId(int nMarkedPcsId);

	int GetSerial();
	COleDateTime GetDateTime();
	int GetTotalDef();
	int GetTotalMark();
	int GetMarkedPcsId(int nIdx);

protected:
};
typedef CArray <CPcrMark, CPcrMark> CArPcrMark;

